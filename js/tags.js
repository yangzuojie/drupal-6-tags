$(function() {
  $('#tags-vocabulary-select').change(function() {
    if($(this).val() == 0) {
      $('#tags-term-div').html('');
    } else {
      $.get(Drupal.settings.basePath + 'tags/manage/vocabulary/' + $(this).val() + '/ajax', {}, function(data) {
        if(data.hide) {
          $('#add-term-submit').hide();
        } else {
          $('#add-term-submit').show();
        }
        $('#tags-term-div').html(data.html);
      }, 'json');
    }
  });
});