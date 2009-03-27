<?php
// $Id: feature.module,v 1.24 2008/01/04 22:27:19 darrenoh Exp $

/**
 * @file
 * Defines a feature content type for use with the custom_teasers module.
 */

/**
 * Implementation of hook_node_info().
 */
function custom_teasers_node_info() {
  return array(
    'feature' => array(
      'name' => t('Feature'),
      'module' => 'feature',
      'description' => t("Feature content on your homepage."),
      'has_title' => TRUE,
      'title_label' => t('Feature Title'),
      'has_body' => TRUE,
      'body_label' => t('Feature Body'),
    )
  );
}

/**
 * Implementation of hook_access().
 *
 * Node modules may implement node_access() to determine the operations
 * users may perform on nodes. This example uses a very common access pattern.
 */
function feature_access($op, $node, $account) {
  if ($op == 'create') {
    return user_access('create feature content', $account);
  }

  if ($op == 'update') {
    if (user_access('edit any feature content', $account) || (user_access('edit own feature content', $account) && ($account->uid == $node->uid))) {
      return TRUE;
    }
  }

  if ($op == 'delete') {
    if (user_access('delete any feature content', $account) || (user_access('delete own feature content', $account) && ($account->uid == $node->uid))) {
      return TRUE;
    }
  }
}

/**
 * Implementation of hook_form().
 *
 * Now it's time to describe the form for collecting the information
 * specific to this node type. This hook requires us to return an array with
 * a sub array containing information for each element in the form.
 */
function feature_form(&$node) {
  // Add js to control the jquery.
  _feature_node_form_js();

  $options = custom_teasers_item_load_all_list();

  // Now we load the title and body information for this node type.
  $type = node_get_types('type', $node);

  if ($type->has_title) {
    $form['title'] = array(
      '#type' => 'textfield',
      '#title' => check_plain($type->title_label),
      '#required' => TRUE,
      '#default_value' => $node->title,
      '#weight' => -5
    );
  }

  if ($type->has_body) {
    $form['body_field'] = node_body_field($node, $type->body_label, $type->min_word_count);
  }

  // Now we list the possible nodes to reference (IOW, link to).
  $references = _feature_potential_references();
  if (isset($node->reference_nid) && !isset($references['titles'][$node->reference_nid])) {
    $reference = node_load($node->reference_nid);
    $references['titles'][$node->reference_nid] = $reference->title;
  }

  $form['feature_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature options'),
  );

  // We want to display the feature name selector first.
  $form['feature_options']['feature_name'] = array(
    '#title' => t('Feature location'),
    '#type' => 'radios',
    '#default_value' => $node->feature_name,
    '#options' => $options,
    '#required' => TRUE,
    '#weight' => -6,
    '#prefix' => '<div id="feature-name-wrap">',
    '#suffix' => '</div>',
  );

  $form['feature_options']['reference_type'] = array(
    '#type' => 'radios',
    '#default_value' => isset($node->reference_type) ? $node->reference_type : 'nid',
    '#required' => TRUE,
    '#title' => t('Destination'),
    '#options' => array(
      'nid' => t('Content'),
      'rel' => t('Path'),
    ),
    '#prefix' => theme('advanced_help_topic', 'custom_teasers', 'reference-type'),
  );

  $form['feature_options']['reference_path'] = array(
    '#type' => 'textfield',
    '#default_value' => $node->reference_path,
    '#title' => t('Path/URL'),
    '#description' => t('Specify your path here.  Do not include a leading slash (ex. about-us instead of /about-us).'),
  );

  $form['feature_options']['reference_nid'] = array(
    '#type' => 'select',
    '#title' => t('Content to link to'),
    '#default_value' => isset($node->reference_nid) ? $node->reference_nid : 0,
    '#options' => (array) $references['titles'],
    '#description' => t('If the piece of content you would like to feature is not listed here, you should edit the content itself to feature it.'),
  );
  if (empty($references)) {
    $form['feature_options']['reference_nid']['#disabled'] = TRUE;
    $form['feature_options']['reference_nid']['#description'] = t('You do not have any content to feature.  Please create some content first to use the Existing content destination.');
  }

  // Check to see if we've added this from another node.
  if (is_numeric($nid = arg(3))) {
    list($form['feature_options']['reference_nid']['#type'], $form['reference_type']['#type']) = 'value';
    $form['feature_options']['reference_nid']['#value'] = $nid;
    $form['feature_options']['reference_type']['#value'] = 'nid';
  }

  return $form;
}

/**
 * Implementation of hook_validate().
 *
 * Errors should be signaled with form_set_error().
 */
function feature_validate(&$node) {
  if ($node->reference_type == 'nid' && empty($node->reference_nid)) {
    form_set_error('reference_nid', t('You need to select a piece of content to feature.'));
  }
  if ($node->reference_type == 'rel') {
    // Strip slashes from the beginning and end of the reference_path
    $node->reference_path = trim($node->reference_path, '\\/');
    // Check to make sure it's not empty.
    if (empty($node->reference_path)) {
      form_set_error('reference_path', t('You must specify a destination path.'));
    }
    // Check to make sure they didn't include a full path.
    if (strpos($node->reference_path, 'http') === 0) {
      form_set_error('reference_path', t('Please use a relative path. A "relative path" is a URL WITHOUT the domain name stuff in front of it (ex. about-us).'));
    }
  }
  if (!$node->feature_name) {
    form_set_error('feature_name', t('You must select a feature location.'));
  }
}

