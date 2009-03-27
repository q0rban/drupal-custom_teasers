<?php

/**
 * @file
 * Administration forms and settings.
 */
 
 /**
 * Feature settings form.
 */
function custom_teasers_settings() {
  $form = array();
  $node_types = array();

  foreach(node_get_types() as $type) {
    if ($type->module != 'feature') {
      $node_types[$type->type] = $type->name;
    }
  }

  $form['feature'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature Settings'),
  );
  $form['feature']['custom_teasers_nodereference_limit'] = array(
    '#type' => 'select',
    '#title' => t('Reference limit'),
    '#default_value' => variable_get('custom_teasers_nodereference_limit', 0),
    '#options' => array(t('All')) + range(0, 50),
    '#description' => t('How many nodes should the select menu hold on the Feature node form?'),
  );

  $form['feature']['custom_teasers_nodereference_types'] = array(
    '#type' => 'checkboxes',
    '#default_value' => variable_get('custom_teasers_nodereference_types', array('page', 'blog', 'gallery', 'faq')),
    '#title' => t('Feature content types'),
    '#options' => $node_types,
    '#description' => t('Only nodes from the checked boxes above will be displayed on the Feature node form.'),
  );

  if (!variable_get('custom_teasers_imagefield_installed', FALSE)) {
    $form['imagefield'] = array(
      '#type' => 'fieldset',
      '#title' => t('Install imagefield'),
      '#description' => t('Click here to import the imagefield field settings into the feature content type.')
    );
    $form['imagefield']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Import'),
      '#submit' => array('custom_teasers_imagefield_import_submit'),
    );
  }

  $form = system_settings_form($form);
  $form['#submit'] = array('custom_teasers_settings_submit');

  return $form;
}

/**
 * Submit handler for teaserific_settings form.
 */
function custom_teasers_settings_submit($form, &$form_state) {
  $values = $form_state['values'];

  if ($values['op'] == t('Reset to defaults')) {
    variable_del('custom_teasers_nodereference_limit');
    variable_del('custom_teasers_nodereference_types');
    drupal_set_message(t('The settings have been reset to their default options.'));
  }
  else if ($values['op'] != t('Import')) {
    variable_set('custom_teasers_nodereference_limit', $values['custom_teasers_nodereference_limit']);
    variable_set('custom_teasers_nodereference_types', array_keys(array_filter($values['custom_teasers_nodereference_types'])));
    drupal_set_message(t('The configuration options have been saved.'));
  }
}

/**
 * Submit handler.
 */
function custom_teasers_imagefield_import_submit($form, &$form_state) {
  $values = $form_state['values'];

  if ($values['op'] == t('Import')) {
    require_once(drupal_get_path('module', 'custom_teasers') .'/custom_teasers.install');
    _custom_teasers_imagefield_import();
  }
}


/**
 * Helper function to import imagefield settings into the feature node type.
 */
function _custom_teasers_imagefield_import() {
  if (module_exists('imagefield') && !variable_get('custom_teasers_imagefield_installed', FALSE)) {  
    $filename = drupal_get_path('module', 'custom_teasers') . '/includes/custom_teasers.imagefield.cck';

    // If we can't find the file, display an error and return.
    if (!is_file($filename)) {
      drupal_set_message(t('The Features field import file can\'t be found at !loc.', array('!loc' => $filename)), 'warning');
      return;
    }

    $fields = implode('', file ($filename));
    module_invoke('content', 'clear_type_cache');
  
    // Build form state
    $form_state = array(
      'values' => array(
        'type_name' => 'feature',
        'macro' => $fields,
      ),
    );
  
    // Inject our form into the CCK Import form.
    drupal_execute("content_copy_import_form", $form_state);
    variable_set('custom_teasers_imagefield_installed', TRUE);
  }
  else {
    drupal_set_message(t('There was an error importing the imagefield field.  Make sure imagefield.module is installed and then visit <a href="@url">the settings page</a> to try again.', array('@url' => url('admin/settings/custom_teasers'))));
    variable_set('custom_teasers_imagefield_installed', FALSE);
  }
}