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
      '#title' => t('Vocabularies'),
      '#id' => 'tags-vocabulary-select',
      '#type' => 'select',
      '#options' => $vocabulary_options,
    );

  $form[''] = array('#value' => '<div id="tags-term-div"></div>');

  $form['add'] = array('#type' => 'submit', '#value' => t('Add'), '#id' => 'add-term-submit', '#attributes' => array('style' => 'display:none'));

  $tree = tags_get_tree($vid_list);

  // Build the actual form.
  foreach ($tree as $term) {
    // Save the term for the current page so we don't have to load it a second time.
    _tags_build_term_form($form, $term);
  }

  if(!empty($form_state['values'])) {
    $term_list = array();
    foreach($form_state['values'] as $key => $value) {
      if(strpos($key, 'tid:') !== false) {
        $term = taxonomy_get_term($value['tid']);
        $term->parent = $value['parent'];
        $term->depth = $value['depth'];

        _tags_build_term_form($form, $term);
      }
    }
  }

  if(!empty($form_state['clicked_button']['#post']['add_term_id'])) {
    foreach($form_state['clicked_button']['#post']['add_term_id'] as $add_term_id) {
      _tags_build_term_form($form, taxonomy_get_term($add_term_id));
    }
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'), '#weight' => PHP_INT_MAX);

  return $form;
}

/**
 *
 */
function _tags_build_term_form(&$form, $term) {
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

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No terms available.')));
  } else {
    drupal_add_tabledrag('tags', 'match', 'parent', 'term-parent', 'term-parent', 'term-id', FALSE);
    drupal_add_tabledrag('tags', 'depth', 'group', 'term-depth', NULL, NULL, FALSE);
  }

  $header = array(t('Name'));

  $form['table'] = array('#value' => theme('table', $header, $rows, array('id' => 'tags')), '#weight' => PHP_INT_MAX-1);

  return drupal_render($form);
}

/**
 *
 */
function tags_manage_form_submit($form, &$form_state) {

  $order = array_flip(array_keys($form['#post'])); // Get the $_POST order.
  $form_state['values'] = array_merge($order, $form_state['values']); // Update our original form with the new order.

  if($form_state['clicked_button']['#value'] == t('Add')) {
    $form_state['rebuild'] = TRUE;
  } else {
    global $user;
    db_query('DELETE FROM {tag_hierarchy} WHERE TID IN (SELECT tid FROM {term_data} WHERE vid IN (SELECT vid FROM {vocabulary_user} WHERE uid = %d))', $user->uid);
    foreach($form_state['values'] as $key => $value) {
      if(strpos($key, 'tid:') !== false) {
        db_query("INSERT INTO {tag_hierarchy} (tid, parent) VALUES (%d, %d)", $value['tid'], $value['parent']);
      }
    }

    drupal_set_message('Save success');
  }
}

/**
 * tags/vocabulary/%/ajax
 */
function tags_manage_vocabulary_ajax($vid) {
  $term_list = taxonomy_get_tree($vid);

  if(empty($term_list)) {
    echo json_encode(array('hide' => TRUE, 'html' => t('No terms available.')));
  } else {
    $options = array();
    foreach($term_list as $term) {
      $options[$term->tid] = str_repeat('-', $term->depth) . $term->name;
    }

    $form['add_term_id'] = array('#name' => 'add_term_id', '#type' => 'select', '#options' => $options, '#multiple' => TRUE);
    echo json_encode(array('hide' => FALSE, 'html' => drupal_render($form)));
  }
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
      $term->parents = $tags_parents[$vid][$child];
      $tree[] = $term;
      if (!empty($tags_children[$vid][$child])) {
        $tree = array_merge($tree, tags_get_tree($vid_list, $child, $depth, $max_depth));
      }
    }
  }

  return $tree;
}