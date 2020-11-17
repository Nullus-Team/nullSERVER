function user_focus()
{
	document.getElementById("user-tab").style.backgroundColor="#E6E6E6";
	document.getElementById("user-tbox").style.backgroundColor="#E6E6E6";
	if (document.getElementById("wrong-pass").innerHTML!="") $("#wrong-pass").remove();
}
function user_focusout()
{
	document.getElementById("user-tab").style.backgroundColor="#FFFFFF";
	document.getElementById("user-tbox").style.backgroundColor="#FFFFFF";
	if (document.getElementById("user-tbox").value.length==0) 
	{
		document.getElementById("error-u").innerHTML = "You did not enter username";
		document.getElementById("user-tab").style.backgroundColor="#FFCECF";
		document.getElementById("user-tbox").style.backgroundColor="#FFCECF";
	}
	else 
		document.getElementById("error-u").innerHTML = "";
}
function pass_focus()
{
	document.getElementById("pass-tab").style.backgroundColor="#E6E6E6";
	document.getElementById("pass-tbox").style.backgroundColor="#E6E6E6";
	if (document.getElementById("wrong-pass").innerHTML!="") $("#wrong-pass").remove();
}
function pass_focusout()
{
	document.getElementById("pass-tab").style.backgroundColor="#FFFFFF";
	document.getElementById("pass-tbox").style.backgroundColor="#FFFFFF";
	if (document.getElementById("pass-tbox").value.length==0) 
	{
		document.getElementById("error-p").innerHTML = "You did not enter password";
		document.getElementById("pass-tab").style.backgroundColor="#FFCECF";
		document.getElementById("pass-tbox").style.backgroundColor="#FFCECF";
	}
	else
		document.getElementById("error-p").innerHTML = "";
}
function lockpass_focus()
{
	document.getElementById("lock-cre-block").style.backgroundColor="#FFFFFF";
	document.getElementById("pass-lockbox").style.backgroundColor="#FFFFFF";
	if (document.getElementById("wrong-lock").innerHTML!="") $("#wrong-lock").remove();
}
function lockpass_focusout()
{
	document.getElementById("lock-cre-block").style.backgroundColor="#E6E6E6";
	document.getElementById("pass-lockbox").style.backgroundColor="#E6E6E6";
	if (document.getElementById("pass-lockbox").value.length==0) 
	{
		document.getElementById("error-pl").innerHTML = "You did not enter password";
		document.getElementById("lock-cre-block").style.backgroundColor="#FFCECF";
		document.getElementById("pass-lockbox").style.backgroundColor="#FFCECF";
	}
	else
		document.getElementById("error-pl").innerHTML = "";
}

$(document).ready(function() {
    $('.add-dialog-open').click(function() {

        var loginBox = $(this).attr('href');
        var deviceID = $(this).attr('deviceid');
        var deviceName = $(this).attr('dname');

        if (deviceID!="") 
        	{
        		document.getElementById("fake-input").setAttribute('value', deviceID);
        		document.getElementById("mod_name_title").innerHTML = deviceName;
        	}


        $(loginBox).fadeIn("slow");

        var overname = $(this).attr('overname');


        $('body').append('<div class="over" id="'.concat(overname,'"></div>'));
        $('#'.concat(overname)).fadeIn(300);
        
        return false;
    });


    $(document).on('click', "button.close-dialog, #add-over", function() { 
        $('#add-over, #add-dialog-box').fadeOut(300 , function() {
            $('#add-over').remove();  
            let boxes = ["name", "place", "type", "state"];
            boxes.forEach(reset_form_valid);
            document.getElementById("err_add_form").innerHTML = "";
        }); 
        return false;
    });

    $(document).on('click', "button.close-dialog-mod, #mod-over", function() { 
        $('#mod-over, #mod-dialog-box').fadeOut(300 , function() {
            $('#mod-over').remove();  
            let boxes = ["mod_name", "mod_place", "mod_type", "mod_state"];
            boxes.forEach(reset_form_valid);
            document.getElementById("err_mod_form").innerHTML = "";
        }); 
        return false;
    });

});
function reset_form_valid(object)
{
	document.getElementById(object.concat("_box")).style.borderColor = "#e6e6e6";
	document.getElementById(object.concat("_box")).style.borderWidth = "1px 1px 1px 1px";
}
function add_form_focus(object)
{
	document.getElementById(object.concat("_box")).style.borderColor = "#7d9eff";
	document.getElementById(object.concat("_box")).style.borderWidth = "medium";
	if (document.getElementById("err_add_form").innerHTML!="") document.getElementById("err_add_form").innerHTML="";
}
function add_form_focusout(object)
{
	document.getElementById(object.concat("_box")).style.borderColor = "#e6e6e6";
	document.getElementById(object.concat("_box")).style.borderWidth = "1px 1px 1px 1px";
	if (document.getElementById('device_'.concat(object)).value.length==0) {
		document.getElementById(object.concat("_box")).style.borderColor = "#f37278";
		document.getElementById(object.concat("_box")).style.borderWidth = "medium";
	}
}

function getInputVal(object) {
	return (document.getElementById('device_'.concat(object)).value);
}
function addFormValidate(type) {
	if (type===1) 
	{
		var prefix = "mod_";
	} else var prefix="";
	if (getInputVal(prefix.concat('name')).length==0 || getInputVal(prefix.concat('type')).length==0 || 
		getInputVal(prefix.concat('place')).length==0 || getInputVal(prefix.concat('state')).length==0)
	{
		if (prefix=="")
			document.getElementById("err_add_form").innerHTML = "Please fulfill all fields";
		else document.getElementById("err_mod_form").innerHTML = "Please fulfill all fields";
		return false;
	}
	return true;
}

function TOGGLE(device_id) {
		let req = new XMLHttpRequest();
		req.open('GET', 'index.php?device_id=' + device_id + '&value=TOGGLE', true);
		req.send();
		if (document.getElementById(device_id).className=='box-button') 
				document.getElementById(device_id).className+=' button-off';
			else document.getElementById(device_id).className='box-button'
}
