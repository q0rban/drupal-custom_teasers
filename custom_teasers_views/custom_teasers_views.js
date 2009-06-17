// $Id: custom_teasers_views.js,v 1.1 2009/06/16 21:51:04 q0rban Exp $

function limitChars(textid, limit, infodiv) {
  var text = $('#' + textid).val(); 
  var textlength = text.length;
  if (textlength > limit) {
    $('#' + infodiv).html('This field will be truncated to '+limit+' characters.');
    //$('#'+textid).val(text.substr(0,limit));
    return false;
  }
  else if (limit == parseInt(limit)) {
    $('#' + infodiv).html('You have '+ (limit - textlength) +' characters left.');
    return true;
  }
  else {
    $('#' + infodiv).html('');
    return true;
  }
}

function fieldLimit(view, viewName) {
  var inputId = 'input#edit-view-name-' + viewName;
  $.each(view, function(fieldName, settings) {
    var textId = 'edit-' + fieldName;
    var infoDiv = 'teaser-views-' + textId + '-wrapper';

    $('#' + textId).keyup(function() {
      if ($(inputId + ':checked').val()) {
        limitChars(textId, settings.length, infoDiv);
      }
    });

    $('#' + textId).keyup();
  });
}

$(function() {

  // Add our infoDiv to each input & textarea element
  $('.form-text, .form-textarea').each(function() {
    var wrapperId = $(this).attr('id') + '-wrapper';
    var viewId = 'teaser-views-' + wrapperId;

    $('#' + wrapperId).append('<div class="teaser-views-limitchars" id="' + viewId + '"></div>');
  });

  // Pull in our views object from the drupal settings.
  var custom_teasers_views = Drupal.settings.custom_teasers_views;

  // If there's already one checked, run our fieldLimit fnc on it.
  $('input[id^=edit-view-name-]').change(function() {
    var viewName = $(this).val();
    fieldLimit(custom_teasers_views[viewName], viewName);
  });

  $('input[id^=edit-view-name-]').change();
});

