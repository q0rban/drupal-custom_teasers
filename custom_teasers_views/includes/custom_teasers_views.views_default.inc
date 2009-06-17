<?php
// $Id: custom_teasers_views.views_default.inc,v 1.1 2009/06/16 21:51:04 q0rban Exp $

/**
 * Implementation of hook_views_default_views().
 *
 * Loads all teaser_views entries and content type settings, then generates a
 * view for each one. We do this so that the simple views can be overridden by
 * views_ui and customized by advanced users.
 */
function custom_teasers_views_default_views() {
  $views = array();

  $teaser_views = custom_teasers_views_load_all();
  $items = array();
  foreach($teaser_views as $item) {
    if (!empty($item['name']) && !empty($item['fields'])) {
      $view = custom_teasers_views_build_view($item);
      $views[$view->name] = $view;
    }
  }

  return $views;
}