<?php
// $Id: custom_teasers_views.inc,v 1.1 2009/06/16 21:51:04 q0rban Exp $

/**
 * @file
 * SimpleViews utility library.
 *
 * Provides helper functions to generate standard Views based on a small number
 * of parameters.
 */

/**
 * Build a view object.
 *
 * This function converts a simple set of parameters into a full View definition.
 *
 * @param $teaser_view
 *   An array of votes, each with the following structure:
 *   $teaser_view['path']      (Required)
 *   $teaser_view['title']     (Required)
 *   $teaser_view['header']    (Optional)
 *   $teaser_view['filter']    (Optional, defaults to 'all')
 *   $teaser_view['style']     (Optional, defaults to 'teasers')
 *   $teaser_view['sort']      (Optional, defaults to 'newest')
 *   $teaser_view['argument']  (Optional)
 *   $teaser_view['rss']       (Optional, defaults to TRUE)
 *   $teaser_view['block']     (Optional, defaults to TRUE)
 *   $teaser_view['module']    (Optional, defaults to 'custom_teasers')
 *
 *   For individual properties inside $simplview, accepted values include:
 *   $teaser_view['filter']    'all', or 'node:$nodetype'
 *   $teaser_view['style']     'teasers', 'full', 'titles', or 'gallery'
 *   $teaser_view['sort']      'newest', 'oldest', 'a-z', 'rating', or 'hits'
 *   $teaser_view['argument']  'author', 'date', 'term:$vid', or 'field:$fieldname'

 * @return
 *   A newly constructed View object. To be properly handled by the Views module,
 *   the $view->name property must be set.
 */
function _custom_teasers_views_build_view($teaser_view = array()) {
  $teaser_view += custom_teasers_views_default_data();
  
  // Now we'll set up the basic starting structure.
  $view = new view;
  $view->name = $teaser_view['module'] .'_'. $teaser_view['name'];
  $view->description = "Generated automatically by the {$teaser_view['module']} module.";
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;

  // Bootstrap the default display for the view. Here's where the action happens.
  $handler = $view->new_display('default', 'Defaults', 'default');

  _custom_teasers_views_add_fields($handler, $teaser_view);
  _custom_teasers_views_add_sort($handler, $teaser_view);
  _custom_teasers_views_add_filter($handler, $teaser_view);

  _custom_teasers_views_add_extras($view, $teaser_view);

  drupal_alter('teaser_view', $view, $teaser_view);

  // We will NOT save the view. Instead we're going to return it so others can
  // expose it as a default, save it to the DB, or whatever they'd like to do.
  return $view;
}

function _custom_teasers_views_add_fields(&$handler, $teaser_view) {
  $fields = array(
    'path' => array(
      'id' => 'path',
      'table' => 'custom_teasers',
      'field' => 'path',
      'exclude' => 1,
    ),
  );
  $formatted = _custom_teasers_fields_views_format($teaser_view['fields']);
  if (!empty($formatted)) {
    $fields += $formatted;
  }
  $handler->override_option('fields', $fields);
}

