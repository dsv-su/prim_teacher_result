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
function prim_teacher_result_admin_form($form_state) {

    $form = array();
    $db_probe = db_query("select * FROM {prim_matrix} order by weight");
 
    $form['#tree'] = TRUE;
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
    return system_settings_form($form);
}