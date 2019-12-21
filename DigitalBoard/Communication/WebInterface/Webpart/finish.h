#pragma once

#define COMMUNICATION_WEBPART_BEGIN R"__(
<body>
	<div class="container">
		<h2>Game Over!</h2> 
		<p>Winner: 
		<b>Villian</b>
	</div>
</body>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const begin = PROGMEM COMMUNICATION_WEBPART_BEGIN;
		const size_t begin_size = sizeof(COMMUNICATION_WEBPART_BEGIN);
	}
}

