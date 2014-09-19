<?php

// The following two functions are taken from webform module since we need to customize the submissions output

/**
 * Theme the header of the submissions table.
 *
 * This is done in it's own function so that webform can retrieve the header and
 * use it for sorting the results.
 */
function theme_teacher_results_submissions_header($variables) {
  drupal_get_path('module', 'webform');
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  
  $node = $variables['node'];

  $columns = array(
    // Edited the title of the first column
    array('data' => t('Elevens namn'), 'field' => 'sid', 'sort' => 'desc'),
    array('data' => t('Submitted'), 'field' => 'submitted'),
  );
  if (webform_results_access($node)) {
    $columns[] = array('data' => t('User'), 'field' => 'name');
    $columns[] = array('data' => t('IP Address'), 'field' => 'remote_addr');
  }
  $columns[] = array('data' => t('Operations'), 'colspan' => module_exists('print') ? 5 : 3);

  return $columns;
}

/**
 * Retrieve lists of submissions for a given webform.
 */
function teacher_results_submissions($node, $user_filter, $pager_count) {
  global $user;

  if (isset($_GET['results']) && is_numeric($_GET['results'])) {
    $pager_count = $_GET['results'];
  }

  $header = theme('teacher_results_submissions_header', array('node' => $node));
  if ($user_filter) {
    if ($user->uid) {
      drupal_set_title(t('Submissions for %user', array('%user' => $user->name)), PASS_THROUGH);
    }
    else {
      drupal_set_title(t('Your submissions'));
      webform_disable_page_cache();
    }
    webform_set_breadcrumb($node);
    $submissions = webform_get_submissions(array('nid' => $node->nid, 'uid' => $user->uid), $header, $pager_count);
    $count = webform_get_submission_count($node->nid, $user->uid);
  }
  else {
    $submissions = webform_get_submissions($node->nid, $header, $pager_count);
    $count = webform_get_submission_count($node->nid);
  }

  $operation_column = end($header);
  $operation_total = $operation_column['colspan'];

  $rows = array();
  foreach ($submissions as $sid => $submission) {

    // Set up student's name instead of submission id
    $fieldid = webform_get_cid($node, 'elevens_namn', webform_get_cid($node, 'bakgrundsinformation_om_eleven', 0));
    print_r($submissions);
    $elevensnamn = $submission->data[$fieldid][0];
    if (!$elevensnamn) {$elevensnamn = t($sid);};

    $row = array(
      $submission->is_draft ? t('@sid (draft)', array('@sid' => $elevensnamn)) : $elevensnamn,
      format_date($submission->submitted, 'short'),
    );
    if (webform_results_access($node, $user)) {
      $row[] = theme('username', array('account' => $submission));
      $row[] = $submission->remote_addr;
    }
    $row[] = l(t('View'), "node/$node->nid/submission/$sid");
    $operation_count = 1;
    // No need to call this multiple times, just reference this in a variable.
    $destination = drupal_get_destination();
    if (webform_submission_access($node, $submission, 'edit', $user)) {
      $row[] = l(t('Edit'), "node/$node->nid/submission/$sid/edit", array('query' => $destination));
      $operation_count++;
    }
    if (webform_submission_access($node, $submission, 'delete', $user)) {
      $row[] = l(t('Delete'), "node/$node->nid/submission/$sid/delete", array('query' => $destination));
      $operation_count++;
    }
    if ($operation_count < $operation_total) {
      $row[count($row) - 1] = array('data' => $row[count($row) - 1], 'colspan' => $operation_total - $operation_count + 1);
    }
    $rows[] = $row;
  }

  $element['#theme'] = 'webform_results_submissions';
  $element['#node'] = $node;
  $element['#submissions'] = $submissions;
  $element['#total_count'] = $count;
  $element['#pager_count'] = $pager_count;
  $element['#attached']['library'][] = array('webform', 'admin');

  $element['table']['#theme'] = 'table';
  $element['table']['#header'] = $header;
  $element['table']['#rows'] = $rows;
  $element['table']['#operation_total'] = $operation_total;

  return drupal_render($element);

}


