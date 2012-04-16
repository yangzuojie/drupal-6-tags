<?php

/**
 * tags/manage
 */
function tags_manage_form(&$form_state) {

  $form = array('#tree' => TRUE, '#parent_fields' => TRUE);

  $path = drupal_get_path('module', 'tags');

  drupal_add_css($path . '/css/style.css', 'header');

  drupal_add_js($path . '/js/tags.js');

  global $user;

  $result = db_query("SELECT v.* FROM {vocabulary} v LEFT JOIN {vocabulary_user} vu ON v.vid = vu.vid WHERE vu.uid = %d", $user->uid);
  $vocabulary_options = array(0 => t('- Please choose -'));
  $vid_list = array();
  while ($vocabulary = db_fetch_object($result)) {
    $vocabulary_options[$vocabulary->vid] = $vocabulary->name;
    $vid_list[] = $vocabulary->vid;
  }

  $form['vocabulary'] = array(
      '#id' => 'tags-vocabulary-select',
      '#type' => 'select',
      '#options' => $vocabulary_options,
    );

  $form[''] = array('#value' => '<div id="tags-term-div"></div>');

  $form['add'] = array('#type' => 'submit', '#value' => t('Add'));

  $tree = tags_get_tree($vid_list);

  // Build the actual form.
  foreach ($tree as $term) {
    // Save the term for the current page so we don't have to load it a second time.
    $key = 'tid:'. $term->tid;

    $form[$key]['#term'] = (array)$term;

    if (isset($term->parents)) {
      $form[$key]['#term']['parent'] = $term->parent = $term->parents[0];
      unset($form[$key]['#term']['parents'], $term->parents);
    }

    if(isset($form_state['values'][$key])) {
      $form[$key]['#term']['depth'] = $form_state['values'][$key]['depth'];
      $form[$key]['#term']['parent'] = $form_state['values'][$key]['parent'];
    }

    $form[$key]['view'] = array('#value' => l($term->name, "taxonomy/term/$term->tid"));

    $form['#parent_fields'] = TRUE;
    $form[$key]['tid'] = array(
      '#type' => 'hidden',
      '#value' => $term->tid
    );
    $form[$key]['parent'] = array(
      '#type' => 'hidden',
      // Yes, default_value on a hidden. It needs to be changeable by the javascript.
      '#default_value' => $form[$key]['#term']['parent'],
    );
    $form[$key]['depth'] = array(
      '#type' => 'hidden',
      // Same as above, the depth is modified by javascript, so it's a default_value.
      '#default_value' => $form[$key]['#term']['depth'],
    );
  }

  if(!empty($form_state['values'])) {
    foreach($form as $key => $value) {
      if(is_array($value) && isset($value['#term']) && $value['parent']['#default_value']) {
        unset($form[$key]);
        $form = array_insert_after('tid:' . $value['parent']['#default_value'], $form, $key, $value);
      }
    }
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'), '#weight' => PHP_INT_MAX);

  return $form;
}

/**
 *
 */
function theme_tags_manage_form($form) {

  $rows = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#term'])) {
      $term = &$form[$key];

      $row = array();
      $row[] = (isset($term['#term']['depth']) && $term['#term']['depth'] > 0 ? theme('indentation', $term['#term']['depth']) : '') . drupal_render($term['view']);
      if ($form['#parent_fields']) {
        $term['tid']['#attributes']['class'] = 'term-id';
        $term['parent']['#attributes']['class'] = 'term-parent';
        $term['depth']['#attributes']['class'] = 'term-depth';
        $row[0] .= drupal_render($term['parent']) . drupal_render($term['tid']) . drupal_render($term['depth']);
      }

      $row = array('data' => $row, 'class' => 'draggable');
      $rows[$key] = $row;
    }
  }

  $header = array(t('Name'));

  drupal_add_tabledrag('tags', 'match', 'parent', 'term-parent', 'term-parent', 'term-id', FALSE);
  drupal_add_tabledrag('tags', 'depth', 'group', 'term-depth', NULL, NULL, FALSE);

  $form['table'] = array('#value' => theme('table', $header, $rows, array('id' => 'tags')), '#weight' => PHP_INT_MAX-1);

  return drupal_render($form);
}

/**
 *
 */
function tags_manage_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * tags/vocabulary/%/ajax
 */
function tags_manage_vocabulary_ajax($vid) {
  $term_list = taxonomy_get_tree($vid);
  echo theme('tags_manage_vocabulary_ajax', array('term_list' => $term_list));
}

/**
 *
 */
function tags_get_tree($vid_list, $parent = 0, $depth = -1, $max_depth = NULL) {
  static $tags_children, $tags_parents, $tags_terms;

  $vid = implode('-', $vid_list);

  $depth++;

  // We cache trees, so it's not CPU-intensive to call get_tree() on a term
  // and its children, too.
  if (!isset($tags_children[$vid])) {
    $tags_children[$vid] = array();

    $result = db_query(db_rewrite_sql('SELECT t.tid, t.*, parent FROM {term_data} t INNER JOIN {tag_hierarchy} h ON t.tid = h.tid WHERE t.vid IN (%s) ORDER BY weight, name', 't', 'tid'), implode(',', $vid_list));
    while ($term = db_fetch_object($result)) {
      $tags_children[$vid][$term->parent][] = $term->tid;
      $tags_parents[$vid][$term->tid][] = $term->parent;
      $tags_terms[$vid][$term->tid] = $term;
    }
  }

  $max_depth = (is_null($max_depth)) ? count($tags_children[$vid]) : $max_depth;
  $tree = array();
  if ($max_depth > $depth && !empty($tags_children[$vid][$parent])) {
    foreach ($tags_children[$vid][$parent] as $child) {
      $term = drupal_clone($tags_terms[$vid][$child]);
      $term->depth = $depth;
      // The "parent" attribute is not useful, as it would show one parent only.
      unset($term->parent);
      $term->parents = $parents[$vid][$child];
      $tree[] = $term;
      if (!empty($tags_children[$vid][$child])) {
        $tree = array_merge($tree, tags_get_tree($vid_list, $child, $depth, $max_depth));
      }
    }
  }

  return $tree;
}

/**
 *
 */
function array_insert_after($key, array &$array, $new_key, $new_value) {
  if (array_key_exists($key, $array)) {
    $new = array();
    foreach ($array as $k => $value) {
      $new[$k] = $value;
      if ($k === $key) {
        $new[$new_key] = $new_value;
      }
    }
    return $new;
  }
  return FALSE;
}