function _custom_teasers_views_add_filter(&$handler, $teaser_view) {
  // We always want to add the status filter.
  $filters = array(
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
        'teaser' => 'teaser',
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
    'view_name' => array(
      'operator' => 'in',
      'value' => array(
        $teaser_view['name'] => $teaser_view['name'],
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'view_name',
      'table' => 'custom_teasers',
      'field' => 'view_name',
      'relationship' => 'none',
    ),
  );

  // Currently, the only supported filters are per-node-type.
  // 'all' is actually just a passthrough option that doesn't apply
  // filtering beyond the standard 'published' check.
  $filter = $teaser_view['filter'];
  if (strstr($filter, 'node') !== FALSE && $type = end(explode(':', $filter))) {
    $filters['type'] = array(
      'operator' => 'in',
      'value' => array($type => $type),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array('operator' => FALSE, 'label' => ''),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'relationship' => 'none',
    );
  }
  
  $handler->override_option('filters', $filters);
}

function _custom_teasers_views_add_sort(&$handler, $teaser_view) {
  $sorts = array();

  if ($teaser_view['sort'] == 'random') {
    $sorts['random'] = array(
      'id' => 'random',
      'table' => 'views',
      'field' => 'random',
    );
  }
  else {
    $sorts['weight'] = array(
      'order' => 'ASC',
      'id' => 'weight',
      'table' => 'custom_teasers',
      'field' => 'weight',
      'relationship' => 'none',
    );
    $sorts['created'] = array(
      'order' => $teaser_view['sort'],
      'granularity' => 'second',
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
      'relationship' => 'none',
    );
 }

  $handler->override_option('sorts', $sorts);
}


function _custom_teasers_views_add_extras(&$view, $teaser_view) {
  // Generate a block.
  $handler = $view->new_display('block', 'Block', 'block');
  // $handler->override_option('use_more', 1); Take this out until we figure out if we want this or not.
  $handler->override_option('block_description', $teaser_view['title']);
  $handler->override_option('block_caching', -1);
  $handler->override_option('items_per_page', $teaser_view['items_per_page']);

  _custom_teasers_views_add_style($handler, $teaser_view);

  // Here we generate an RSS feed at [path]/rss.xml
  /*
  if (!empty($teaser_view['rss'])) {
    $handler = $view->new_display('feed', 'Feed', 'feed');
    $handler->override_option('row_plugin', 'node_rss');
    $handler->override_option('row_options', array(
      'item_length' => 'default',
    ));
    $handler->override_option('style_plugin', 'rss');
    $handler->override_option('style_options', array(
      'mission_description' => FALSE,
      'description' => '',
    ));
    
    $path = $teaser_view['path'];
    $path .= empty($teaser_view['argument']) ? '/rss.xml' : '/%/rss.xml';
    $handler->override_option('path', $path);
    $handler->override_option('displays', array('page' => 'page'));
  }
  */
}


function _custom_teasers_views_add_style(&$handler, $teaser_view) {
  if (!empty($teaser_view['style_plugin'])) {
    $handler->override_option('style_plugin', $teaser_view['style_plugin']);
    if (!empty($teaser_view['style_options'])) {
      $handler->override_option('style_options', $teaser_view['style_options']);
    }
  }
}


/**
 * Formats all fields how views expects.
 */
function _custom_teasers_fields_views_format($fields) {
  foreach ($fields as $name => $field) {
    $fields[$name] = _custom_teasers_views_field_views_format($field);
    if (!is_array($fields[$name])) {
      unset($fields[$name]);
    }
  }
  return $fields;
}

/**
 * Handles one field and formats it how views expects. We separate this out
 * From the above so we can use it on individual fields in custom_teasers_views.field.inc
 */
function _custom_teasers_views_field_views_format($field) {
  $name = $field['field_name'];
  $field = (array) $field['options'];
  $base_info = _custom_teasers_field_views_info($name);

  if (is_array($base_info)) {
    return $field + $base_info;
  }
}

/**
 * Helper function to build the default structure for each view fields.
 */
function _custom_teasers_field_views_info($field_name) {
  $default = array(
    'label' => '',
    'relationship' => 'none',
    'link_to_node' => 0,
    'exclude' => 0,
  );

  switch($field_name) {
    case 'title':
      return array(
        'id' => 'title',
        'table' => 'node',
        'field' => 'title',
      ) + $default;

    case 'body':
      return array(
        'id' => 'body',
        'table' => 'node_revisions',
        'field' => 'body',
      ) + $default;

    case 'read_more':
      return array(
        'id' => 'read_more',
        'table' => 'custom_teasers',
        'field' => 'read_more',
      ) + $default;

    default:
      $field = _custom_teasers_field_cck_views_info($field_name);
      if (is_array($field)) {
        return $field + $default;
      }
  }
}

/**
 * Helper function to build the default structure for cck fields.
 */
function _custom_teasers_field_cck_views_info($field_name) {
  module_load_include('inc', 'content', '/includes/views/content.views');

  $field = content_fields($field_name);

  $result = module_invoke($module, 'field_settings', 'views data', $field);
  if (empty($result)) {
    $result = content_views_field_views_data($field);
  }
  if (is_array($result)) {
    foreach($result as $table_name => $item) {
      foreach($item as $key => $data) {
        if (isset($data['field'])) {
          return array(
            'id' => $key,
            'table' => $table_name,
            'field' => $key,
            'multiple' => array(
              'group' => TRUE,
              'multiple_number' => '',
              'multiple_from' => '',
              'multiple_reversed' => FALSE,
            ),
          );
        }
      }
    }
  }
}
