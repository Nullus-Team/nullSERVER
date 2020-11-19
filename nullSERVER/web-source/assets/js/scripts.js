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
$(document).ready(function(){
	$(".content-box").hover(function(){
	  $(this).addClass("shadow");
	  }, function(){
	  $(this).removeClass("shadow");
	});
	$(".content-box").mousedown(function(){
	  $(this).css("background-color", "#e8eeff");
	  }
	);
	$(".content-box").mouseup(function(){
	  $(this).css("background-color", "#fafafa");
	  }
	);
});