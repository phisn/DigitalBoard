#include "WebsiteConstructor.h"

namespace
{
	FlashString webparts[(int) Communication::Webpart::_Length] =
	{
		FPSTR(""),
		FPSTR(""),
	};

	size_t webpartsSize[(int)Communication::Webpart::_Length] =
	{
		0,
		0
	};
}

namespace Communication
{
	namespace WebsiteConstructor
	{
		Webpart collect[] =
		{
			Webpart::Begin,
			Webpart::StyleLib,
			Webpart::StyleMisc,
			Webpart::Collect,
			Webpart::End
		};

		Webpart setup[] =
		{
			Webpart::Begin,
			Webpart::Setup,
			Webpart::StyleLib,
			Webpart::End
		};

		Webpart requestpid[] =
		{
			Webpart::Begin,
			Webpart::StyleLib,
			Webpart::StyleMisc,
			Webpart::Request,
			Webpart::End
		};

		Webpart runningdetective[] =
		{
			Webpart::Begin,
			Webpart::StyleLib,
			Webpart::StyleController,
			Webpart::CommonSocket,
			Webpart::DetectiveRunning,
			Webpart::End
		};

		Webpart runningvillian[] =
		{
			Webpart::Begin,
			Webpart::StyleLib,
			Webpart::StyleController,
			Webpart::CommonSocket,
			Webpart::VillianRunning,
			Webpart::End
		};

		Webpart finish[] =
		{
			Webpart::Begin,
			Webpart::StyleLib,
			Webpart::StyleMisc,
			Webpart::Finish,
			Webpart::End
		};

		size_t CalculateSize(Webpart* website);
		size_t CalculateSizeDynamic(WebpartCustom* custom, const int count);
		size_t CountCustomWebparts(Webpart* website);

		char* Construct(Webpart* website, WebpartCustom* dynamic)
		{
			// content size + custom size + '\0'
			char* const buffer = (char*)Device::MemoryManager::AllocateDynamic(
				CalculateSize(website) + CalculateSizeDynamic(dynamic, CountCustomWebparts(website)) + 1
			);

			size_t offset = 0;

			while (true)
			{
				if (*website == Webpart::Custom)
				{
					memcpy(buffer + offset,
						(const void*) dynamic->content,
						dynamic->size);
					offset += dynamic->size;

					++dynamic;
				}
				else
				{
					const size_t webpartSize = webpartsSize[(int)*website];
					const void* webpartContent = (const void*)webparts[(int)*website];

					memcpy_P(buffer + offset,
						webpartContent,
						webpartSize);
					offset += webpartSize;

					if (*website == Webpart::End)
					{
						break;
					}
				}

				++website;
			}

			buffer[offset] = '\0';
			return buffer;
		}

		size_t CalculateSize(Webpart* website)
		{
			size_t size = 0;

			while (true)
			{
				size += webpartsSize[(int) *website];

				if (*website == Webpart::End)
				{
					break;
				}

				++website;
			}

			return size;
		}

		size_t CalculateSizeDynamic(WebpartCustom* custom, const int count)
		{
			size_t size = 0;

			for (int i = 0; i < count; ++i, ++custom)
			{
				size += custom->size;
			}

			return size;
		}
		
		size_t CountCustomWebparts(Webpart* website)
		{
			size_t count = 0;

			while (*website != Webpart::End)
			{
				if (*website == Webpart::Custom)
				{
					++count;
				}

				website++;
			}

			return count;
		}
	}
}