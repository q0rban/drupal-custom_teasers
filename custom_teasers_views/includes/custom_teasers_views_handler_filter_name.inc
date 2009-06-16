<?php
// $Id$

/**
 * Filter by teaser view name
 */
class custom_teasers_handler_filter_name extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_options = custom_teasers_views_load_all_list();
    }
  }
}