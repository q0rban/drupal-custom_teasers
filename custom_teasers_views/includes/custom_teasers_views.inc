<?php
// $Id$

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
 * @param $feature
 *   An array of votes, each with the following structure:
 *   $feature['path']      (Required)
 *   $feature['title']     (Required)
 *   $feature['header']    (Optional)
 *   $feature['filter']    (Optional, defaults to 'all')
 *   $feature['style']     (Optional, defaults to 'teasers')
 *   $feature['sort']      (Optional, defaults to 'newest')
 *   $feature['argument']  (Optional)
 *   $feature['rss']       (Optional, defaults to TRUE)
 *   $feature['block']     (Optional, defaults to TRUE)
 *   $feature['module']    (Optional, defaults to 'custom_teasers')
 *
 *   For individual properties inside $simplview, accepted values include:
 *   $feature['filter']    'all', or 'node:$nodetype'
 *   $feature['style']     'teasers', 'full', 'titles', or 'gallery'
 *   $feature['sort']      'newest', 'oldest', 'a-z', 'rating', or 'hits'
 *   $feature['argument']  'author', 'date', 'term:$vid', or 'field:$fieldname'

 * @return
 *   A newly constructed View object. To be properly handled by the Views module,
 *   the $view->name property must be set.
 */
function _custom_teasers_views_build_view($feature = array()) {
  $feature += custom_teasers_default_data();
  
  // Now we'll set up the basic starting structure.
  $view = new view;
  $view->name = $feature['module'] .'_'. $feature['name'];
  $view->description = "Generated automatically by the {$feature['module']} module.";
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;

  // Bootstrap the default display for the view. Here's where the action happens.
  $handler = $view->new_display('default', 'Defaults', 'default');

  _custom_teasers_views_add_fields($handler, $feature);
  _custom_teasers_views_add_sort($handler, $feature);
  _custom_teasers_views_add_filter($handler, $feature);

  _custom_teasers_views_add_extras($view, $feature);

  drupal_alter('feature', $view, $feature);

  // We will NOT save the view. Instead we're going to return it so others can
  // expose it as a default, save it to the DB, or whatever they'd like to do.
  return $view;
}

function _custom_teasers_views_add_fields(&$handler, $feature) {
  $fields = array(
    'path' => array(
      'id' => 'path',
      'table' => 'custom_teasers_node',
      'field' => 'path',
      'exclude' => 1,
    ),
  );
  $formatted = _custom_teasers_fields_views_format($feature['fields']);
  if (!empty($formatted)) {
    $fields += $formatted;
  }
  $handler->override_option('fields', $fields);
}

function _custom_teasers_views_add_filter(&$handler, $feature) {
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
    'view_name' => array(
      'operator' => 'in',
      'value' => array(
        $feature['name'] => $feature['name'],
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'view_name',
      'table' => 'custom_teasers_node',
      'field' => 'view_name',
      'relationship' => 'none',
    ),
  );

  // Currently, the only supported filters are per-node-type.
  // 'all' is actually just a passthrough option that doesn't apply
  // filtering beyond the standard 'published' check.
  $filter = $feature['filter'];
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

function _custom_teasers_views_add_sort(&$handler, $feature) {
  $sorts = array();

  if ($feature['sort'] == 'random') {
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
      'table' => 'custom_teasers_node',
      'field' => 'weight',
      'relationship' => 'none',
    );
    $sorts['created'] = array(
      'order' => $feature['sort'],
      'granularity' => 'second',
      'id' => 'created',
      'table' => 'node',
      'field' => 'created',
      'relationship' => 'none',
    );
 }

  $handler->override_option('sorts', $sorts);
}


function _custom_teasers_views_add_extras(&$view, $feature) {
  // Generate a block.
  $handler = $view->new_display('block', 'Block', 'block');
  // $handler->override_option('use_more', 1); Take this out until we figure out if we want this or not.
  $handler->override_option('block_description', $feature['title']);
  $handler->override_option('block_caching', -1);
  $handler->override_option('items_per_page', $feature['items_per_page']);

  _custom_teasers_views_add_style($handler, $feature);

  // Here we generate an RSS feed at [path]/rss.xml
  /*
  if (!empty($feature['rss'])) {
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
    
    $path = $feature['path'];
    $path .= empty($feature['argument']) ? '/rss.xml' : '/%/rss.xml';
    $handler->override_option('path', $path);
    $handler->override_option('displays', array('page' => 'page'));
  }
  */
}


function _custom_teasers_views_add_style(&$handler, $feature) {
  if (!empty($feature['style_plugin'])) {
    $handler->override_option('style_plugin', $feature['style_plugin']);
    if (!empty($feature['style_options'])) {
      $handler->override_option('style_options', $feature['style_options']);
    }
  }
}


/**
 * Formats all fields how views expects.
 */
function _custom_teasers_fields_views_format($fields) {
  foreach ($fields as $name => $field) {
    $fields[$name] = _custom_teasers_field_views_format($field);
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
function _custom_teasers_field_views_format($field) {
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
        'table' => 'custom_teasers_node',
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
