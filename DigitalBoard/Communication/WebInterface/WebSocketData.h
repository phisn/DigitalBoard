#pragma once

#include "../../Game/Data/PlayerData.h"

namespace Communication
{
	namespace WebSocketData
	{
		struct Type 
		{
			enum : unsigned char
			{
				Invalid,
				
				Collect,
				Setup,
				Running,

				Redirect,
				InvalidPid,
				Error

			} type;
		};

		struct CollectData
		{
			Type type{ Type::Collect }; // 1 byte
			int8_t playerCount;			// 1 byte
		};

		struct SetupData
		{
			Type type{ Type::Setup };
		};

		struct RunningData
		{
			Type type{ Type::Running }; // 1 byte

			int8_t playerCount;
			Game::PlayerId currentPlayer;

			/*	Villian		= 0,
				Detective	= 1
			*/
			Game::PlayerData::Type playerType; // 1 byte

			uint8_t position;

			int8_t yellowTicketCount;
			int8_t greenTicketCount;
			int8_t redTicketCount;

			int8_t positionVillianReveal;

			union // 8 byte offset
			{
				struct
				{
					int8_t blackTicketCount;
					int8_t doubleTicketCount;

					// size is playercount
					int8_t detYellowTickets[5];
					int8_t detGreenTickets[5];
					int8_t detRedTickets[5];

					int8_t turnFerryCount;
					uint8_t turnFerry[2];

				} villian;

				struct
				{
					uint8_t positionVillian;

					int8_t villianYellowTicketCount;
					int8_t villianGreenTicketCount;
					int8_t villianRedTicketCount;
					int8_t villianBlackTicketCount;
					int8_t villianDoubleTicketCount;

					/*  Black	= 0,

						Red		= 1,
						Green	= 2,
						Blue	= 3,

						Purple	= 4,
						Yellow	= 5,
					*/

					Game::Color color; // 1 byte

				} detective;
			}; // 28 byte = 20 + 8 offset

			int8_t turnYellowCount;
			int8_t turnGreenCount;
			int8_t turnRedCount;

			uint8_t turnYellow[6];
			uint8_t turnGreen[5];
			uint8_t turnRed[4];
		}; // max is 46

		struct RawData
		{
			RawData(const int type)
				:
				type(type)
			{
			}

			int type;
		};

		// ensure size to prevent breaking js
		static_assert(sizeof(RunningData) == 47, "Invalid RunningData size");
		static_assert(sizeof(CollectData) == 2, "Invalid CollectData size");
	}
}