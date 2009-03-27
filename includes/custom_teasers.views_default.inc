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

  return $views;
}

/**
 * Implementation of hook_views_default_views().
 */
function _custom_teasers_views_default_views() {
  $view = new view;
  $view->name = 'primary';
  $view->description = 'Generated automatically by the custom_teasers module.';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'nid' => array(
      'id' => 'nid',
      'table' => 'feature',
      'field' => 'nid',
      'exclude' => 1,
    ),
    'title' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 1,
        'path' => '[path]',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 1,
        'max_length' => '20',
        'word_boundary' => 0,
        'ellipsis' => 1,
        'html' => 0,
      ),
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'body' => array(
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
        'trim' => 1,
        'max_length' => '300',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 1,
      ),
      'exclude' => 0,
      'id' => 'body',
      'table' => 'node_revisions',
      'field' => 'body',
      'relationship' => 'none',
    ),
    'field_feature_image_fid' => array(
      'label' => '',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 1,
        'path' => '[path]',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
      ),
      'link_to_node' => 0,
      'label_type' => 'none',
      'format' => 'feature_primary_default',
      'multiple' => array(
        'group' => TRUE,
        'multiple_number' => '',
        'multiple_from' => '',
        'multiple_reversed' => FALSE,
      ),
      'exclude' => 0,
      'id' => 'field_feature_image_fid',
      'table' => 'node_data_field_feature_image',
      'field' => 'field_feature_image_fid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'weight' => array(
      'order' => 'ASC',
      'id' => 'weight',
      'table' => 'feature',
      'field' => 'weight',
      'relationship' => 'none',
    ),
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
      'value' => array(
        'feature' => 'feature',
      ),
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
    'feature_name' => array(
      'operator' => 'in',
      'value' => array(
        'primary' => 'primary',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'feature_name',
      'table' => 'feature',
      'field' => 'feature_name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->override_option('items_per_page', 5);
  $handler->override_option('style_plugin', 'views_rotator');
  $handler->override_option('block_description', 'Homepage Primary feature');
  $handler->override_option('block_caching', -1);
  $views[$view->name] = $view;

  return $views;
}

// */
