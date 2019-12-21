#pragma once

#define COMMUNICATION_WEBPART_STYLECONTROLLER R"__(div,
h3 {
	text-align: center;
}

button {
	margin-right: 3px;
}

.black {
	background-color: black;
	padding: 20px;
	margin-bottom: 10px;
	border-radius: 15px;
	width: max-content;
	display: inline-block;
}

.taxi {
	background-color: yellow;
	padding: 20px;
	margin-bottom: 10px;
	border-radius: 15px;
	width: max-content;
	display: inline-block;
}

.bus {
	background-color: green;
	padding: 20px;
	margin-bottom: 10px;
	border-radius: 15px;
	width: max-content;
	display: inline-block;
}

.underground {
	background-color: red;
	padding: 20px;
	margin-bottom: 10px;
	border-radius: 15px;
	width: max-content;
	display: inline-block;
}

.moves {
	display: inline-block;
	overflow: visible;
})__"

namespace Communication
{
	namespace Webpart
	{
		const char* const styleController = PROGMEM COMMUNICATION_WEBPART_STYLECONTROLLER;
		const size_t styleController_size = sizeof(COMMUNICATION_WEBPART_STYLECONTROLLER);
	}
}
