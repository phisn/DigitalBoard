#pragma once

#define COMMUNICATION_WEBPART_SETUP R"__(<script language="javascript"> 
	var pidParts=document.cookie.split('='); 
	var b='NULL'; 
	
	if(pidParts[0]==='pid')
	{
		b=pidParts[1];
	}
	
	document.getElementById('pid').innerHTML=b;
</script>
<body>
	<div class="container"> 
		<h3>Please wait...</h3> 
		<i>Your Player-ID is 
			<b id="pid">
			</b>
		</i> 
	</div>
</body>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const setup = PROGMEM COMMUNICATION_WEBPART_SETUP;
		const size_t setup_size = sizeof(COMMUNICATION_WEBPART_SETUP);
	}
}
