#include "WebServerManager.h"

#include "../../Communication/WebInterface/WebHandlerCollect.h"
#include "../../Communication/WebInterface/WebHandlerRequestPid.h"
#include "../../Communication/WebInterface/WebHandlerRunning.h"

namespace
{
	WebInterface* interfaces[COMMON_MAX_PLAYERCOUNT] = { };
}

namespace Communication
{
	namespace WebServerManager
	{
		AsyncWebServer* _GetServer()
		{
			static AsyncWebServer* const server = new AsyncWebServer(80);
			return server;
		}

		AsyncWebSocket* _GetSocket()
		{
			static AsyncWebSocket* const socket = new AsyncWebSocket("/ws");
			return socket;
		}

		void HandleCommonRequest(AsyncWebServerRequest* const request);

		void HandleWebSocketEvent(
			AsyncWebSocket* const socket,
			AsyncWebSocketClient* const client,
			const AwsEventType type,
			void* const arg,
			uint8_t* const data,
			const size_t length);

		void Initialize()
		{
			AsyncWebServer* const server = _GetServer();
			AsyncWebSocket* const socket = _GetSocket();

			server->on(WEB_DIR_COMMON, HTTP_GET, HandleCommonRequest);

			server->on(WEB_DIR_COLLECT, HTTP_GET, WebHandler::HandleCollectRequest);
			server->on(WEB_DIR_RUNNING, HTTP_GET, WebHandler::HandleRunningRequest);
			server->on(WEB_DIR_REQPID, HTTP_GET, WebHandler::HandleReqPidRequest);

			server->on(WEB_DIR_RUNNING, HTTP_POST, WebHandler::HandleRunningPost);

			server->onNotFound([](AsyncWebServerRequest* const request)
				{
					request->redirect(WEB_DIR_COMMON);
				});

			socket->onEvent(HandleWebSocketEvent);
			server->addHandler(socket);

			server->begin();
		}

		void HandleCommonRequest(AsyncWebServerRequest* const request)
		{
			switch (Game::Controller::GetState())
			{
			case Game::GameState::Collect:
				request->redirect(WEB_DIR_COLLECT);

				break;
			case Game::GameState::Running:
				request->redirect(WEB_DIR_RUNNING);

				break;
			}
		}

		Game::PlayerId EnforcePlayerId(AsyncWebServerRequest* const request)
		{
			const Game::PlayerId playerId = FindPlayerId(request);

			if (playerId == 0)
			{
				request->redirect(WEB_DIR_REQPID);
			}

			return playerId;
		}

		Game::PlayerId FindPlayerId(AsyncWebServerRequest* const request)
		{
			// read and validate
			AsyncWebHeader* const cookieHeader = request->getHeader(F("Cookie"));

			if (cookieHeader != nullptr) // nullptr on fail
			{
				const String& cookie = cookieHeader->value();

				DEBUG_MESSAGE("Got Cookie");
				DEBUG_MESSAGE(cookie);

				if (cookie.length() > 5)
				{
					const int position = cookie.indexOf("pid=");
					DEBUG_MESSAGE("COOKIE POS AND COOKIE");
					DEBUG_MESSAGE(cookie.substring(position + 4, position + 4 + 3));
					DEBUG_MESSAGE(cookie.substring(position + 4, position + 4 + 3));

					return FindPlayerId(
						cookie.substring(position + 4, position + 4 + 3).toInt()
					);
				}
			}

			DEBUG_MESSAGE("NO PID FOUND");

			return 0;
		}

		Game::PlayerId FindPlayerId(const Game::PlayerId suspected)
		{
			DEBUG_MESSAGE("SUSPECTED");
			DEBUG_MESSAGE(suspected);

			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
				if (Game::Collector::GetData()->playerIds[i] == suspected)
				{
					DEBUG_MESSAGE("PID FOUND");
					DEBUG_MESSAGE(suspected);

					return suspected;
				}

			DEBUG_MESSAGE("NO PID FOUND");
			return 0;
		}

