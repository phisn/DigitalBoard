#pragma once

#define COMMUNICATION_WEBPART_COMMONSOCKET R"__(
let cookie = document.cookie.replace(/(?:(?:^|.*;\s*)pid\s*\=\s*([^;]*).*$)|^.*$/, "$1");
var pid = parseInt(cookie, 10);
if (pid == 0 || isNaN(cookie)) {
	alert("Got invalid PlayerID, reconnecting soon");

	setTimeout(function() {
		location.reload();
	}, 300);
}

let socket = new WebSocket("ws://" + document.domain + "/ws?pid=" + cookie);
socket.binaryType = "arraybuffer";

socket.onmessage = function(event) {
	generate(new DataView(event.data));
})__"

namespace Communication
{
	namespace Webpart
	{
		const char* const commonSocket = PROGMEM COMMUNICATION_WEBPART_COMMONSOCKET;
		const size_t commonSocket_size = sizeof(COMMUNICATION_WEBPART_COMMONSOCKET);
	}
}
