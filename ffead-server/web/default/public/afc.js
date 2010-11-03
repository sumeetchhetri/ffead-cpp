function prepForm(arr)
{
	var form = new Element('form', { 'display': 'hidden' });
	form.innerHTML = '';
	if(arr!=null)
	{
		for(var i=0;i<arr.length;i++) 
		{
			var inp = new Element('input', { 'type': 'hidden' ,'name': arr[i].key ,'value': arr[i].value});
			form.appendChild(inp);
		}
	}
	document.body.appendChild(form);
	return form;
}

function afcPrototypeAjaxCall()
{
	alert("ASDSADASD");
	var form = prepForm(null);
	alert(form);
	var obj = new Array();
	obj.push({key:"userName",value:"tempor"});
	prepForm(obj);
	var opt = {
	    method: 'post',
	    postBody: Form.serialize($('afc-form')) + '&ajax=true',
	    onSuccess: function(response) 
		{
			var msg = response.responseText;
			alert(msg);
	   	}
	}
	document.body.removeChild(form);
	new Ajax.Request('/', opt);
}

function AfcCall(claz,meth,param)
{	
	var form = prepForm(null);
	alert("ASDSADASD");
	alert(form);
	var obj = new Array();
	var postdata = "";
	obj.push({key:"claz",value:claz});
	postdata += "claz="+claz+"&";
	obj.push({key:"method",value:meth});
	postdata += "method="+meth+"&";
	obj.push({key:"paramsize",value:param.length});
	postdata += "paramsize="+param.length+"&";
	
	for(var i=1;i<param.length+1;i++)
	{
		obj.push({key:"param_"+i,value:param[i-1]});
		postdata += "param_"+i+"="+param[i-1];
		if(i!=param.length)
			postdata += "&";
		//obj.push({key:"type_"+i,value:types[i-1]});
	}
	//prepForm(obj);
	var opt = {
	    method: 'post',
	    postBody: postdata,
	    //postBody: Form.serialize($('afc-form')) + '&ajax=true',
	    onSuccess: function(response) 
		{
			var msg = response.responseText;
			alert(msg);
	   	}
	}
	new Ajax.Request('/', opt);
	//alert(postdata);
	document.body.removeChild(form);
}

function testWsCall()
{	
	var form = prepForm(null);
	alert("ASDSADASD");
	alert(form);
	var obj = new Array();
	var postdata = "";
	postdata += '<?xml version="1.0"?><soap:Envelope xmlns:soap="http://www.w3.org/2001/12/soap-envelope" soap:encodingStyle="http://www.w3.org/2001/12/soap-encoding"><soap:Body xmlns:tns="http://www.example.org/stock"><tns:test4><arg0>IBM</arg0></tns:test4></soap:Body></soap:Envelope>';
	var opt = {
	    method: 'post',
	    postBody: postdata,
	    contentType: 'application/soap+xml',
	    onSuccess: function(response) 
		{
			var msg = response.responseText;
			alert(msg);
	   	}
	}
	new Ajax.Request('/Testing', opt);
	//alert(postdata);
	document.body.removeChild(form);
}

function callServerFunc()
{
	//document.getElementById();
}
