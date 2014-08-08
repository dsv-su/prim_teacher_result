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
        $operations['edit'] = array('#type' => 'link', '#title' => t('edit'), '#href' => 'admin/prim/prim_teacher_result/category/' . $row->mid . '/edit');
        $operations['delete'] = array('#type' => 'link', '#title' => t('delete'), '#href' => 'admin/prim/prim_teacher_result/category/' . $row->mid . '/delete');
        $form[$row->mid]['operations'] = $operations;

        // This field is invisible, but contains sort info (weights).
        $form[$row->mid]['weight'] = array(
            '#type' => 'weight',
            '#title' => t('Weight'),
            '#title_display' => 'invisible',
            '#default_value' => $row->weight,
        );
    }
     
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save changes'));
    return $form;
}

function prim_teacher_result_admin_form_submit($form, &$form_state) {
    $slides = array(); 
    foreach ($form_state['values']['slides'] as $slide) {   
        $slides[] = array(
            'id' => $slide['mid'],       
            'weight' => $slide['weight'],
        );         
    }  
    if (!empty($slides)) {
        usort($slides, '_prim_teacher_result_arraysort');
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

function theme_prim_teacher_result_admin_form($variables) {

    $form = $variables['form'];

    drupal_add_tabledrag('prim-overview', 'order', 'sibling', 'prim-weight');

    $header = array(
        '',
        t('name'),
        t('abbreviation'),
        t('weight'),
        array('data' => t('Operations'), 'colspan' => '3'),
    );
  
    $rows = array();
    foreach (element_children($form) as $nid) {
        $element = &$form[$nid];
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
        $row[] = theme('indentation', array('size' => $element['#item']['depth'] - 1)) . drupal_render($element['name']);
        $row[] = drupal_render($element['weight']) . drupal_render($element['mid']) . drupal_render($element['weight']);
        $row = array_merge($row, $operations);

        $row = array_merge(array('data' => $row), $element['#attributes']);
        $row['class'][] = 'draggable';
        $rows[]= $row;
    }

    $output = '';
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'prim-overview')));
    $output .= drupal_render_children($form);
  
    return $output;
 
}


/**
 * Menu callback; Build the menu link editing form.
 */
function theme_prim_teacher_result_admin_edit_item($form, &$form_state, $type, $item, $menu) {
  if ($type == 'add' || empty($item)) {
    // This is an add form, initialize the menu link.
    $item = array('link_title' => '', 'mlid' => 0, 'plid' => 0, 'menu_name' => $menu['menu_name'], 'weight' => 0, 'link_path' => '', 'options' => array(), 'module' => 'menu', 'expanded' => 0, 'hidden' => 0, 'has_children' => 0);
  }
  else {
    // Get the human-readable menu title from the given menu name.
    $titles = menu_get_menus();
    $current_title = $titles[$item['menu_name']];

    // Get the current breadcrumb and add a link to that menu's overview page.
    $breadcrumb = menu_get_active_breadcrumb();
    $breadcrumb[] = l($current_title, 'admin/structure/menu/manage/' . $item['menu_name']);
    drupal_set_breadcrumb($breadcrumb);
  }
  $form['actions'] = array('#type' => 'actions');
  $form['link_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Menu link title'),
    '#default_value' => $item['link_title'],
    '#description' => t('The text to be used for this link in the menu.'),
    '#required' => TRUE,
  );
  foreach (array('link_path', 'mlid', 'module', 'has_children', 'options') as $key) {
    $form[$key] = array('#type' => 'value', '#value' => $item[$key]);
  }
  // Any item created or edited via this interface is considered "customized".
  $form['customized'] = array('#type' => 'value', '#value' => 1);
  $form['original_item'] = array('#type' => 'value', '#value' => $item);

  $path = $item['link_path'];
  if (isset($item['options']['query'])) {
    $path .= '?' . drupal_http_build_query($item['options']['query']);
  }
  if (isset($item['options']['fragment'])) {
    $path .= '#' . $item['options']['fragment'];
  }
  if ($item['module'] == 'menu') {
    $form['link_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#maxlength' => 255,
      '#default_value' => $path,
      '#description' => t('The path for this menu link. This can be an internal Drupal path such as %add-node or an external URL such as %drupal. Enter %front to link to the front page.', array('%front' => '<front>', '%add-node' => 'node/add', '%drupal' => 'http://drupal.org')),
      '#required' => TRUE,
    );
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#access' => $item['mlid'],
      '#submit' => array('menu_item_delete_submit'),
      '#weight' => 10,
    );
  }
  else {
    $form['_path'] = array(
      '#type' => 'item',
      '#title' => t('Path'),
      '#description' => l($item['link_title'], $item['href'], $item['options']),
    );
  }
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => isset($item['options']['attributes']['title']) ? $item['options']['attributes']['title'] : '',
    '#rows' => 1,
    '#description' => t('Shown when hovering over the menu link.'),
  );
  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => !$item['hidden'],
    '#description' => t('Menu links that are not enabled will not be listed in any menu.'),
  );
  $form['expanded'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show as expanded'),
    '#default_value' => $item['expanded'],
    '#description' => t('If selected and this menu link has children, the menu will always appear expanded.'),
  );

  // Generate a list of possible parents (not including this link or descendants).
  $options = menu_parent_options(menu_get_menus(), $item);
  $default = $item['menu_name'] . ':' . $item['plid'];
  if (!isset($options[$default])) {
    $default = 'navigation:0';
  }
  $form['parent'] = array(
    '#type' => 'select',
    '#title' => t('Parent link'),
    '#default_value' => $default,
    '#options' => $options,
    '#description' => t('The maximum depth for a link and all its children is fixed at !maxdepth. Some menu links may not be available as parents if selecting them would exceed this limit.', array('!maxdepth' => MENU_MAX_DEPTH)),
    '#attributes' => array('class' => array('menu-title-select')),
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#delta' => 50,
    '#default_value' => $item['weight'],
    '#description' => t('Optional. In the menu, the heavier links will sink and the lighter links will be positioned nearer the top.'),
  );
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}