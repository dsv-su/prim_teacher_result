<?php

/**
 * @file
 * Administration pages provided by Prim Matrix module.
 */


/**
 * Menu callback for admin/config/prim_teacher_result.
 * @param $form
 *   The settings form.
 * @param $form_state
 *   The form state.
 * @return
 *   system_settings_form($form)
 */
function prim_teacher_result_admin_form() {

   $db_result = db_query( "select mid, name, abbreviation, weight from prim_matrix order by weight");   
    // create array and add one element called data
    $rows= array();
    $form['#tree'] = TRUE;
    $max = 60;
    foreach($db_result as $row){   
        $name = $row->name;
        $abbreviation = $row->abbreviation;
        if(strlen($name)>$max)
            $name = substr($name,0,$max).' ...';
        $form[$row->mid]['mid'] = array(
            '#type' => 'hidden',      
            '#default_value' => $row->mid,       
        );
        // Textfield to hold content id.
        $form[$row->mid]['name'] = array(
            '#type' => 'item',        
            '#title' => $name
        );     

        // Textfield to hold content id.
        $form[$row->mid]['abbreviation'] = array(
            '#type' => 'item',        
            '#title' => $abbreviation
        );   

        // Build a list of operations.
        $operations = array();
        $operations['edit'] = array('#type' => 'link', '#title' => t('edit'), '#href' => 'admin/prim/prim_teacher_result/' . $row->mid . '/edit');
        $operations['delete'] = array('#type' => 'link', '#title' => t('delete'), '#href' => 'admin/prim/prim_teacher_result/' . $row->mid . '/delete');
        $form[$row->mid]['operations'] = $operations;

        // This field is invisible, but contains sort info (weights).
        $form[$row->mid]['weight'] = array(
            '#type' => 'weight',
            '#title' => t('Weight'),
            '#title_display' => 'invisible',
            '#default_value' => $row->weight,
        );
    }
    $form['actions'] = array('#type' => 'actions'); 
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));
    return $form;
}

function prim_teacher_result_admin_form_submit($form, &$form_state) {
    // When dealing with saving menu items, the order in which these items are
    // saved is critical. If a changed child item is saved before its parent,
    // the child item could be saved with an invalid path past its immediate
    // parent. To prevent this, save items in the form in the same order they
    // are sent by $_POST, ensuring parents are saved first, then their children.
    // See http://drupal.org/node/181126#comment-632270
    $order = array_flip(array_keys($form_state['input'])); // Get the $_POST order.
    $form = array_merge($order, $form); // Update our original form with the new order.

    $updated_items = array();
    $fields = array('weight');
    foreach (element_children($form) as $mid) {
        if (isset($form[$mid]['#item'])) {
            $element = $form[$mid];
            // Update any fields that have changed in this menu item.
        foreach ($fields as $field) {
            if ($element[$field]['#value'] != $element[$field]['#default_value']) {
                $element['#item'][$field] = $element[$field]['#value'];
                $updated_items[$mid] = $element['#item'];
            }
        }
        // Hidden is a special case, the value needs to be reversed.
        if ($element['hidden']['#value'] != $element['hidden']['#default_value']) {
            // Convert to integer rather than boolean due to PDO cast to string.
            $element['#item']['hidden'] = $element['hidden']['#value'] ? 0 : 1;
            $updated_items[$mid] = $element['#item'];
        }
    }
}

  // Save all our changed items to the database.
  foreach ($updated_items as $item) {
    $item['customized'] = 1;
  }
  drupal_set_message(t('Your configuration has been saved.'));
}
 

function theme_prim_teacher_result_admin_form($variables) {

    $form = $variables['form'];

    drupal_add_tabledrag('prim-overview', 'order', 'sibling', 'prim-weight');

    $header = array(
        t('name'),
        t('abbreviation'),
        t('weight'),
        array('data' => t('Operations'), 'colspan' => '3'),
    );
  
    $rows = array();
    foreach (element_children($form) as $mid) {
      if (isset($form[$mid]['weight'])) {
        $element = &$form[$mid];
        // Build a list of operations.
        $operations = array();
        foreach (element_children($element['operations']) as $op) {
            $operations[] = array('data' => drupal_render($element['operations'][$op]), 'class' => array('prim-operations'));
        }
        while (count($operations) < 2) {
            $operations[] = '';
        }

        // Add special classes to be used for tabledrag.js.
        $element['mid']['#attributes']['class'] = array('prim-mid');
        $element['weight']['#attributes']['class'] = array('prim-weight');

        $row = array();
        $row[] = drupal_render($element['name']);
        $row[] = drupal_render($element['abbreviation']);
        $row[] = drupal_render($element['weight']);
        $row = array_merge($row, $operations);

        $row = array_merge(array('data' => $row), $element['#attributes']);
        $row['class'][] = 'draggable';
        $rows[]= $row;
      }
    }

    $output = '';
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'prim-overview')));
    $output .= drupal_render_children($form);
  
    return $output;
 
}


