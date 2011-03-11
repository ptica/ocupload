<?php
// $Id$

/**
 * @file
 * Service functions
 */


/**
 * Admin page with templates list
 */
function bocu_config_page() {
  $templates = bocu_templates();
  $table_header = array(t('Mask'), t('Operations'));
  $table_data = array();
  $output = '';
  
  foreach ($templates as $template) {
    $actions = array(
      l(t('edit'), 'admin/config/content/bocu/edit/' . $template->tid),
      l(t('delete'), 'admin/config/content/bocu/delete/' . $template->tid),
    );
    $table_data[] = array($template->mask, implode(' | ', $actions));
  }
  
  $output .= '<h3>' . t('Templates') . '</h3>';
  $output .= theme('table', array('header' => $table_header, 'rows' => $table_data));
  
  return $output;
}


/**
 * Form add/edit template
 */
function bocu_form_template($form, &$form_state, $template = NULL) {
  $public_path = variable_get('file_public_path', conf_path() . '/files');

  if (!$template) {
    $template = (object)array(
      'tid' => 0,
      'mask' => '',
      'path' => '',
      'template' => '<a href="!filepath">!filename</a>',
      'template_select' => '<a href="!filepath">!text</a>',
      'image_style' => '',
      'link_to_original' => 0,
      'link_only_big' => 0,
      'rename_file' => 1,
    );
    drupal_set_title(t('Add template'));
  }
  
  $form['tid'] = array(
    '#type' => 'value',
    '#value' => $template->tid,
  );
  
  $form['mask'] = array(
    '#type' => 'textfield',
    '#title' => t('File extensions'),
    '#description' => t('File extensions comma separated, which should be handled within this template. Example: <code>jpg,gif,png</code>'), // Список расширений файлов, через запятую, которые нужно обрабатывать в рамках этого шаблона. Например: <code>jpg,gif,png</code>
    '#default_value' => $template->mask,
    '#size' => 40,
    '#required' => TRUE,
  );
  
  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Upload path'),
    '#description' => t('Dir name, relative <code>!path</code>, with will upload files. Without the initial and trailing slashes.', array('!path' => $public_path)), // Имя директории, относительно <code>!path</code>, в которую будут загружаться файлы. Без начального и завершающего слэша.
    '#default_value' => $template->path,
    '#field_prefix' => $public_path . '/',
    '#size' => 22,
    '#required' => TRUE,
  );
  
  if (module_exists('token')) {
    $form['token_help'] = array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
  
    $form['token_help']['help'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('user'),
    );
  }
  
  $form['template'] = array(
    '#type' => 'textfield',
    '#title' => t('Template to insert into editor'),
    '#default_value' => $template->template,
    '#maxlength' => NULL,
    '#size' => 100,
    '#required' => TRUE,
  );
  
  $form['template_select'] = array(
    '#type' => 'textfield',
    '#title' => t('Template to replace selected text'),
    '#default_value' => $template->template_select,
    '#maxlength' => NULL,
    '#size' => 100,
    '#required' => TRUE,
  );
  
  $form['help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['help']['text'] = array(
    '#markup' => '
      !filepath     — ' . t('full path to file') . '<br />
      !filename     — ' . t('file name after upload') . '<br />
      !originalname — ' . t('original file name') . '<br />
      !filesize     — ' . t('file size') . '<br />
      !text         — ' . t('selected text') . '<br />
    ',
  );
  
  if (module_exists('image')) {
    $form['image_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Options for images'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    
    $form['image_settings']['image_style'] = array(
      '#type' => 'select',
      '#title' => t('Use image style'),
      '#options' => _bocu_get_image_styles(),
      '#default_value' => $template->image_style,
    );
    
    $form['image_settings']['link_to_original'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add link to original image'),
      '#default_value' => $template->link_to_original,
      '#states' => array('invisible' => array(
        'select[name="image_style"]' => array('value' => ''),
      )),
    );
    
    $form['image_settings']['link_only_big'] = array(
      '#type' => 'checkbox',
      '#title' => t('Add link only for images whose size exceeds the specified style'),
      '#default_value' => $template->link_only_big,
      '#states' => array('visible' => array(
        'input[name="link_to_original"]' => array('checked' => TRUE),
      )),
    );
  }
  
  $form['rename_file'] = array(
    '#type' => 'checkbox',
    '#title' => t('Rename uploaded file'),
    '#description' => t('Checked, if you want to rename uploded files to current date/time.'), // Отметьте, если хотите чтобы загружаемым файлам давались уникальные имена (дата и время загрузки).
    '#default_value' => $template->rename_file,
  );
  
  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles'),
    '#description' => t('Checked roles who can upload files of this type.'),
    '#options' => array_map('check_plain', user_roles()),
    '#default_value' => array_keys(user_roles(FALSE, 'upload files use template ' . $template->tid)),
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );
  
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => $template->tid ? t('Save template') : t('Add template'),
  );
  
  return $form;
}


/**
 * Form submit
 */
function bocu_form_template_submit($form, &$form_state) {
  $template = array(
    'tid'             => $form_state['values']['tid'],
    'mask'            => $form_state['values']['mask'],
    'path'            => $form_state['values']['path'],
    'template'        => $form_state['values']['template'],
    'template_select' => $form_state['values']['template_select'],
    'rename_file'     => $form_state['values']['rename_file'],
  );
  
  if (module_exists('image')) {
    $template += array(
      'image_style'      => $form_state['values']['image_style'],
      'link_to_original' => $form_state['values']['link_to_original'],
      'link_only_big'    => $form_state['values']['link_only_big'],
    );
  }
  
  bocu_template_save($template);
  
  foreach ($form_state['values']['roles'] as $rid => $enabled) {
    user_role_change_permissions($rid, array('upload files use template ' . $template['tid'] => $enabled));
  }
  
  drupal_set_message($form_state['values']['tid'] ? t('Template saved') : t('Template added'));
  $form_state['redirect'] = 'admin/config/content/bocu';
}


