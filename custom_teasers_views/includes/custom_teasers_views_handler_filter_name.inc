<?php
// $Id: custom_teasers_views_handler_filter_name.inc,v 1.2 2009/06/17 15:35:48 q0rban Exp $

/**
 * Filter by teaser view name
 */
class custom_teasers_views_handler_filter_name extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_options = custom_teasers_views_load_all_list();
    }
  }
}