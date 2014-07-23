// JavaScript Document
// Replace Visa with Ny registrering

jQuery(document).ready(function(){
  jQuery('.tabs.primary li ').each(function(){
    if(jQuery(this).find('a:first').text() == 'Visa'){
      jQuery(this).find('a').html("Visa registrering");
    } else if(jQuery(this).find('a:first').text() == 'Visa(aktiv flik)') {
      jQuery(this).find('a').html("Visa registrering");
    }
  });
});