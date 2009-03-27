<?php
// $Id$

/**
 * Overview page for sorting the feature nodes
 */
function custom_teasers_node_sort_overview() {
  $features = array();

  $output = '<div class="features-node-sort">';
  $output .= t('Use this area to sort your featured content.  Please note, feature types that use random sorting are not listed here.  Please select from an available feature below:');
  $output .= '</div>';

  foreach (custom_teasers_item_load_all() as $feature) {
    if ($feature['sort'] != 'random') {
      $features[] = l($feature['title'], 'admin/content/node/custom_teasers/'. $feature['name']);
    }
  }

 $output .= theme('item_list', $features);

  return $output;
}

 /**
 * Re-order the display of feature nodes associated with this feature.
 */
function custom_teasers_node_sort_form(&$form_state, $feature_name, $sort) {
  $form = array();

  $form['nodes']['#tree'] = TRUE;

  $result = db_query("SELECT f.weight, n.nid, n.title FROM {custom_teasers_node} f INNER JOIN {node} n ON f.nid = n.nid WHERE f.feature_name = '%s' ORDER BY f.weight ASC, n.created ". $sort, $feature_name);

  while($node = db_fetch_object($result)) {
    $form['nodes'][$node->nid] = _custom_teasers_node_item($node);
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Build the elements for a single node.
 */
function _custom_teasers_node_item($node) {

  $form['weight'] = array(
    '#type' => 'weight',
    '#default_value' => $node->weight,
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form['title'] = array(
    '#type' => 'markup',
    '#value' => check_plain($node->title),
  );

  $path = drupal_get_path('module', 'custom_teasers') . '/images/';
  $links[] = array(
    'title' => theme('image', $path .'text-editor.png', t('Edit')),
    'href' => 'node/'. $node->nid .'/edit',
    'html' => TRUE,
  );
  $links[] = array(
    'title' => theme('image', $path .'edit-delete.png', t('Delete')),
    'href' => 'node/'. $node->nid .'/delete',
    'html' => TRUE,
  );

  $form['operations'] = array(
    '#type' => 'markup',
    '#value' => theme('links', $links),
  );

  return $form;
}

/**
 * Theme the node sorting form.
 *
 * Arranges items in a table.
 *
 * @ingroup themeable
 * @ingroup forms
 */
function theme_custom_teasers_node_sort_form($form) {
  $rows = array();
  $output = '';

  foreach (element_children($form['nodes']) as $key) {
    $row = array();

    $row[] = drupal_render($form['nodes'][$key]['nid']) . drupal_render($form['nodes'][$key]['title']);

    // Add an identifying CSS class to our weight field, as it's the one
    // the tabledrag.js will be controlling. This can be anything we want it to
    // be, we'll just tell the tabledrag.js library what it should look for.
    $form['nodes'][$key]['weight']['#attributes']['class'] = 'features-nodes-weight';
    $row[] = drupal_render($form['nodes'][$key]['weight']);

    $row[] = drupal_render($form['nodes'][$key]['operations']);

    // Add the new row to our collection of rows, and give it the 'draggable'
    // class, indicating that it should be... well, draggable.
    $rows[] = array(
      'data' => $row,
      'class' => 'draggable',
    );
  }

  // If there were no records found, note the fact so users don't get confused
  // by a completely empty table.
  if (count($rows) == 0) {
    $output = t('No features have been added to this feature view.');
  }
  else {
    // Render a list of header titles, and our array of rows, into a table. Even
    // we've already rendered all of our records, we always call drupal_render()
    // on the form itself after we're done, so hidden security fields and other
    // elements (like buttons) will appear properly at the bottom of the form.
    $header = array(t('Title'), t('Weight'), t('Operations'));
    $output .= theme('table', $header, $rows, array('id' => 'features-nodes'));
    $output .= drupal_render($form);
  
    // Now that we've built our output, tell Drupal to add the tabledrag.js library.
    // We'll pass in the ID of the table, the behavior we want it to use, and the
    // class that appears on each 'weight' form element it should be controlling.
    drupal_add_tabledrag('features-nodes', 'order', 'self', 'features-nodes-weight');
  }

  return $output;
}


/**
 * General submit handler for the add/edit form.
 *
 * Simply passes incoming form values on to the module's CRUD save function,
 * then redirects to the overview form.
 *
 * @ingroup formapi
 * @see custom_teasers_form()
 */
function custom_teasers_node_sort_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $fields = array();
  //$feature = custom_teasers_field_save($values['name']);

  foreach ($values['nodes'] as $node) {
    feature_update_weight($node['nid'], $node['weight']);
  }
}

/**
 * Build the overview form.
 *
 * Loads all items and builds an overview form with weight elements for each
 * item.
 *
 * @ingroup forms
 * @see _custom_teasers_overview_item_field()
 * @see custom_teasers_overview_form_submit()
 * @see theme_custom_teasers_overview_form()
 */
function custom_teasers_overview_form(&$form_state) {
  $items = custom_teasers_item_load_all();

  $form['items']['#tree'] = TRUE;
  foreach ($items as $name => $item) {
    $form['items'][$name] = _custom_teasers_overview_item_field($item);
  }

  return $form;
}


/**
 * Build the overview form fields for a single item.
 *
 * @see custom_teasers_overview_form()
 */
function _custom_teasers_overview_item_field($item) {
  $form['name'] = array(
    '#type' => 'hidden',
    '#value' => $item['name'],
  );

  $form['title'] = array(
    '#type' => 'markup',
    '#value' => check_plain($item['title']),
  );

  $form['path'] = array(
    '#type' => 'markup',
    '#value' => l($item['path'], $item['path']),
  );

  $path = drupal_get_path('module', 'custom_teasers') . '/images/';
  $links[] = array(
    'title' => theme('image', $path . 'text-editor.png', t('Edit')),
    'href' => 'admin/build/custom_teasers/edit/' . $item['name'],
    'html' => TRUE,
  );
  if (!$item['default']) {
    $links[] = array(
      'title' => theme('image', $path . 'edit-delete.png', t('Delete')),
      'href' => 'admin/build/custom_teasers/delete/' . $item['name'],
      'html' => TRUE,
    );
  }

  $form['operations'] = array(
    '#type' => 'markup',
    '#value' => theme('links', $links),
  );

  return $form;
}

/**
 * Theme the overview form.
 *
 * Arranges items in a table.
 *
 * @ingroup themeable
 * @ingroup forms
 * @see custom_teasers_overview_form()
 */
function theme_custom_teasers_overview_form($form) {
  $rows = array();
  foreach (element_children($form['items']) as $key) {
    $row = array();

    // Render the hidden 'item id' field and the title of the item into the
    // same column of the row.
    $row[] = drupal_render($form['items'][$key]['name']) . drupal_render($form['items'][$key]['title']);
    $row[] = drupal_render($form['items'][$key]['path']);
    $row[] = drupal_render($form['items'][$key]['operations']);
    $rows[] = $row;
  }

  // If there were no items found, note the fact so users don't get confused
  // by a completely empty table.
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No items have been added.'), 'colspan' => 3));
  }

  // Render a list of header titles, and our array of rows, into a table. Even
  // we've already rendered all of our items, we always call drupal_render()
  // on the form itself after we're done, so hidden security fields and other
  // elements (like buttons) will appear properly at the bottom of the form.
  $header = array(t('Label'), t('Path'), t('Operations'));
  $output = theme('table', $header, $rows);
  $output .= drupal_render($form);

  return $output;
}


/* Individual item editing form */

/**
 * Build the record editing form.
 *
 * If a record is passed in, an edit form with both Save and Delete buttons will
 * be built. Otherwise, a blank 'add new record' form, without the Delete button,
 * will be built.
 *
 * @ingroup forms
 * @see custom_teasers_form_submit()
 * @see custom_teasers_form_delete()
 */
function custom_teasers_form(&$form_state, $feature = array()) {
  module_load_include('inc', 'views', 'includes/admin');

  $form['feature'] = _custom_teasers_form($feature);

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  // Only show the delete button if we already have an ID. Set the delete
  // button's submit handler to a custom function that should only fire if
  // this button is clicked. In all other cases, the form will fall back to
  // the default $form_id_submit() function.
  if (!empty($feature['name']) && !$feature['default']) {
    $form['buttons']['submit']['#value'] = t('Update');
    $form['buttons']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('custom_teasers_form_delete'),
    );
  }
  if (!empty($feature['name']) || $feature['default']) {
    $form['feature']['name']['#type'] = 'value';
    $form['feature']['name']['#value'] = $feature['name'];
  }
  if ($feature['default']) {
    $form['feature']['default'] = array(
      '#type' => 'value',
      '#value' => TRUE,
    );
    $form['#fields'] = $feature['fields'];
  }
  
  return $form;
}

