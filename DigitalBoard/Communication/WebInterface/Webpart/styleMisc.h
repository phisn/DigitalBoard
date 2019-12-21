#pragma once

#define COMMUNICATION_WEBPART_STYLEMISC R"__(p, div, h2{text-align: center;}
)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const styleMisc = PROGMEM COMMUNICATION_WEBPART_STYLEMISC;
		const size_t styleMisc_size = sizeof(COMMUNICATION_WEBPART_STYLEMISC);
	}
}
