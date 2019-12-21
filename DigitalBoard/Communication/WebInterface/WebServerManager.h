#pragma once

#include "../../Communication/WebInterface/WebInterface.h"
#include "../../Game/GameController.h"

#define WEB_DIR_COMMON ("/")
#define WEB_DIR_COLLECT ("/collect")
#define WEB_DIR_RUNNING ("/running")
#define WEB_DIR_REQPID ("/requestpid")

#define WEB_PARAM_PLAYERID ("playerid")
#define WEB_RESPONSE_TYPE ("text/html")

#define WEB_MAX_CONN 10

namespace Communication
{
	typedef uint32_t WebsocketId;

	struct WebCode
	{
		enum
		{
			_Begin = 2,

			WebSocketUnregister,
			WebSocketInvalidParam,
			WebSocketInvalidPid,
			WebSocketPidInUsage
		};
	};

	namespace WebServerManager
	{
		void Initialize();

		Game::PlayerId FindPlayerId(AsyncWebServerRequest* const request);
		Game::PlayerId FindPlayerId(const Game::PlayerId suspected);
		Game::PlayerId EnforcePlayerId(AsyncWebServerRequest* const request);

		void RegisterWebInterface(WebInterface* const interface);
		void UnregisterWebInterface(WebInterface* const interface);

		WebInterface* FindWebInterface(const Game::PlayerId playerId);

		void SendWebSocketData(
			WebsocketId wid,
			const char* const data, 
			const int length);
	}
}
