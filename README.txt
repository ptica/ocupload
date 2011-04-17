Install module:
---------------
1. Extract module archive in "sites/all/modules"

2. Enable module "One Click Upload"

3. Download SWFUpload v2.2.0.1 from http://code.google.com/p/swfupload/downloads/list, extract archive in "sites/all/libraries" and rename dir "SWFUpload v2.2.0.1 Core" to "swfupload". After this file "swfupload.js" must be available on path "sites/all/libraries/swfupload/swfupload.js". Drush users can use the command "drush ocupload-dl-library"


Integrate to standalone BUEditor:
----------------------
1. Open BUEditor config page "admin/config/content/bueditor"

2. Click "Edit" link for your use editor.

3. Add new button with code: "js: E.showFileSelectionDialog();" (do not change this code!)

4. Click "Save configuration"


Integrate to standalone CKEditor (not Wysiwyg module):
-------------------------------------------
1. Open in text editor file "sites/all/modules/ckeditor/ckeditor.config.js"

2. Add to bottom, but before "};" next code:

    config.extraPlugins += (config.extraPlugins ? ',OCUpload' : 'OCUpload');
    CKEDITOR.plugins.addExternal('OCUpload', Drupal.settings.ocupload.modulePath + '/static/');

3. Open CKEditor config page "admin/config/content/ckeditor"

4. Click "edit" link for your use editor.

5. Open section "Editor appearance" and replace value in "Toolbar"

    ['Image','Flash','Table','HorizontalRule','Smiley','SpecialChar'],

to:

    ['OCUpload','Image','Flash','Table','HorizontalRule','Smiley','SpecialChar'],


6. Click "Save".

7. Clear your browser cache.


Integrate to CKEditor with Wysiwyg:
-----------------------------------
1. Open Wysiwyg cofig page admin/config/content/wysiwyg.

2. Click  Edit link for your use editor.

3. Open fieldset Buttons and plugins and mark checkbox One Click UPload.

4. Click Save.