/**
 * Build the record editing chunk, in a reusable fashion.
 */
function _custom_teasers_form($feature = array()) {
  $feature += custom_teasers_default_data();

  // We theme this explicitly, rather than overriding the entire form,
  // so that modules re-use this chunk of form to on their own pages and still
  // get the proper formatting.
  $form['#theme'] = 'custom_teasers_edit_form';

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The name must contain only lowercase letters, numbers, and underscores.'),
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
    '#default_value' => $feature['title'],
  );

  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#required' => TRUE,
    '#default_value' => $feature['path'],
  );

  $form['style_plugin'] = array(
    '#type' => 'select',
    '#title' => t('Style'),
    '#options' => views_fetch_plugin_names('style', 'normal'),
    '#required' => TRUE,
    '#default_value' => $feature['style_plugin'],
  );

  $form['sort'] = array(
    '#type' => 'select',
    '#title' => t('Sorted'),
    '#options' => array(
      'DESC' => t('Newest first'),
      'ASC' => t('Oldest first'),
      'random' => t('Random'),
    ),
    '#required' => TRUE,
    '#default_value' => $feature['sort'],
  );

  $form['items_per_page'] = array(
    '#type' => 'select',
    '#title' => t('Number of items to display'),
    '#options' => range(0, 15),
    '#required' => TRUE,
    '#default_value' => $feature['items_per_page'],
  );

  drupal_alter('custom_teasers_reusable_form', $form, $feature);
  
  return $form;
}