/**
 * Menu callback; Build the menu link editing form.
 */
function prim_teacher_result_admin_edit_item($form, &$form_state, $type, $item) {
  if ($type == 'add' || empty($item)) {
    // This is an add form, initialize the menu link.
    $item = array('name' => '', 'mid' => 0, 'abbreviation' => '', 'weight' => 0);
  }
  else {
    // Get the human-readable menu title from the given menu name.
    $db_result = db_query( "select mid, name, abbreviation, weight from prim_matrix where mid = " . $item  ." order by weight");
    $db_item = db_fetch_array($db_result)
    $mid = $item->mid;
    $name = $db_item->name;
    $abbreviation = $db_item->abbreviation;
    $weight = $db_item->weight;
  }

  $form['actions'] = array('#type' => 'actions');
  foreach (array('name', 'mid', 'abbreviation', 'weight') as $key) {
    $form[$key] = array('#type' => 'value', '#value' => $item[$key]);
  }
  // Any item created or edited via this interface is considered "customized".
  $form['customized'] = array('#type' => 'value', '#value' => 1);
  $form['original_item'] = array('#type' => 'value', '#value' => $item);

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Category'),
    '#default_value' => $item['name'],
    '#description' => t('The name of the Category'),
    '#required' => TRUE,
  );

  $form['abbreviation'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#default_value' => $item['abbreviation'],
    '#rows' => 1,
    '#description' => t('The Abbreviation that is used to connect this item to the result fields'),
    '#required' => TRUE,
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#delta' => 50,
    '#default_value' => $item['weight'],
    '#description' => t('Optional. In the Matrix, the heavier categories will sink and the lighter categories will be positioned nearer the top.'),
  );
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

/**
 * Process menu and menu item add/edit form submissions.
 */
function prim_teacher_result_admin_edit_item_submit($form, &$form_state) {
  $item = &$form_state['values'];

  if ($item['mid'] == 0) {
    db_insert('prim_matrix')->fields(array('mid' => $item['mid'], 'name' => $item['name'], 'abbreviation' => $item['abbreviation'], 'weight' => $item['weight']))->execute();
    drupal_set_message(t('Your category has been added'));
  }
  else {
    db_update('prim_matrix')->fields(array('mid' => $item['mid'], 'name' => $item['name'], 'abbreviation' => $item['abbreviation'], 'weight' => $item['weight']))->condition('mid', $item['mid'])->execute();
    drupal_set_message(t('Your category has been updated'));
  }
  $form_state['redirect'] = 'admin/prim/prim_teacher_result/';
}

/**
 * Menu callback; Check access and present a confirm form for deleting a menu link.
 */
function prim_teacher_result_admin_delete_item($item) {
  // Links defined via hook_menu may not be deleted. Updated items are an
  // exception, as they can be broken.
  return drupal_get_form('prim_teacher_result_admin_delete_form', $item);
}

/**
 * Build a confirm form for deletion of a single menu link.
 */
function prim_teacher_result_admin_delete_form($form, &$form_state, $item) {
  $form['#item'] = $item;
  return confirm_form($form, t('Are you sure you want to delete the Matrix Category?', array('%item' => $item)), 'admin/prim/prim_teacher_result/' . $item . '/delete');
}

/**
 * Process menu delete form submissions.
 */
function prim_teacher_result_admin_delete_form_submit($form, &$form_state) {
  $item = $form['#item'];
  db_delete('prim_matrix')->condition('mid', $item['mid'])->execute();
  $t_args = array('%title' => $item['name']);
  drupal_set_message(t('The category %title has been deleted.', $t_args));
  $form_state['redirect'] = 'admin/prim/prim_teacher_result/';
}