<?php
// $Id$

/**
 * Overview page for sorting the feature nodes
 */
function custom_teasers_sort_overview() {
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
function custom_teasers_sort_form(&$form_state, $view_name, $sort) {
  $form = array();

  $form['nodes']['#tree'] = TRUE;

  $result = db_query("SELECT f.weight, n.nid, n.title FROM {custom_teasers} f INNER JOIN {node} n ON f.nid = n.nid WHERE f.view_name = '%s' ORDER BY f.weight ASC, n.created ". $sort, $view_name);

  while($node = db_fetch_object($result)) {
    $form['nodes'][$node->nid] = _custom_teasers_item($node);
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Build the elements for a single node.
 */
function _custom_teasers_item($node) {

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
function theme_custom_teasers_sort_form($form) {
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
function custom_teasers_sort_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $fields = array();
  //$feature = custom_teasers_field_save($values['name']);

  foreach ($values['nodes'] as $node) {
    feature_update_weight($node['nid'], $node['weight']);
  }
}