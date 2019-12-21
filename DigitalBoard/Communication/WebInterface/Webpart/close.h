#pragma once

#define COMMUNICATION_WEBPART_CLOSE R"__(</html>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const close = PROGMEM COMMUNICATION_WEBPART_CLOSE;
		const size_t close_size = sizeof(COMMUNICATION_WEBPART_CLOSE);
	}
}
