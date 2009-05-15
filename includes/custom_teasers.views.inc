<?php
// $Id$
/**
 * @file
 * Provide views data and handlers for feature.module
 */

/**
 * Implementation of hook_views_data()
 */
function custom_teasers_views_data() {

  // We're registering the 'custom_teasers_node' table.
  $data['custom_teasers_node'] = array(
    'table' => array(
      'group' => t('Feature'),
      'join' => array(
        'node' => array(
          'left_field' => 'vid',
          'field' => 'vid',
        ),
      ),
    ),
    'view_name' => array(
      'title' => t('Feature'), // The item it appears as on the UI,
      'help' => t('The selected feature view.'), // The help that appears on the UI,
      'filter' => array(
         'handler' => 'custom_teasers_handler_filter_name',
      ),
    ),
    'path' => array(
      'real field' => 'reference_path',
      'title' => t('Featured path'),
      'help' => t('The path this feature links to.'),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'read_more' => array(
      'title' => t('Read more link'),
      'help' => t('A simple read more link.'),
      'field' => array(
        'handler' => 'custom_teasers_handler_field_node_link',
      ),
    ),
    'weight' => array(
      'title' => t('Weight'), // The item it appears as on the UI,
      'help' => t('The sort weight.'), // The help that appears on the UI,
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function custom_teasers_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'custom_teasers') .'/includes',
    ),
    'handlers' => array(
      // filters
      'custom_teasers_handler_filter_name' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
      'custom_teasers_handler_field_node_link' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}