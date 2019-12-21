#include "WebInterface.h"
#include "../../Communication/WebInterface/WebServerManager.h"

namespace Communication
{
	WebInterface::~WebInterface()
	{
		WebServerManager::UnregisterWebInterface(this);
	}

	void WebInterface::initialize(const Game::PlayerId id)
	{
		lastState = Game::Controller::GetState();

		playerId = id;
		unregisterWebSocket();

		WebServerManager::RegisterWebInterface(this);
	}

	bool WebInterface::process()
	{
		if (webSocketId == NULL)
		{
			timerCounter += millis() - timerLast;
			timerLast = millis();

			if (timerCounter > COMM_WEBINT_WEBSOCK_TIMEOUT)
			{
				DEBUG_MESSAGE("KICK MISSING WEBCLINET");
				DEBUG_MESSAGE(playerId);
				InterfaceManager::RemoveInterface(playerId);
				return true;
			}
		}

		return false;
	}

	inline void WebInterface::update()
	{
		DEBUG_MESSAGE("Update");

		if (webSocketId == 0)
		{
			DEBUG_MESSAGE("Fail 0");
			return;
		}

		const Game::GameState currentState = Game::Controller::GetState();
		if (lastState != currentState)
		{
			DEBUG_MESSAGE("Update, change state");

			lastState = currentState;

			WebSocketData::RawData data{ WebSocketData::Type::Redirect };
			sendData((const char*) &data, sizeof(data));
		}

		switch (lastState)
		{
		case Game::GameState::Collect:
			updateCollect();

			break;
		case Game::GameState::Running:
			updateRunning();

			break;
		}
	}

	void WebInterface::updateCollect()
	{
		DEBUG_MESSAGE("State collect");

		WebSocketData::CollectData data;
		data.playerCount = Game::Collector::GetData()->playerCount;
		sendData((const char*)&data, sizeof(data));
	}

	void WebInterface::updateSetup()
	{
	}

	void WebInterface::updateRunning()
	{
		DEBUG_MESSAGE("State running");
		const Game::Player* const player = Game::GameManager::ReadPlayer(playerId);

		if (player == NULL)
		{
			DEBUG_MESSAGE("Got empty player (players)");

#ifdef VM_DEBUG
			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
			{
				DEBUG_MESSAGE(Game::SetupManager::GetData()->playerContext.data[i].player);
			}
#endif

			DEBUG_MESSAGE("Searching: ");
			DEBUG_MESSAGE(playerId);

			return;
		}

		WebSocketData::RunningData data;

		data.playerCount = Game::Collector::GetData()->playerCount;
		data.currentPlayer = Game::GameManager::GetData()->state.activePlayer;

		data.playerType = player->data->type;
		data.position = player->state->position;

		data.yellowTicketCount = player->state->ticket.yellowTicketCount;
		data.greenTicketCount = player->state->ticket.greenTicketCount;
		data.redTicketCount = player->state->ticket.redTicketCount;

		data.positionVillianReveal = Game::GameManager::GetShowVillianPositionRound();

		if (data.playerType == Game::PlayerData::Type::Villian)
		{
			data.villian.blackTicketCount = player->state->villian.ticket.blackTicketCount;
			data.villian.doubleTicketCount = player->state->villian.ticket.doubleTicketCount;

			// skip villian
			for (int i = 1; i < Game::Collector::GetData()->playerCount; ++i)
			{
				const Game::Player* const detPlayer = Game::GameManager::ReadPlayerByIndex(i);

				data.villian.detYellowTickets[i - 1] = detPlayer->state->ticket.yellowTicketCount;
				data.villian.detGreenTickets[i - 1] = detPlayer->state->ticket.greenTicketCount;
				data.villian.detRedTickets[i - 1] = detPlayer->state->ticket.redTicketCount;
			}

			const Game::PathManager::FindOptionsSpecificResult result = Game::PathManager::FindOptionsSpecific(
				player->state->position,
				Game::Station::Ferry);

			data.villian.turnFerryCount = result.stationCount;

			// unrolled
			data.villian.turnFerry[0] = result.station[0];
			data.villian.turnFerry[1] = result.station[1];
		}
		else
		{
			data.detective.positionVillian = Game::GameManager::GetLastVillianPosition();

			// villian index is always 0
			const Game::Player* const villian = Game::GameManager::ReadPlayerByIndex(0);

			data.detective.villianYellowTicketCount = villian->state->ticket.yellowTicketCount;
			data.detective.villianGreenTicketCount = villian->state->ticket.greenTicketCount;
			data.detective.villianRedTicketCount = villian->state->ticket.redTicketCount;

			data.detective.villianBlackTicketCount = villian->state->villian.ticket.blackTicketCount;
			data.detective.villianDoubleTicketCount = villian->state->villian.ticket.doubleTicketCount;

			data.detective.color = player->data->color;
		}

		const Game::PathManager::FindOptionsSpecificResult
			taxiResult = Game::PathManager::FindOptionsSpecific(
				player->state->position,
				Game::Station::Taxi),
			busResult = Game::PathManager::FindOptionsSpecific(
				player->state->position,
				Game::Station::Bus),
			undergroundResult = Game::PathManager::FindOptionsSpecific(
				player->state->position,
				Game::Station::Underground);

		data.turnYellowCount = taxiResult.stationCount;
		data.turnGreenCount = busResult.stationCount;
		data.turnRedCount = undergroundResult.stationCount;

		memcpy(data.turnYellow, taxiResult.station, taxiResult.stationCount);
		memcpy(data.turnGreen, busResult.station, busResult.stationCount);
		memcpy(data.turnRed, undergroundResult.station, undergroundResult.stationCount);

		sendData((const char*)&data, sizeof(data));
	}

	void WebInterface::sendData(
		const char* const data,
		const int length)
	{
		WebServerManager::SendWebSocketData(webSocketId, data, length);
	}
}
