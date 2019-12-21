#pragma once

#include "../../Communication/InterfaceManager.h"
#include "../../Communication/WebInterface/WebSocketData.h"

#include "../../Game/GameController.h"

#define COMM_WEBINT_WEBSOCK_TIMEOUT 2000
#define COMM_WEBINT_WEBSOCK_PING_INT 3000

namespace Communication
{
	class WebInterface
		:
		public Interface
	{
	public:
		~WebInterface();
		void initialize(const Game::PlayerId id) override;

		void notifyFault(const Device::Fault fault) override
		{
		}

		bool process() override;
		void update() override;

		void registerWebSocket(const uint32_t id)
		{
			DEBUG_MESSAGE("Register Socket");
			webSocketId = id;
		}

		void unregisterWebSocket()
		{
			webSocketId = NULL;

			timerCounter = 0;
			timerLast = millis();
		}

		uint32_t getWebSocketId() const
		{
			return webSocketId;
		}

		const Game::PlayerId getPlayerId() const
		{
			return playerId;
		}

	private:
		void updateCollect();
		void updateSetup();
		void updateRunning();

		void sendData(const char* const data, const int length);

		uint32_t webSocketId = 0;

		unsigned long timerCounter = 0;
		unsigned long timerLast = 0;

		Game::GameState lastState;
		Game::PlayerId playerId;
	};
}
