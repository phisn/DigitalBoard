#pragma once

#include "../../Communication/InterfaceManager.h"

#include "../../Communication/WebInterface/WebServerManager.h"
#include "../../Communication/WebInterface/WebInterface.h"

#include "../../Game/GameController.h"

namespace
{
	using namespace Communication;

	void WritePlayerId(
		AsyncWebServerResponse* const response,
		const Game::PlayerId playerId)
	{
		// pid: + number + \0
		char pidBuffer[3 + 4 + 1] = "pid=";
		itoa(playerId, pidBuffer + 4, 10);

		for (int i = 4; i < 4 + 3 && pidBuffer[i] == ' '; ++i)
			pidBuffer[i] = '0';

		pidBuffer[sizeof(pidBuffer) - 1] = '\0';

		DEBUG_MESSAGE("Sending new cookie");
		DEBUG_MESSAGE(pidBuffer);

		response->addHeader(F("Set-Cookie"), pidBuffer);
	}
}

namespace Communication
{
	namespace Website
	{
		const char* Collect PROGMEM =
R"(
<!DOCTYPE html>
<html>
  <head>
    <script>
		let cookie = document.cookie.replace(/(?:(?:^|.*;\s*)pid\s*\=\s*([^;]*).*$)|^.*$/, "$1");

		if (parseInt(cookie, 10) == 0 || isNaN(cookie)) {
			alert("Got invalid PlayerID, reconnecting soon");

			setTimeout(function () {
				location.reload();
			}, 300);
		}

		let socket = new WebSocket("ws://" + document.domain + "/ws?pid=" + cookie);
		socket.binaryType = "arraybuffer";

		socket.onopen = function (event) {
		};

		socket.onmessage = function (event) {
			var data = event.data;
			var dataView = new DataView(data);

			var type = dataView.getInt8(0);

			switch (type) {
				case 1: // collect
					var playerCount = dataView.getInt8(1);
					document.getElementById("pcount").innerHTML = playerCount;

					break;
				case 4:
					window.location = "/";					

					break;
				default:
					// alert("Got invalid type");
					window.location = "/";					

					break;
			}
		};

		socket.onclose = function (event) {
			alert("Lost Connection!" + event.code + " " + event.reason + " " + event.wasClean);
		};
    </script>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <title></title>
  </head>
  <body>
    <p style="text-align: center;">Waiting for other players</p>
    <p style="text-align: center;">Currently <b id="pcount">loading ...</b> Online</p>
  </body>
</html>
)";
	}

	namespace WebHandler
	{
		void HandleCollectRequest(AsyncWebServerRequest* const request)
		{
			randomSeed(millis() + random(LONG_MAX));

			if (Game::Controller::GetState() != Game::GameState::Collect)
			{
				request->redirect(WEB_DIR_COMMON);
				return;
			}

			const Game::PlayerId playerId = WebServerManager::FindPlayerId(request);

			if (playerId == 0 && Game::Collector::GetData()->playerCount >= COMMON_MAX_PLAYERCOUNT)
			{
				request->redirect(WEB_DIR_REQPID);

				return;
			}

			AsyncWebServerResponse* const response = request->beginResponse_P(
				200,
				WEB_RESPONSE_TYPE,
				Website::Collect);

			if (playerId == 0)
			{
				WritePlayerId(
					response, 
					InterfaceManager::CreatePushedInterface<WebInterface>()->getPlayerId()
				);
			}

			request->send(response);
		}
	}
}
