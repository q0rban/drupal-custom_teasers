<?php

/**
 * @file
 * Administration forms and settings.
 */
 
 /**
 * Teaser settings form.
 */
function custom_teasers_settings() {
  $form = array();
  $node_types = array();

  foreach(node_get_types() as $type) {
    if ($type->type != 'teaser') {
      $node_types[$type->type] = $type->name;
    }
  }

  $form['teasers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Teaser Settings'),
  );
  $form['teasers']['custom_teasers_nodereference_limit'] = array(
    '#type' => 'select',
    '#title' => t('Reference limit'),
    '#default_value' => variable_get('custom_teasers_nodereference_limit', 0),
    '#options' => array(t('All')) + range(0, 50),
    '#description' => t('How many nodes should the select menu hold on the teaser node form?'),
  );

  $form['teasers']['custom_teasers_nodereference_types'] = array(
    '#type' => 'checkboxes',
    '#default_value' => variable_get('custom_teasers_nodereference_types', array('page')),
    '#title' => t('Teasable content types'),
    '#options' => $node_types,
    '#description' => t('Only nodes from the checked boxes above will be displayed on the teaser node form.'),
  );

  $form = system_settings_form($form);

  $form['#submit'][] = 'custom_teasers_settings_submit';

  return $form;
}

/**
 * Submit handler for the * settings form.
 */
function custom_teasers_settings_submit($form, &$form_state) {
  if (function_exists('custom_teasers_views_invalidate_cache')) {
    custom_teasers_views_invalidate_cache();
  }
}