/**
 * Implementation of hook_form_validate().
 *
 * Errors should be signaled with form_set_error().
 */
function custom_teasers_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $features = custom_teasers_item_load_all();

  if ($values['op'] == t('Submit') && !$values['default'] && isset($features[$values['name']])) {
    form_set_error('name', t('The feature name %name is already taken.', array('%name' => $values['name'])));
  }
  if (!preg_match('!^[a-z0-9_]+$!', $values['name'])) {
    form_set_error('type', t('The name must contain only lowercase letters, numbers, and underscores.'));
  }
}

/**
 * General submit handler for the add/edit form.
 *
 * Simply passes incoming form values on to the module's CRUD save function,
 * then redirects to the overview form.
 *
 * @ingroup formapi
 * @see custom_teasers_form()
 */
function custom_teasers_form_submit($form, &$form_state) {
  $item = $form_state['values'];
  $item['fields'] = empty($form['#fields']) ? NULL : $form['#fields']; 
  $op = (($item['op'] == t('Submit')) || $item['default']) ? 'insert' : 'update';

  custom_teasers_item_save($item, $op);
  custom_teasers_invalidate_cache();

  if ($op == 'update') {
    $form_state['redirect'] = 'admin/build/custom_teasers';
  }
  else if ($item['style_plugin'] == 'default' && empty($item['fields'])) {
    $form_state['redirect'] = "admin/build/custom_teasers/edit/{$item['name']}/fields/add";
  }
  else {
    $form_state['redirect'] = "admin/build/custom_teasers/edit/{$item['name']}/style_options";
  }
}


/**
 * Delete button submit handler for the add/edit form.
 *
 * Redirects to the 'delete item' confirmation page without performing any
 * operations.
 *
 * @ingroup formapi
 * @see custom_teasers_form()
 */
function custom_teasers_form_delete($form, &$form_state) {
  $form_state['redirect'] = 'admin/build/custom_teasers/' . $form_state['values']['name'] . '/delete';
}


/**
 * Build the delete confirmation form.
 *
 * A simple wrapper around Drupal's core confirm_form() function. Adds a value
 * field to store the ID of the item being deleted.
 *
 * @ingroup forms
 * @see custom_teasers_delete_confirm_submit()
 * @see confirm_form()
 */
function custom_teasers_delete_confirm(&$form_state, $item) {
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $item['name'],
  );

  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $item['title'])),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/build/custom_teasers',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * General submit handler for the delete confirmation form.
 *
 * Core's confirm_form() function adds the 'confirm' value element we check
 * against to ensure the form was properly submitted. If it's there, delete
 * the item and redirect to the overview form.
 *
 * @ingroup formapi
 * @see custom_teasers_form()
 */

function custom_teasers_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    custom_teasers_item_delete($form_state['values']['name']);
    custom_teasers_fields_delete($form_state['values']['name']);
    drupal_set_message(t('Your item was deleted.'));
  }
  $form_state['redirect'] = 'admin/build/custom_teasers';
}

/**
 * 
 */
function custom_teasers_style_options_form(&$form_state, $feature = array()) {
  $form = $options_form = array();

  if ($feature['default']) {
    $message = t('This is a default feature.  Styles are not editable until the feature has been <a href="@url">edited</a> and saved.', array('@url' => url('admin/build/custom_teasers/edit/'. $feature['name'])));
    drupal_set_message($message, 'warning');
  }
  else {
    $plugin = views_get_plugin('style', $feature['style_plugin']);
    $plugin->options_form($options_form, $form_state);

    $form['name'] = array(
      '#type' => 'value',
      '#value' => $feature['name'],
    );

    $form['style_options'] = $options_form;
    $form['style_options']['#type'] = 'fieldset';
    $form['style_options']['#tree'] = TRUE;
    $form['style_options']['#title'] = t('Style options');
  
    foreach($options_form as $key => $element) {
      if (isset($element['#default_value']) && isset($feature['style_options'][$key])) {
        $form['style_options'][$key]['#default_value'] = $feature['style_options'][$key];
      }
    }
  
    $form['style_options']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
  }

  return $form;
}

/**
 * 
 */
function custom_teasers_style_options_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  db_query("UPDATE {custom_teasers_view} SET style_options = '%s' WHERE name = '%s'", serialize($values['style_options']), $values['name']);

  views_invalidate_cache();
  views_flush_caches();

  $form_state['redirect'] = 'admin/build/custom_teasers/edit/'. $values['name'];
}