function teacher_results_download($node, $format = 'delimited', $options = array()) {
  global $user;
  module_load_include('inc', 'webform', 'includes/webform.export');
  module_load_include('inc', 'webform', 'includes/webform.components');

  $submission_information = array(
    'serial' => t('Serial'),
    'sid' => t('SID'),
    'time' => t('Time'),
    'draft' => t('Draft'),
    'ip_address' => t('IP Address'),
    'uid' => t('UID'),
    'username' => t('Username'),
  );

  if (empty($options)) {
    $options = array(
      'delimiter' => variable_get('webform_csv_delimiter', '\t'),
      'components' => array_merge(array_keys($submission_information), array_keys(webform_component_list($node, 'csv', TRUE))),
      'select_keys' => 0,
      'select_format' => 'separate',
      'range_type' => 'all',
    );
  }
  else {
    foreach ($submission_information as $key => $label) {
      if (!in_array($key, $options['components'])) {
        unset($submission_information[$key]);
      }
    }
  }

  // Open a new Webform exporter object.
  $exporter = webform_export_create_handler($format, $options);

  $file_name = drupal_tempnam('temporary://', 'webform_');
  $handle = @fopen($file_name, 'w'); // The @ suppresses errors.
  $exporter->bof($handle);

  // Fill in the header for the submission information (if any).
  $header[2] = $header[1] = $header[0] = count($submission_information) ? array_fill(0, count($submission_information), '') : array();
  if (count($submission_information)) {
    $header[0][0] = $node->title;
    $header[1][0] = t('Submission Details');
    foreach (array_values($submission_information) as $column => $label) {
      $header[2][$column] = $label;
    }
  }

  // Compile header information for components.
  foreach ($options['components'] as $cid) {
    if (isset($node->webform['components'][$cid])) {
      $component = $node->webform['components'][$cid];

      // Let each component determine its headers.
      if (webform_component_feature($component['type'], 'csv')) {
        $component_header = (array) webform_component_invoke($component['type'], 'csv_headers', $component, $options);
        $header[0] = array_merge($header[0], (array) $component_header[0]);
        $header[1] = array_merge($header[1], (array) $component_header[1]);
        $header[2] = array_merge($header[2], (array) $component_header[2]);
      }
    }
  }

  // Add headers to the file.
  foreach ($header as $row) {
    $exporter->add_row($handle, $row);
  }

  // Get all the required submissions for the download.
  $filters['nid'] = $node->nid;
  if (!empty($options['sids'])){
    $filters['sid'] = $options['sids'];
  }

  // Only get the teachers submissions
  $filters['uid'] = $user->uid;
  $submissions = webform_get_submissions($filters);

  // Generate a row for each submission.
  $row_count = 0;
  foreach ($submissions as $sid => $submission) {
    $row_count++;

    $row = array();
    if (isset($submission_information['serial'])) {
      $row[] = $row_count;
    }
    if (isset($submission_information['sid'])) {
      $row[] = $sid;
    }
    if (isset($submission_information['time'])) {
      $row[] = format_date($submission->submitted, 'small');
    }
    if (isset($submission_information['draft'])) {
      $row[] = $submission->is_draft;
    }

    // Show only IP-address to admins.
    if((in_array('formuläradmin', $user->roles) || in_array('administrator', $user->roles))) { 
      if (isset($submission_information['ip_address'])) {
        $row[] =  $submission->remote_addr;
      }
    }
    if (isset($submission_information['uid'])) {
      $row[] = $submission->uid;
    }
    if (isset($submission_information['username'])) {
      $row[] = $submission->name;
    }

    foreach ($options['components'] as $cid) {
      if (isset($node->webform['components'][$cid])) {
        $component = $node->webform['components'][$cid];
        // Let each component add its data.
        $raw_data = isset($submission->data[$cid]['value']) ? $submission->data[$cid]['value'] : NULL;
        if (webform_component_feature($component['type'], 'csv')) {
          $data = webform_component_invoke($component['type'], 'csv_data', $component, $options, $raw_data);
          if (is_array($data)) {
            $row = array_merge($row, array_values($data));
          }
          else {
            $row[] = isset($data) ? $data : '';
          }
        }
      }
    }

    // Write data from submissions.
    $data = $exporter->add_row($handle, $row);
  }

  // Add the closing bytes.
  $exporter->eof($handle);

  // Close the file.
  @fclose($handle);

  $export_name = _webform_safe_name($node->title);
  $exporter->set_headers($export_name);
  @readfile($file_name);  // The @ makes it silent.
  @unlink($file_name);  // Clean up, the @ makes it silent.

  // Update user last downloaded sid if required.
  if ($options['range_type'] != 'range' && $row_count > 0) {
    // Delete existing record.
    db_delete('webform_last_download')
      ->condition('nid', $node->nid)
      ->condition('uid', $user->uid)
      ->execute();
    // Write new record.
    db_insert('webform_last_download')
      ->fields(array(
        'nid' => $node->nid,
        'uid' => $user->uid,
        'sid' => $sid,
        'requested' => REQUEST_TIME,
      ))
      ->execute();
  }

  exit();
}

