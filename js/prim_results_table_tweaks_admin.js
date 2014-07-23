// JavaScript Document
//append a new table heading named "Matris"
jQuery(document).ready(function(){
	jQuery('.sticky-enabled.tableheader-processed.sticky-table  tr:first ').append("<th class='TableHeading'>Matris</th>");    

	//loop through all the tr are are not first
	jQuery('.sticky-enabled.tableheader-processed tr:not(:first)').each(function(){
	//find the fifth row and replace the link caption to "Visa Registering"
	//not needed anymore, we would like to bring back username
	//jQuery(this).find("td").eq(2).find("a").html("Visa registering");

	//assign its url to a variable
	var visa_url = jQuery(this).find("td").eq(4).find("a").attr('href');
	//append the matrix link to the url
	visa_url = visa_url + '/matrix';
	//add a matris row link in the table
	jQuery(this).append("<td><a href='"+visa_url+"'>Visa matris</a></td>");
 });
});
