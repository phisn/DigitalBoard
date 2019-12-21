#pragma once

#define COMMUNICATION_WEBPART_REQUEST R"__(
<script language="javascript"> 
	function setCookie(){
		var pidHex=document.getElementById('pid').value; 
		document.cookie="pid=" + parseInt(pidHex, 16); 
		window.location="/";
	}
</script>
<body> 
	<div class="container"> 
		<h2>Request PlayerID</h2> 
		<select id="pid"> 
			<option value="ff">Black (0)</option> 
			<option value="e6">Red (1)</option> 
			<option value="78">Green (2)</option> 
			<option value="59">Blue (3)</option> 
			<option value="ca">Purple (4)</option> 
			<option value="1e">Yellow (5)</option> 
		</select> 
		<input type="submit" value="Request" formmethod="post" onClick="setCookie()"> 
	</div>
</body>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const request = PROGMEM COMMUNICATION_WEBPART_REQUEST;
		const size_t request_size = sizeof(COMMUNICATION_WEBPART_REQUEST);
	}
}
