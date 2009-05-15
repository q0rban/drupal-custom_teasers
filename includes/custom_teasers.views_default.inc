<?php
// $Id$

/**
 * Implementation of hook_views_default_views().
 *
 * Loads all features entries and content type settings, then generates a
 * view for each one. We do this so that the simple views can be overridden by
 * views_ui and customized by advanced users.
 */
function custom_teasers_views_default_views() {
  $views = array();

  $features = custom_teasers_item_load_all();
  $items = array();
  foreach($features as $item) {
    if (!empty($item['name']) && !empty($item['fields'])) {
      $view = custom_teasers_build_view($item);
      $views[$view->name] = $view;
    }
  }

  _custom_teasers_nodereferences_view($views);

  return $views;
}

/**
 * Implementation of hook_views_default_views().
 */
function _custom_teasers_nodereferences_view(&$views) {
  $view = new view;
  $view->name = 'custom_teasers_nodereferences';
  $view->description = 'Displays a list of available nodes to reference to for Custom Teasers.';
  $view->tag = 'custom_teasers';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'title' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'strip_tags' => 0,
        'html' => 0,
      ),
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'created' => array(
      'order' => 'DESC',
      'granularity' => 'second',
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'status' => array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'type' => array(
      'operator' => 'in',
      'value' => variable_get('custom_teasers_nodereference_types', array('page')),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('items_per_page', variable_get('custom_teasers_nodereference_limit', 0));
  $views[$view->name] = $view;

  return $views;
}
