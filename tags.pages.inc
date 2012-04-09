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

  return theme('tags_dashboard_page', array());
}