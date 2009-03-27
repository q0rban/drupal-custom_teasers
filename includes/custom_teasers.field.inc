<?php

/**
 * @file
 * Feature fields forms and settings
 */
 
 /**
 * Re-order the display of fields associated with this feature.
 */
function custom_teasers_fields_form(&$form_state, $feature = array()) {
  $form = array();

  if ($feature['default']) {
    drupal_set_message(t('This is a default feature, and cannot have fields attached to it without first <a href="@url">editing</a> and saving the feature.', array('@url' => url('admin/build/custom_teasers/edit/'. $feature['name']))), 'warning');
    return $form;
  }

  $form['feature_name'] = array(
    '#type' => 'value',
    '#value' => $feature['name'],
  );

  $fields = $feature['fields'];

  $form['fields']['#tree'] = TRUE;
  if (is_array($fields)) {
    foreach ($fields as $name => $field) {
      $form['fields'][$name] = _custom_teasers_fields_item_field($field);
    }
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Build the overview form fields for a single item.
 *
 * @see custom_teasers_overview_form()
 */
function _custom_teasers_fields_item_field($item) {

  $form['weight'] = array(
    '#type' => 'weight',
    '#default_value' => $item['weight'],
  );

  $form['title'] = array(
    '#type' => 'markup',
    '#value' => check_plain($item['title']),
  );

  $form['field_name'] = array(
    '#type' => 'markup',
    '#value' => check_plain($item['field_name']),
  );

  $path = drupal_get_path('module', 'custom_teasers') . '/images/';
  $links[] = array(
    'title' => theme('image', $path . 'text-editor.png', t('Edit')),
    'href' => 'admin/build/custom_teasers/fields/' . $item['feature_name'] .'/options/'. $item['field_name'],
    'html' => TRUE,
  );
  $links[] = array(
    'title' => theme('image', $path . 'edit-delete.png', t('Delete')),
    'href' => 'admin/build/custom_teasers/fields/' . $item['feature_name'] .'/delete/'. $item['field_name'],
    'html' => TRUE,
  );

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
function theme_custom_teasers_fields_form($form) {
  $rows = array();
  $output = '';

  foreach (element_children($form['fields']) as $key) {
    $row = array();

    $row[] = drupal_render($form['fields'][$key]['title']);
    $row[] = drupal_render($form['fields'][$key]['field_name']);

    // Add an identifying CSS class to our weight field, as it's the one
    // the tabledrag.js will be controlling. This can be anything we want it to
    // be, we'll just tell the tabledrag.js library what it should look for.
    $form['fields'][$key]['weight']['#attributes']['class'] = 'features-fields-weight';
    $row[] = drupal_render($form['fields'][$key]['weight']);

    $row[] = drupal_render($form['fields'][$key]['operations']);

    // Add the new row to our collection of rows, and give it the 'draggable'
    // class, indicating that it should be... well, draggable.
    $rows[] = array(
      'data' => $row,
      'class' => 'draggable',
    );
  }

  // If there were no records found, note the fact so users don't get confused
  // by a completely empty table.
  if (isset($form['feature_name']['#value'])) {
    if (count($rows) == 0) {
      $output = t('No fields have been added.  Add some fields by navigating <a href="@url">here</a>.', array('@url' => url('admin/build/custom_teasers/edit/'. $form['feature_name']['#value'] .'/fields/add')));
    }
    else {
      $output = t('You can add more fields by navigating <a href="@url">here</a>.', array('@url' => url('admin/build/custom_teasers/edit/'. $form['feature_name']['#value'] .'/fields/add')));

      // Render a list of header titles, and our array of rows, into a table. Even
      // we've already rendered all of our records, we always call drupal_render()
      // on the form itself after we're done, so hidden security fields and other
      // elements (like buttons) will appear properly at the bottom of the form.
      $header = array(t('Label'), t('Name'), t('Weight'), t('Operations'));
      $output .= theme('table', $header, $rows, array('id' => 'features-fields'));
      $output .= drupal_render($form);
    
      // Now that we've built our output, tell Drupal to add the tabledrag.js library.
      // We'll pass in the ID of the table, the behavior we want it to use, and the
      // class that appears on each 'weight' form element it should be controlling.
      drupal_add_tabledrag('features-fields', 'order', 'self', 'features-fields-weight');
    }
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
function custom_teasers_fields_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $fields = array();
  //$feature = custom_teasers_field_save($values['name']);

  foreach($values['fields'] as $field_name => $field) {
    $weight = $field['weight'];
    db_query("UPDATE {custom_teasers_field} SET weight = %d WHERE field_name = '%s' AND feature_name = '%s'",  $weight, $field_name, $values['feature_name']);
  }


  custom_teasers_invalidate_cache();
  //$form_state['redirect'] = 'admin/build/custom_teasers';
}

/**
 *
 */
function custom_teasers_fields_add_form(&$form_state, $feature = array()) {
  $options = _custom_teasers_field_add_options($feature);

  if (empty($options)) {
    $form['notice'] = array(
      '#type' => 'item',
      '#value' => t('All available fields have already been added to this feature.'),
    );

    return $form;
  }

  $form['fields'] = array('#tree' => TRUE);

  foreach ($options as $field_name => $title) {
    $form['fields'][$field_name] = array('#tree' => TRUE);

    $form['fields'][$field_name]['checked'] = array(
      '#type' => 'checkbox',
      '#title' => $title,
      '#default_value' => 0,
    );
    $form['fields'][$field_name]['field_name'] = array(
      '#type' => 'value',
      '#value' => $field_name,
    );
    $form['fields'][$field_name]['title'] = array(
      '#type' => 'value',
      '#value' => $title,
    );
  }

  $form['feature_name'] = array(
    '#type' => 'value',
    '#value' => $feature['name'],
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

/**
 * Submit handler for adding new item(s) to a view.
 */
function custom_teasers_fields_add_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $fields = array_filter($values['fields']);
  $flush = FALSE;

  if (is_array($fields)) {
    foreach($fields as $field) {
      if ($field['checked']) {
        if (!custom_teasers_field_insert($field, $values['feature_name'])) {
          $flush = TRUE;
          drupal_set_message(t('There was a problem adding the field %name.', array('%name' => $field_name)));
        }
      }
    }
  }
  else {
    drupal_set_message(t('You should click something next time!'));
  }

  if ($flush) {
    custom_teasers_invalidate_cache();
  }

  $form_state['redirect'] = 'admin/build/custom_teasers/edit/'. $values['feature_name'] .'/fields';
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
function custom_teasers_field_delete_confirm(&$form_state, $feature, $field_name) {
  $form['feature_name'] = array(
    '#type' => 'value',
    '#value' => $feature['name'],
  );

  $form['field_name'] = array(
    '#type' => 'value',
    '#value' => $field_name,
  );

  return confirm_form($form,
    t('Are you sure you want to delete the %field field from %feature?', array('%field' => $field_name, '%feature' => $feature['title'])),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/build/custom_teasers/edit/'. $feature['name'] .'/fields',
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

function custom_teasers_field_delete_confirm_submit($form, &$form_state) {
  $values = $form_state['values'];

  if ($values['confirm']) {
    custom_teasers_field_delete($values['field_name'], $values['feature_name']);
    drupal_set_message(t('Your item was deleted.'));
  }

  $form_state['redirect'] = 'admin/build/custom_teasers/edit/'. $values['feature_name'] .'/fields';
}


/**
 * 
 */
function custom_teasers_field_options_form(&$form_state, $feature, $field_name) {
  module_load_include('inc', 'custom_teasers', '/includes/custom_teasers');
  // views_add_css('views-admin');

  $form = array();

  $form['feature_name'] = array(
    '#type' => 'value',
    '#value' => $feature['name'],
  );
  $form['field_name'] = array(
    '#type' => 'value',
    '#value' => $field_name,
  );

  // Now we load the field options form.
  $form['options'] = array('#tree' => TRUE);

  $view = views_get_view($feature['module'] .'_'. $feature['name']);
  $type = 'field';
  $types = views_object_types();
  $item = $feature['fields'][$field_name];

  // Now lets load options into the item if they aren't there,
  // and then format the item like views expect.
  $default = custom_teasers_default_data();
  $default_options = $default['fields'][$field_name]['options'];
  if (empty($item['options']) && !empty($default_options)) {
    $item['options'] = $default_options;
  }
  $item = _custom_teasers_field_views_format($item);

  $handler = views_get_handler($item['table'], $item['field'], $type);
  $handler->init($view, $item);
  $view->set_display('block');
  if ($view->display_handler->defaultable_sections($types[$type]['plural'])) {
    $form_state['section'] = $types[$type]['plural'];
    $view->display_handler->add_override_button($form['options'], $form_state, $form_state['section']);
  }
  $handler->options_form($form['options'], $form_state);

  unset($form['options']['override']);

  // @todo get this working.
  _custom_teasers_field_options_js($form_state, $form);

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  return $form;
}

/**
 * Submit handler.
 */
function custom_teasers_field_options_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  if (db_query("UPDATE {custom_teasers_field} SET options = '%s' WHERE field_name = '%s' AND feature_name = '%s'", serialize($values['options']), $values['field_name'], $values['feature_name'])) {
    drupal_set_message(t('Your field options have been saved'));
    custom_teasers_invalidate_cache();
    $form_state['redirect'] = 'admin/build/custom_teasers/edit/'. $values['feature_name'] .'/fields';
  }
  else {
    drupal_set_message(t('There was an error saving your field options.'));
  }
}

/**
 * Insert new field
 */
function custom_teasers_field_insert($field, $feature_name) {
  $sql = "INSERT INTO {custom_teasers_field} (field_name, feature_name, title) VALUES ('%s', '%s', '%s')";
  return db_query($sql, $field['field_name'], $feature_name, $field['title']);
}

/**
 * Delete field
 */
function custom_teasers_field_delete($field_name, $feature_name) {
  $sql = "DELETE FROM {custom_teasers_field} WHERE field_name = '%s' AND feature_name = '%s'";
  db_query($sql, $field_name, $feature_name);

  custom_teasers_invalidate_cache();
}


/**
 *  Returns a list of available fields for a specific feature.
 */
function _custom_teasers_field_add_options($feature) {
  $options = _custom_teasers_view_fields_get();

  foreach ((array) $feature['fields'] as $field) {
    unset($options[$field['field_name']]);
  }

  return $options;
}


/**
 * Helper function to get the cck and node fields for the feature content type;
 *
 * @return
 *  an array of fields
 */
function _custom_teasers_view_fields_get() {
  $types = content_types();

  $fields = $types['feature']['fields'];
  foreach ($fields as $field){
    $fields[$field['field_name']] = $field['widget']['label'];
  }
  if ($types['feature']['has_title']) {
    $fields['title'] = $types['feature']['title_label'];
  }
  if ($types['feature']['has_body']) {
    $fields['body'] = $types['feature']['body_label'];
  }
  $fields['read_more'] = t('Read more link');

  return($fields);
}


/**
 *
 */
function _custom_teasers_field_options_js(&$form_state, &$form) {
  module_load_include('inc', 'views', '/includes/form');
  
  foreach(element_children($form['alter']) as $element) {
    if ($element['#process'] == 'views_process_dependency') {
      views_process_dependency($element, NULL, $form['alter'], $form_state);
    }
  }
}