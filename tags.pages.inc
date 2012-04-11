<?php

/**
 *
 */
function tags_dashboard_page() {
  $path = drupal_get_path('module', 'tags');

  drupal_add_css($path . '/easyui/themes/icon.css');
  drupal_add_css($path . '/easyui/themes/default/easyui.css');
  drupal_add_css($path . '/css/style.css');

  drupal_add_js($path . '/easyui/jquery.easyui.min.js');

  global $user;

  $result = db_query("SELECT v.* FROM {vocabulary} v LEFT JOIN {vocabulary_user} vu ON v.vid = vu.vid WHERE vu.uid = %d", $user->uid);
  $data_list = array();
  while ($data = db_fetch_object($result)) {
    $children = taxonomy_get_tree($data->vid);
    if($children) {
      $children = convert_tree_data($children);
    }

    $data_list[] = array(
        'id' => 'V' . $data->vid,
        'text' => $data->name,
        'children' => $children,
      );
  }

  return theme('tags_dashboard_page', array('data_list' => array(
    'id' => 0,
    'text' => 'ROOT',
    'children' => $data_list)));
}

/**
 *
 */
function tags_save() {
  $id = $_POST['id'];
  $name = $_POST['text'];
  $parent = $_POST['parent'];

  if($name) {
    if($parent === '0') {
      $vocabulary = array(
        'name' => $name,
      );

      $vid = substr($id, 1);
      if(is_numeric($vid)) {
        $vocabulary['vid'] = $vid;
      }

      taxonomy_save_vocabulary($vocabulary);

      echo json_encode(array('id' => 'V' . $vocabulary['vid']));

    } else {
      $term = array();
      if (is_numeric($parent)) { // Is Term
        $term = array(
          'vid' => taxonomy_get_term($parent)->vid, // Voacabulary ID
          'name' => $name, // Term Name
          'parent' => $parent, // (Optional) Term ID of a parent term
        );
      } else { // Is Voacabulary
        $term = array(
          'vid' => substr($parent, 1), // Voacabulary ID
          'name' => $name, // Term Name
        );
      }

      if(is_numeric($id)) {
        $term['tid'] = $id;
      }

      taxonomy_save_term($term);

      echo json_encode(array('id' => $term['tid'])); 
    }
  }
}

/**
 *
 */
function convert_tree_data($term_list) {
  $data_list = array();
  foreach($term_list as $term) {
    $data_list[] = array(
        'id' => $term->tid,
        'text' => $term->name,
      );
  }
  return $data_list;
}