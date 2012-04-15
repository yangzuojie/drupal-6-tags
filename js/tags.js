$(function() {
  $('#tags-vocabulary-select').change(function() {
    if($(this).val() == 0) {
      $('#tags-term-div').html('');
    } else {
      $.get(Drupal.settings.basePath + 'tags/manage/vocabulary/' + $(this).val() + '/ajax', {}, function(html) {
        $('#tags-term-div').html(html);
      });
    }
  });
});