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

   $db_result = db_query( "select mid, name, weight from prim_matrix order by weight");   
    // create array and add one element called data
    $rows= array();
    $form['#tree'] = TRUE;
    $max = 60;
    foreach($db_result as $row){   
        $name = $row->name;
        if(strlen($name)>$max)
            $name = substr($name,0,$max).' ...';
        $form['slides'][$row->mid]['mid'] = array(
            '#type' => 'hidden',      
            '#default_value' => $row->mid,       
        );
        // Textfield to hold content id.
        $form['slides'][$row->mid]['name'] = array(
            '#type' => 'item',        
            '#title' => $name
        );     
        // This field is invisible, but contains sort info (weights).
        $form['slides'][$row->mid]['weight'] = array(
            '#type' => 'weight',
            '#title' => t('Weight'),
            '#title_display' => 'invisible',
            '#default_value' => $row->weight,
        );
    }
     
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));
    return $form;
}

function prim_teacher_result_admin_submit($form, &$form_state) {
    $slides = array(); 
    foreach ($form_state['values']['slides'] as $slide) {   
        $slides[] = array(
            'id' => $slide['mid'],       
            'weight' => $slide['weight'],
        );         
    }  
    if (!empty($slides)) {
        usort($slides, '_module_name_arraysort');
    }  
    $weight = 1;
    foreach($slides as $slide){
        $id = $slide['id'];
        $sql = "UPDATE prim_matrix SET weight={$weight} WHERE mid = {$id}";
        db_query($sql);
        $weight++;
    }
  
    drupal_set_message(t('Ordering have been saved.'));
}
 
// Custom array sort function by weight.
function _prim_teacher_result_arraysort($a, $b) {
    if (isset($a['weight']) && isset($b['weight'])) {
        return $a['weight'] < $b['weight'] ? -1 : 1;
    }
    return 0;
}

function theme_prim_teacher_result_admin($form) {

    $form = $variables['form'];
  
    $rows = array();
    foreach (element_children($form['slides']) as $nid) {
        $form['slides'][$nid]['weight']['#attributes']['class'] = array('slides-order-weight');
        $rows[] = array(
            'data' => array(
                array('class' => array('slide-cross')),               
                    drupal_render($form['slides'][$nid]['title']),
                    drupal_render($form['slides'][$nid]['weight']),       
                ),
            'class' => array('draggable'),
        );
    }
  
    $header = array('',t('title'),t('weight'));
    $output = drupal_render($form['note']);
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'slides-order')));
    $output .= drupal_render_children($form);
  
    drupal_add_tabledrag('slides-order', 'order', 'sibling', 'slides-order-weight');
  
    return $output;
 
}