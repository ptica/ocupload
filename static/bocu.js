BUE.postprocess.bocu = function(E, $) {
  E.showFileSelectionDialog = function () {
    alert(Drupal.t('Error module initialize.'));
  }
  
  for (var i = 0; i < E.tpl.buttons.length; i++) {
    if (E.tpl.buttons[i][1] == 'js: E.showFileSelectionDialog();') {
      var $button = $('#bue-' + E.index + '-button-' + i);
      var $wrapper = $('<span class="bocu-button-wrapper bue-button"></span>').css({
        'background-image'    : $button.css('background-image'),
        'background-position' : $button.css('background-position')
      })
      $button.wrap($wrapper);
      $button.parents('.bue-ui').append('<span id="bocu-progress-' + E.index + '"></span>');
      $button.uploadify({
        'uploader'     : '/sites/all/modules/bocu/static/uploadify/uploadify.swf',
        'cancelImg'    : '/sites/all/modules/bocu/static/uploadify/cancel.png',
        'width'        : $button.width(),
        'height'       : $button.height() - 1,
        'hideButton'   : true,
        'wmode'        : 'transparent',
        'auto'         : true,
        'multi'        : true,
        'queueID'      : 'bocu-progress-' + E.index,
        'sizeLimit'    : Drupal.settings.bocu.sizeLimit,
        'script'       : Drupal.settings.basePath + 'bocu/upload',
        'scriptData'   : {'phpsessid':Drupal.settings.bocu.phpsessid},
        'fileDataName' : 'files[bocu]',
        'onSelectOnce' : function(event, data) {
          var selectedText = BUE.active.getSelection();
          if (selectedText.length > 0) {
            $button.uploadifySettings('scriptData', {selectedText:selectedText});
          }
        },
        'onComplete'   : function(event, ID, fileObj, response, data) {
          if (response.substring(0, 1) != '{') {
            return alert(response);
          }
          response = $.parseJSON(response);
          if (response.status) {
            BUE.active.replaceSelection(response.data + "\n", 'end');
          }
          else {
            alert(response.data);
          }
          BUE.active.focus();
        },
      });
      break;
    }
  }
};