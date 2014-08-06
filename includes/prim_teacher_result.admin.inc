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
function prim_teacher_result_admin_form($form, &$form_state) {

    $form = array();
    $db_probe = db_query("select * FROM {prim_matrix} order by weight");
 
    $form = array(
        '#tree' => TRUE,
        '#parent_fields' => FALSE,
    );    
    while ($row = db_fetch_array($db_probe)) {
        //Make sure all fields that need to be updated in the database (generally, only the ID and weight fields need this for this type of form) and their corresponding database fields have the same name.
         
        //The next two fields should both contain the applicable item title
        //Note how each field (name, id, and weight) is stored as a child of it's $row['id']. This allows each item to be displayed and acted upon as a row in a table.
         
        $form[$row['mid']]['name'] = $row['name'];
        $form[$row['mid']]['name']['#value'] = $row['name'];

        $form[$row['mid']]['abbrev'] = $row['abbreviation'];
        $form[$row['mid']]['abbrev']['#value'] = $row['abbreviation'];

         
        //The id of the field must be included so each  
        $form[$row['mid']]['mid'] = array(
            '#type' => 'hidden',
            '#value' => $row['mid']
        );
         
        //This is the weight field that determines where the item is sorted. The delta value is half of the maximum number of items plus 1 that can be sorted in this table. For example, we can sort 41 items in this table (-20 to 20).
        $form[$row['mid']]['weight'] = array(
            '#type' => 'weight',
            '#delta' => 20,
            '#default_value' => $row['weight']
        );
    }
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save configuration'),
    );
    return $form;
}

function theme_prim_teacher_result_display_items_table_form($variables) {

    $form = $variables['form'];
  //This function is the instantiator of the sorter. Make sure the 0th paramater is the id of your table, and the 3rd paramater is the class of your weight variable
    drupal_add_tabledrag('prim_matrix', 'name', 'sibling', 'theweight');
     

    $errors = form_get_errors() != FALSE ? form_get_errors() : array();
    //Define your table headers
    $header = array(
    t('Name'),
    t('Abbreviation'),
    t('Weight'),
    );
    $rows = array();
 
    //Loop through each item to display in the sortable table
    foreach (element_children($form) as $key) {
        if (isset($form[$key]['mid'])) {
             
            //Make this variable the weight class defined in the drupal_add_tabledrag function.
            $form[$key]['weight']['#attributes']['class'] = 'theweight';
     
            $row = array();
            //Define columns
            $row = array(
                drupal_render($form[$key]['name']), 
                drupal_render($form[$key]['abbreviation']), 
                drupal_render($form[$key]['weight'])
            ); 
            //Add to the $rows varaiable, which will be used to generate the sortable rows
              $rows[] = array(
                'data' => $row,
                'class' => 'draggable'
            );
        }
    }
     
    //Finally, output the sortable table. Make sure the id variable is the same as the table id in drupal_add_tabledrag
    $output = theme('table', $header, $rows, array('id' => 'prim_matrix'), "My Sorted Items");
    $output .= drupal_render($form);
    return $output;
 
}