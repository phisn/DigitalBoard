#pragma once

#define COMMUNICATION_WEBPART_COLLECT R"__(
<body> 
	<div class="container"> 
		<p>Waiting for other players...</p>
		<p>Currently <b>2</b> Online</p>
	</div>
</body>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const collect = PROGMEM COMMUNICATION_WEBPART_COLLECT;
		const size_t collect_size = sizeof(COMMUNICATION_WEBPART_COLLECT);
	}
}
