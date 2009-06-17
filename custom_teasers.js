// $Id: custom_teasers.js,v 1.1 2009/06/16 21:51:04 q0rban Exp $

$(function() {

  $('#edit-reference-nid-wrapper, #edit-reference-path-wrapper').hide();

  $('input[id^=edit-reference-type-]').change(function() {
    var id = $(this).attr('id');
    if (id == 'edit-reference-type-nid') {
      $('#edit-reference-nid-wrapper').show();
      $('#edit-reference-path-wrapper').hide();
    }
    else {
      $('#edit-reference-path-wrapper').show();
      $('#edit-reference-nid-wrapper').hide();
    }
  });

  $('input[id^=edit-reference-type-]:checked').change();

});