		void HandleWebSocketEvent(
			AsyncWebSocket* const socket,
			AsyncWebSocketClient* const client,
			const AwsEventType type,
			void* const arg,
			uint8_t* const data,
			const size_t length)
		{
			switch (type)
			{
			case WS_EVT_CONNECT:
				// arg: request (AsyncWebServerRequest*)
				// data: NULL
				// length: 0
			{
				DEBUG_MESSAGE("CONNECT");

				AsyncWebParameter* const pidParam = ((AsyncWebServerRequest*)arg)->getParam("pid");

				if (pidParam == NULL)
				{
					DEBUG_MESSAGE("INVALID PARAM CONNECT");

					client->close(WebCode::WebSocketInvalidParam);
					return;
				}

				const Game::PlayerId pid = FindPlayerId(pidParam->value().toInt());

				if (pid == 0)
				{
					DEBUG_MESSAGE("NO PID CONNECT");

					client->close(WebCode::WebSocketInvalidPid);
					return;
				}

				DEBUG_MESSAGE("CORRECT PID CONNECT");

				for (int i = 0; i < COMMON_MAX_PLAYERCOUNT; ++i)
				{
					DEBUG_MESSAGE("CHECKING");
					DEBUG_MESSAGE(i);
					DEBUG_MESSAGE((int)interfaces[i]);

					if (interfaces[i] && interfaces[i]->getPlayerId() == pid)
					{
						if (interfaces[i]->getWebSocketId() == NULL)
						{
							DEBUG_MESSAGE(F("Found interface -> register"));

							interfaces[i]->registerWebSocket(client->id());
							interfaces[i]->update();
						}
						else
						{
							DEBUG_MESSAGE(F("PID already in usage, redirecting websocket"));

							WebSocketData::RawData data{ WebSocketData::Type::InvalidPid };
							client->binary((const char*) &data, sizeof(data));
							client->close();
						}

						break;
					}
				}

				DEBUG_MESSAGE("Searching end!");

			}
			break;
			case WS_EVT_DISCONNECT:
				// arg: NULL
				// data: NULL
				// length: 0

				DEBUG_MESSAGE("DISCONNECT");

				for (int i = 0; i < COMMON_MAX_PLAYERCOUNT; ++i)
					if (interfaces[i] && interfaces[i]->getWebSocketId() == client->id())
					{
						interfaces[i]->unregisterWebSocket();
					}

				break;
			case WS_EVT_ERROR:
				// arg: reasoncode (uint16_t)
				// data: reasonString (char*)
				// length: datalen

				// TODO: thinking about showing reasoncode 
				// and reasonstring in faultmanager fault

				break;
			case WS_EVT_DATA:
				// arg: _pinfo (AwsFrameInfo*)
				// data: data (uint8_t*)
				// length: datalen

				// no data will be ever sent from
				// a client!

				break;
			case WS_EVT_PONG:
				// arg: NULL
				// data: data (uint8_t*)
				// length: datalen

				break;
			}
		}

		void RegisterWebInterface(WebInterface* const interface)
		{
			for (int i = 0; i < COMMON_MAX_PLAYERCOUNT; ++i)
				if (interfaces[i] == NULL)
				{
					interfaces[i] = interface;
					return;
				}
		}

		void UnregisterWebInterface(WebInterface* const interface)
		{
			for (int i = 0; i < COMMON_MAX_PLAYERCOUNT; ++i)
				if (interfaces[i] == interface)
				{
					DEBUG_MESSAGE("UNREGISTER");
					DEBUG_MESSAGE(i);

					if (interfaces[i]->getWebSocketId() != NULL)
					{
						_GetSocket()->close(interfaces[i]->getWebSocketId(), WebCode::WebSocketUnregister);
					}

					interfaces[i] = NULL;
					return;
				}
		}

		WebInterface* FindWebInterface(const Game::PlayerId playerId)
		{
			for (int i = 0; i < COMMON_MAX_PLAYERCOUNT; ++i)
				if (interfaces[i] && interfaces[i]->getPlayerId() == playerId)
				{
					return interfaces[i];
				}

			return NULL;
		}

		void SendWebSocketData(
			WebsocketId wid,
			const char* const data,
			const int length)
		{
			_GetSocket()->binary(wid, data, length);
		}
	}
}