/**
 * Form to configure the download of CSV files.
 */
function teacher_results_download_form($form, &$form_state, $node) {
  module_load_include('inc', 'webform', 'includes/webform.export');
  module_load_include('inc', 'webform', 'includes/webform.components');

  $form = array();

  $form['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );

  $form['format'] = array(
    '#type' => 'radios',
    '#title' => t('Export format'),
    '#options' => webform_export_list(),
    '#default_value' => isset($form_state['values']['format']) ? $form_state['values']['format'] : variable_get('webform_export_format', 'delimited'),
  );

  $form['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimited text format'),
    '#description' => t('This is the delimiter used in the CSV/TSV file when downloading Webform results. Using tabs in the export is the most reliable method for preserving non-latin characters. You may want to change this to another character depending on the program with which you anticipate importing results.'),
    '#default_value' => isset($form_state['values']['delimiter']) ? $form_state['values']['delimiter'] : variable_get('webform_csv_delimiter', '\t'),
    '#options' => array(
      ','  => t('Comma (,)'),
      '\t' => t('Tab (\t)'),
      ';'  => t('Semicolon (;)'),
      ':'  => t('Colon (:)'),
      '|'  => t('Pipe (|)'),
      '.'  => t('Period (.)'),
      ' '  => t('Space ( )'),
    ),
  );

  $form['select_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select list options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['select_options']['select_keys'] = array(
    '#type' => 'radios',
    '#title' => t('Select keys'),
    '#options' => array(
      0 => t('Full, human-readable options (values)'),
      1 => t('Short, raw options (keys)'),
    ),
    '#default_value' => isset($form_state['values']['select_options']['select_keys']) ? $form_state['values']['select_options']['select_keys'] : 0,
    '#description' => t('Choose which part of options should be displayed from key|value pairs.'),
  );

  $form['select_options']['select_format'] = array(
    '#type' => 'radios',
    '#title' => t('Select list format'),
    '#options' => array(
      'separate' => t('Separate'),
      'compact' => t('Compact'),
    ),
    '#default_value' => isset($form_state['values']['select_options']['select_format']) ? $form_state['values']['select_options']['select_format'] : 'separate',
    '#attributes' => array('class' => array('webform-select-list-format')),
    '#theme' => 'webform_results_download_select_format',
  );

  $csv_components = array(
    'info' => t('Submission information'),
    'serial' => '-' . t('Submission Number'),
    'sid' => '-' . t('Submission ID'),
    'time' => '-' . t('Time'),
    'draft' => '-' . t('Draft'),
    'ip_address' => '-' . t('IP Address'),
    'uid' => '-' . t('User ID'),
    'username' => '-' . t('Username'),
  );
  $csv_components += webform_component_list($node, 'csv', TRUE);

  $form['components'] = array(
    '#type' => 'select',
    '#title' => t('Included export components'),
    '#options' => $csv_components,
    '#default_value' => isset($form_state['values']['components']) ? $form_state['values']['components'] : array_keys($csv_components),
    '#multiple' => TRUE,
    '#size' => 10,
    '#description' => t('The selected components will be included in the export.'),
    '#process' => array('webform_component_select'),
  );

  $form['range'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download range options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#theme' => 'webform_results_download_range',
    '#element_validate' => array('teacher_results_download_range_validate'),
    '#after_build' => array('teacher_results_download_range_after_build'),
  );

  $form['range']['range_type'] = array(
    '#type' => 'radios',
    '#options' => array(
      'all' => t('All submissions'),
      'new' => t('Only new submissions since your last download'),
      'latest' => t('Only the latest'),
      'range' => t('All submissions starting from'),
    ),
    '#default_value' => 'all',
  );
  $form['range']['latest'] = array(
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 8,
    '#default_value' => isset($form_state['values']['latest']) ? $form_state['values']['latest'] : '',
  );
  $form['range']['start'] = array(
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 8,
    '#default_value' => '',
  );
  $form['range']['end'] = array(
    '#type' => 'textfield',
    '#size' => 5,
    '#maxlength' => 8,
    '#default_value' => '',
    '#description' => '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Download'),
  );

  return $form;
}

