<?php
// Remove print pdf link from pdf file. 
// Do not show link if print module is not installed.
if(in_array('print_pdf', module_list()) && arg(0) != 'printpdf') {
  print print_pdf_insert_link(); 
}
$db_result = db_query( "select mid, name, abbreviation, weight from prim_matrix order by weight"); 
$rows= array();
?>
<table class="prim_matrix">
  <tr>
    <td class="prim_matrix_title"><?php print $elevensnamn; ?></td>
    <td class="prim_matrix_title">E</td>
    <td class="prim_matrix_title">C</td>
    <td class="prim_matrix_title">A</td>
  </tr>
<?php
foreach($db_result as $row){
?>
  <tr>
    <td class="prim_matrix_capability"><?php $row->name ?></td>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['E' . <?php $row->abbreviation ?> ])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['C' . <?php $row->abbreviation ?>])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['A' . <?php $row->abbreviation ?>])); ?>
  </tr>
  <?php
}
?>
    <tr>
    <td></td>
    <td class="prim_matrix_title">
<?php print $variables['grades']['E']['correct']['count']; ?> av <?php print $variables['grades']['E']['all']['count']; ?>
    </td>
    <td class="prim_matrix_title">
<?php print $variables['grades']['C']['correct']['count']; ?> av <?php print $variables['grades']['C']['all']['count']; ?>
    </td>
    <td class="prim_matrix_title">
<?php print $variables['grades']['A']['correct']['count']; ?> av <?php print $variables['grades']['A']['all']['count']; ?>
    </td>
  </tr>
</table>
<p class="prim_matrix_info">
<?php
if(isset($variables['node']->field_matrix_info['und'][0]['safe_value'])) {
  print $variables['node']->field_matrix_info['und'][0]['safe_value'];
}
?>
</p>
<div class="prim_total">
  <span>Totalt</span>
  <div>
<?php print
  $variables['grades']['E']['correct']['count'] +
  $variables['grades']['C']['correct']['count'] +
  $variables['grades']['A']['correct']['count'];
?>
  </div>
</div>


<h2 class="prim_comments_header">Eventuella kommentarer </h2>
<div class="prim_comments"></div>