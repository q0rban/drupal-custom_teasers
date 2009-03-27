<?php
// $Id$
/**
 * Field handler to present a link to the referenced node.
 */
class custom_teasers_handler_field_node_link extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['reference_path'] = 'reference_path';
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['text'] = array('default' => t('Read more'), 'translatable' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Link text'),
      '#default_value' => $this->options['text'],
    );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $text = !empty($this->options['text']) ? $this->options['text'] : t('Read more');
    $path = $values->{$this->aliases['reference_path']};

    return theme('custom_teasers_read_more_link', $text, $path);
  }
}
