Install:
=============================
1. Download Uploadify v2.1.4 from www.uploadify.com and extract in
   "sites/all/libraries/uploadify" so that file
   "jquery.uploadify.v2.1.4.min.js" was available at
   "sites/all/libraries/uploadify/jquery.uploadify.v2.1.4.min.js".
   Drush users can use the command "drush download-uploadify".
3. Extract module archive in "sites/all/modules"
4. Enable module "BUEditor one click upload" in "admin/modules"
5. Add new button to BUEditor with code: "js: E.showFileSelectionDialog();"
6. (Optional) Edit templates in admin/config/content/bocu