/**
 * Implementation of hook_insert().
 */
function feature_insert($node) {
  _feature_reference_format($node);

  db_query("INSERT INTO {custom_teasers_node} (vid, nid, feature_name, reference_nid, reference_type, reference_path) VALUES (%d, %d, '%s', %d, '%s', '%s')", $node->vid, $node->nid, $node->feature_name, $node->reference_nid, $node->reference_type, $node->reference_path);
  // this screws up image uploading.
  // drupal_goto('node/'. $node->reference_nid);
}

/**
 * Implementation of hook_update().
 */
function feature_update($node) {
  // if this is a new node or we're adding a new revision,
  if ($node->revision) {
    feature_insert($node);
  }
  else {
    _feature_reference_format($node);

    db_query("UPDATE {custom_teasers_node} SET feature_name = '%s', reference_nid = %d, reference_type = '%s', reference_path = '%s' WHERE vid = %d",  $node->feature_name, $node->reference_nid, $node->reference_type, $node->reference_path, $node->vid);
  }
  // this screws up image uploading.
  // drupal_goto('node/'. $node->reference_nid);
}

/**
 * Implementation of hook_delete().
 *
 * When a node is deleted, we need to remove all related records from out table.
 */
function feature_delete($node) {
  // Delete all revisions, by using the node's nid.
  db_query('DELETE FROM {custom_teasers_node} WHERE nid = %d', $node->nid);
}

/**
 * Implementation of hook_load().
 *
 * Now that we've defined how to manage the node data in the database, we
 * need to tell Drupal how to get the node back out. This hook is called
 * every time a node is loaded, and allows us to do some loading of our own.
 */
function feature_load($node) {
  $node = db_fetch_object(db_query('SELECT feature_name, reference_nid, reference_type, reference_path FROM {custom_teasers_node} WHERE vid = %d', $node->vid));
  return $node;
}

/**
 * Implementation of hook_view().
 *
 * This is a typical implementation that simply runs the node text through
 * the output filters.
 */
function feature_view($node, $teaser = FALSE, $page = FALSE) {
  $node = node_prepare($node, $teaser);

  return $node;
}

/**
 * Update a weight on a feature.
 */
function feature_update_weight($nid, $weight) {
  db_query("UPDATE {custom_teasers_node} SET weight = %d WHERE nid = %d", $weight, $nid);
}

/**
 * Helper function to format the node reference based on type.
 */
function _feature_reference_format(&$node) {  
  switch($node->reference_type) {
    case 'rel':
      $node->reference_nid = 0;
      break;

    case 'nid':
      $node->reference_path = 'node/'. $node->reference_nid;
      break;
  }
}

/**
 * Returns referenceable nodes defined by content types.
 * Adapted from nodereference.module
 */
function _feature_potential_references($limit = 0) {
  $types = array();
  $where = array();
  $args = array();
  $references = array();

  // If the limit arg is not set, use the variable.
  $limit = ($limit) ? $limit : variable_get('custom_teasers_nodereference_limit', 0);

  foreach (variable_get('custom_teasers_nodereference_types', array('page', 'blog', 'gallery', 'faq')) as $type) {
    $types[] = "n.type = '%s'";
    $args[] = $type;
  }

  $where[] = implode(' OR ', $types);

  // If there's no content types, return an empty array before the query.
  if (!count($types)) {
    return array();
  }

  $where_clause = $where ? 'WHERE ('. implode(') AND (', $where) .')' : '';
  $sql = db_rewrite_sql("SELECT n.nid, n.title AS node_title, n.type AS node_type, n.created AS node_created FROM {node} n $where_clause ORDER BY n.created DESC, n.title, n.type");
  $result = $limit ? db_query_range($sql, $args, 0, $limit) : db_query($sql, $args);

  // Load the array
  while ($node = db_fetch_object($result)) {
    $references['titles'][$node->nid] = $node->node_title;
    //$references['types'][$node->nid] = $node->node_type;
  }

  return $references;
}


/**
 * Add js to the feature node form.
 */
function _feature_node_form_js() {
  $features = custom_teasers_item_load_all();
  $settings = array();

  foreach ($features as $name => $feature) {
    $feature += custom_teasers_default_data();
    if (is_array($feature['fields'])) {
      foreach($feature['fields'] as $field_name => $field) {
        $settings[$name][$field_name]['field_name'] = $field_name;
        if (is_array($field) && $field['options']['alter']['trim']) {
          $settings[$name][$field_name]['length'] = (int) $field['options']['alter']['max_length'];
        }
      }
    }
  }

  if (!empty($settings)) {
    drupal_add_js(drupal_get_path('module', 'custom_teasers') .'/custom_teasers.js', 'module');
    drupal_add_js(array('custom_teasers' => $settings), 'setting');
  }
}