/**
 * Validate handler for teacher_results_download_form().
 */
function teacher_results_download_form_submit(&$form, &$form_state) {
  $options = array(
    'delimiter' => $form_state['values']['delimiter'],
    'components' => array_keys(array_filter($form_state['values']['components'])),
    'select_keys' => $form_state['values']['select_keys'],
    'select_format' => $form_state['values']['select_format'],
    'range_type' => $form_state['values']['range']['range_type'],
  );

  // Retrieve the list of required SIDs.
  if ($options['range_type'] != 'all') {
    $options['sids'] = webform_download_sids($form_state['values']['node']->nid, $form_state['values']['range']);
  }

  teacher_results_download($form_state['values']['node'], $form_state['values']['format'], $options);
}

/**
 * FormAPI after build function for the download range fieldset.
 */
function teacher_results_download_range_after_build($element, &$form_state) {
  global $user;

  module_load_include('inc', 'webform', 'includes/webform.report');
  module_load_include('inc', 'webform', 'includes/webform.submissions');


  $node = $form_state['values']['node'];

  // Build a list of counts of new and total submissions.
  $count = webform_get_submission_count($node->nid, $user->uid);
  $sids = webform_download_sids($node->nid, array('range_type' => 'new'));

  $last_download = webform_download_last_download_info($node->nid);

  $element['#webform_download_info']['sid'] = $last_download ? $last_download['sid'] : 0;
  $element['#webform_download_info']['requested'] = $last_download ? $last_download['requested'] : $node->created;
  $element['#webform_download_info']['total'] = $count;
  $element['#webform_download_info']['new'] = count($sids);

  return $element;
}

/**
 * FormAPI element validate function for the range fieldset.
 */
function teacher_results_download_range_validate($element, $form_state) {
  switch ($element['range_type']['#value']) {
    case 'latest':
      // Download latest x submissions.
      if ($element['latest']['#value'] == '') {
        form_error($element['latest'], t('Latest number of submissions field is required.'));
      }
      else{
        if (!is_numeric($element['latest']['#value'])) {
          form_error($element['latest'], t('Latest number of submissions must be numeric.'));
        }
        else{
          if ($element['latest']['#value'] <= 0) {
            form_error($element['latest'], t('Latest number of submissions must be greater than 0.'));
          }
        }
      }
      break;
    case 'range':
      // Download Start-End range of submissions.
      // Start submission number.
      if ($element['start']['#value'] == '') {
        form_error($element['start'], t('Start submission number is required.'));
      }
      else{
        if (!is_numeric($element['start']['#value'])) {
          form_error($element['start'], t('Start submission number must be numeric.'));
        }
        else{
          if ($element['start']['#value'] <= 0) {
            form_error($element['start'], t('Start submission number must be greater than 0.'));
          }
        }
      }
      // End submission number.
      if ($element['end']['#value'] != '') {
        if (!is_numeric($element['end']['#value'])) {
          form_error($element['end'], t('End submission number must be numeric.'));
        }
        else{
          if ($element['end']['#value'] <= 0) {
            form_error($element['end'], t('End submission number must be greater than 0.'));
          }
          else{
            if ($element['end']['#value'] < $element['start']['#value']) {
              form_error($element['end'], t('End submission number may not be less than Start submission number.'));
            }
          }
        }
      }
      break;
  }

}