/**
 * Confirm form delete teamplate
 */
function bocu_delete_confirm($form, &$form_state, $template) {
  $form['tid'] = array(
    '#type' => 'value',
    '#value' => $template->tid,
  );

  return confirm_form(
    $form,
    t('Do you really want to delete the template') . ' ' . $template->tid . '?',
    'admin/config/content/bocu'
  );
}


/**
 * Confirm form submit
 */
function bocu_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    bocu_template_delete($form_state['values']['tid']);
    drupal_set_message(t('Deleted'));
  }

  drupal_goto('admin/config/content/bocu');
}


/**
 * Upload file
 */
function bocu_upload() {
  global $user, $base_url;
  
  _bocu_auth();
  
  $templates = bocu_templates();
  $extension_uploaded_file = _bocu_get_file_extension($_FILES['files']['name']['bocu']);
  $selected_text = !empty($_POST['selectedText']) ? $_POST['selectedText'] : '';

  foreach ($templates as $template) {
    if (in_array($extension_uploaded_file, explode(',', $template->mask)) && user_access('upload files use template ' . $template->tid)) {
      $destination = 'public://' . token_replace($template->path, array('user' => $user));
      file_prepare_directory($destination, FILE_CREATE_DIRECTORY);
      $file = file_save_upload('bocu', array(), $destination);
      
      if ($file) {
        if ($template->rename_file) {   
          $file = file_move($file, $destination . '/' . date('YdmHis') . '.' . $extension_uploaded_file);
        }
        
        $cur_template = $selected_text ? $template->template_select : $template->template;
        $filepath = $file->uri;
        
        // If upload image and template image style not empty
        if (_bocu_is_image($file->filename) && $template->image_style && module_exists('image')) {
          $filepath = image_style_path($template->image_style, $file->uri);
          
          if ($template->link_to_original) {
            $wrap_template = TRUE;
            if ($template->link_only_big) {
              $image_style = image_style_load($template->image_style);
              $image_style_width = $image_style['effects'][0]['data']['width'];
              $image_style_height = $image_style['effects'][0]['data']['height'];
              $image_info = image_get_info($file->uri);
              if (!(($image_style_width > 0 && $image_info['width'] > $image_style_width) || ($image_style_height > 0 && $image_info['height'] > $image_style_height))) {
                $wrap_template = FALSE;
              }
            }
            if ($wrap_template) {
              $cur_template = '<a href="' . _bocu_get_local_url($file->uri) . '" target="_blank">' . $cur_template . '</a>';
            }
          }
        }
        
        $data = strtr($cur_template, array(
          '!filepath'     => _bocu_get_local_url($filepath),
          '!filename'     => $file->filename,
          '!originalname' => basename($file->destination),
          '!filesize'     => format_size($file->destination),
          '!text'         => $selected_text,
        ));
        
        return _bocu_answer(TRUE, $data);
      }
      else {
        return _bocu_answer(FALSE, 'Error uploading file');
      }
      
      break;
    }
  }
  
  return _bocu_answer(FALSE, 'This file type can not be upload');
}


/**
 * Return all templates
 */
function bocu_templates() {
  return db_select('bocu_templates', 'bc')->fields('bc')->execute()->fetchAll();
}


/**
* Add/save template
*/
function bocu_template_save(&$template, $multi = FALSE) {
  if ($multi) {
    return array_map('bocu_template_save', $template);
  }
  
  return drupal_write_record('bocu_templates', $template, !empty($template['tid']) ? 'tid' : array());
}


/**
 * Delete template
 */
function bocu_template_delete($tid) {
  db_delete('bocu_templates')->condition('tid', $tid)->execute();
}


/**
 * Authorize by $_POST data
 */
function _bocu_auth() {
  $phpsessid = isset($_POST['phpsessid']) ? $_POST['phpsessid'] : FALSE;
  if ($phpsessid && session_id() != $phpsessid) {
    session_id($phpsessid);
    $_COOKIE[session_name()] = $phpsessid;
    drupal_session_initialize();
  }
}


/**
 * Return file extension
 */
function _bocu_get_file_extension($filename) {
  if (preg_match('#\.([a-zA-Z0-9]+)$#', $filename, $matches)) {
    return $matches[1];
  }
}


/**
 * Return local url by uri
 * Example: "_bocu_get_local_url('public://images/logo.jpg')" returned "/images/logo.jpg"
 */
function _bocu_get_local_url($uri) {
  return drupal_substr(file_create_url($uri), drupal_strlen($GLOBALS['base_url']));
}


/**
 * Return image styles for use in #options
 */
function _bocu_get_image_styles() {
  $image_styles = image_styles();
  $image_style_options = array('' => t('none'));
  foreach ($image_styles as $name => $image_style) {
    $image_style_options[$name] = $image_style['name'];
  }
  return $image_style_options;
}


/**
 * Return TRUE if file extension equal jpg,gif,png или bmp
 */
function _bocu_is_image($filename) {
  $extension = _bocu_get_file_extension($filename);
  return in_array($extension, array('jpg', 'jpeg', 'png', 'gif', 'bmp'));
}


/**
 * Return answer in json format
 */
function _bocu_answer($status, $data) {
  drupal_json_output(array('status' => $status, 'data' => $data));
}
