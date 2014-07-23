<?php
// Remove print pdf link from pdf file. 
// Do not show link if print module is not installed.
if(in_array('print_pdf', module_list()) && arg(0) != 'printpdf') {
  print print_pdf_insert_link(); 
}
?>
<table class="prim_matrix">
  <tr>
    <td class="prim_matrix_title"><?php print $elevensnamn; ?></td>
    <td class="prim_matrix_title">E</td>
    <td class="prim_matrix_title">C</td>
    <td class="prim_matrix_title">A</td>
  </tr>

  <tr>
    <td class="prim_matrix_capability">Begrepp</td>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Eb'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Cb'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Ab'])); ?>
  </tr>

  <tr>
    <td class="prim_matrix_capability">Procedur</td>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Ep'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Cp'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Ap'])); ?>
  </tr>

  <tr>
    <td class="prim_matrix_capability">Problem-lösning</td>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Epl'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Cpl'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Apl'])); ?>
  </tr>

  <tr>
    <td class="prim_matrix_capability">Modellering</td>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Em'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Cm'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Am'])); ?>
  </tr>

  <tr>
    <td class="prim_matrix_capability">Resonemang</td>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Er'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Cr'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Ar'])); ?>
  </tr>

  <tr>
    <td class="prim_matrix_capability">Kommunikation</td>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Ek'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Ck'])); ?>
<?php print theme('prim_teacher_result_questions',array('grades'=>$variables['grades']['Ak'])); ?>
  </tr>

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