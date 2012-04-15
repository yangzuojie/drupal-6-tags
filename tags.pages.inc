<?php

/**
 * tags/dashboard
 */
function tags_dashboard_page() {

  global $user;

  $result = db_query("SELECT v.* FROM {vocabulary} v LEFT JOIN {vocabulary_user} vu ON v.vid = vu.vid WHERE vu.uid = %d", $user->uid);
  $vocabulary_list = array();
  while ($vocabulary = db_fetch_object($result)) {
    $vocabulary_list[] = $vocabulary;
  }

  $path = drupal_get_path('module', 'tags');

  drupal_add_css($path . '/css/style.css', 'header');

  drupal_add_js($path . '/js/tags.js');

  return theme('tags_dashboard_page', array('vocabulary_list' => $vocabulary_list));
}

/**
 * tags/vocabulary/%/ajax
 */
function tags_vocabulary_ajax($vid) {
  //@TODO
  echo theme('tags_vocabulary', array('term_list' => $term_list));
}