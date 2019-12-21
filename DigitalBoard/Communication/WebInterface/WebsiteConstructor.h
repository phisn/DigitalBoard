#pragma once

#include "../../Common/Common.h"
#include "../../Device/MemoryManager.h"

namespace Communication
{
	enum class Webpart
	{
		Begin,

		CommonSocket,
		StyleLib,

		StyleMisc,
		StyleController,

		Collect,
		Setup,
		Request,
		Finish,

		DetectiveRunning,
		VillianRunning,

		End,
		_Length,

		// used to provide custom
		// and dynamic webparts
		Custom
	};

	struct WebpartCustom
	{
		const char* content;
		const size_t size;
	};

	namespace WebsiteConstructor
	{
		/*Webpart * collect,
				* setup,
				* requestpid,
				* runningdetective,
				* runningvillian,
				* finish;*/

		char* Construct(Webpart* website, WebpartCustom* dynamic = NULL);
	}
}
