#include "Collector.h"

#include "../Device/FaultHandler.h"

namespace
{
	Game::Collector::Data* data;

	namespace Fault
	{
		// interface has the responsibility to ensure playercount
		FlashString player_overflow = DEVICE_FAULT_MESSAGE("Player overflow caused by Interfc");
	}
}

namespace Game
{
	Collector::Collector(Collector::Data* const data)
	{
		::data = data;
	}

	void Collector::Create()
	{
		data->playerCount = 0;
	}

	bool Collector::Process()
	{
		if (needsUpdate)
		{
			needsUpdate = false;
			return true;
		}

		return false;
	}

	bool Collector::Restore()
	{
		// can not be restored
		return false;
	}

	bool Collector::Finish()
	{
		return data->playerCount > 4;
	}

	PlayerId Collector::CreatePlayer()
	{
		if (data->playerCount >= DEF_PLAYER_COUNT)
		{
			Device::FaultHandler::Handle(
				{
					Device::FaultModule::Collector,
					(Device::FailureId) FID::PLAYER_OVERFLOW,
					Fault::player_overflow
				}, true);
		}

		PlayerId nextPlayerId;
		while (true)
		{
			nextPlayerId = (PlayerId) rand();

			for (int i = 0; i < data->playerCount; ++i)
				if (nextPlayerId == data->playerIds[i])
				{
					nextPlayerId = 0;
					break;
				}

			if (nextPlayerId != 0)
			{
				break;
			}
		}

		data->playerIds[data->playerCount++] = nextPlayerId;

		needsUpdate = true;
		return nextPlayerId;
	}

	bool Collector::RemovePlayer(const PlayerId playerId)
	{
		for (int i = 0; i < data->playerCount; ++i)
			if (data->playerIds[i] == playerId)
			{
				for (int j = i; j < data->playerCount - 1; ++j)
				{
					data->playerIds[j] = data->playerIds[j + 1];
				}

				--data->playerCount;

				needsUpdate = true;
				return true;
			}

		return false;
	}

	bool Collector::ExistsPlayer(const PlayerId playerId) const
	{
		return CollectorAccess::ExistsPlayer(playerId);
	}

	int Collector::FindPlayerIndex(const PlayerId playerId) const
	{
		return CollectorAccess::FindPlayerIndex(playerId);
	}

	namespace CollectorAccess
	{
		bool ExistsPlayer(const PlayerId playerId)
		{
			for (int i = 0; i < data->playerCount; ++i)
				if (data->playerIds[i] == playerId)
				{
					return true;
				}

			return false;
		}

		// -1 if not found
		int FindPlayerIndex(const PlayerId playerId)
		{
			for (int i = 0; i < data->playerCount; ++i)
				if (data->playerIds[i] == playerId)
				{
					return i;
				}

			return -1;
		}

		const CollectData* GetData()
		{
			return data;
		}
	}
}
