// $Id$

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

function fieldLimit(feature, featureName) {
  var inputId = 'input#edit-feature-name-' + featureName;
  $.each(feature, function(fieldName, settings) {
    var textId = 'edit-' + fieldName;
    var infoDiv = 'features-' + textId + '-wrapper';

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
    var featureId = 'features-' + wrapperId;

    $('#' + wrapperId).append('<div class="features-limitchars" id="' + featureId + '"></div>');
  });

  // Pull in our features object from the drupal settings.
  var custom_teasers = Drupal.settings.custom_teasers;

  // If there's already one checked, run our fieldLimit fnc on it.
  $('input[id^=edit-feature-name-]').change(function() {
    var featureName = $(this).val();
    fieldLimit(custom_teasers[featureName], featureName);
  });

});

