#pragma once

namespace Communication
{
	namespace Website
	{
		const char* NeutralRequestPid_empty = PROGMEM
			R"__(
<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <title></title>
    </head>
    <style>
        div,
        h2 {
            text-align: center;
        }
    </style>
    <body>
        <div class="jumbotron">
            <h2>Request PlayerID</h2>
            <p>No Player available</p>
        </div>
    </body>
</html>)__";

		const char* NeutralRequestPid_prefix = PROGMEM
			R"__(
<!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="content-type" content="text/html; charset=UTF-8">
		<title></title>
	</head>
	<style>
		div,
		h2 {
			text-align: center;
		}
	</style>
	<body>
		<div class="jumbotron">
			<h2>Request PlayerID</h2>
			<select id="pid">)__";

		/*
			<option value="0">Black (0)</option>
			<option value="1">Red (1)</option>
			<option value="2">Green (2)</option>
			<option value="3">Blue (3)</option>
			<option value="4">Purple (4)</option>

		*/

		const char* NeutralRequestPid_sufix = PROGMEM
			R"__(
			</select>
			<input type="submit" value="Request" formmethod="post" onClick="setCookie()">
		</div>
	</body>
	<script language="javascript">
		function setCookie() {
			var pidHex = document.getElementById('pid').value;
			document.cookie = "pid=" + parseInt(pidHex, 16);
			window.location = "/";
		}
	</script>
</html>
)__";

		const size_t NeutralRequestPid_prefix_length = strlen_P(Website::NeutralRequestPid_prefix);
		const size_t NeutralRequestPid_sufix_length = strlen_P(Website::NeutralRequestPid_sufix);
	}

	namespace
	{
		struct EmptyPlayer
		{
			Game::PlayerId id;
			FlashString color;
		};

		size_t FindEmptyPlayers(EmptyPlayer* const emptyPlayerBuffer)
		{
			size_t emptyPlayerCounter = 0;
			
			const Game::PlayerData* const playerData = Game::SetupManager::GetData()->playerContext.data;
			for (int i = 0; i < Game::Collector::GetData()->playerCount; ++i)
			{
				const Game::PlayerId player = playerData[i].player;

				if (!Communication::InterfaceManager::ExistsInterface(player))
				{
					emptyPlayerBuffer[emptyPlayerCounter].id = player;
					emptyPlayerBuffer[emptyPlayerCounter].color = Game::ColorToFlash(playerData[i].color);

					++emptyPlayerCounter;
				}
			}

			return emptyPlayerCounter;
		}
	}

	namespace WebHandler
	{
		void HandleReqPidRequest(AsyncWebServerRequest* const request)
		{
			if (Game::Controller::GetState() == Game::GameState::Collect)
			{
				request->redirect(WEB_DIR_COMMON);
				return;
			}

			EmptyPlayer emptyPlayerBuffer[COMMON_MAX_PLAYERCOUNT];
			const size_t emptyPlayers = FindEmptyPlayers(emptyPlayerBuffer);

			if (emptyPlayerBuffer == 0)
			{
				DEBUG_MESSAGE("No empty players available");
				request->send_P(
					200, 
					WEB_RESPONSE_TYPE,
					Website::NeutralRequestPid_empty);

				return;
			}

			size_t responseSize = Website::NeutralRequestPid_prefix_length
				+ Website::NeutralRequestPid_sufix_length + 1; // + '\0'

			for (int i = 0; i < emptyPlayers; ++i)
			{
				responseSize += 7; // 2 hex + " - xx" 3 + value "xx"
				responseSize += strlen_P((const char*) emptyPlayerBuffer[i].color);
				responseSize += 26; // remaining select size
			}

			char* responseBuffer = new char[responseSize];
			size_t responsePosition = 0;

			memcpy_P(responseBuffer + responsePosition,
				Website::NeutralRequestPid_prefix, 
				Website::NeutralRequestPid_prefix_length);
			responsePosition += Website::NeutralRequestPid_prefix_length;

			const char html_option_0[] = "<option value=\"";
			const char html_option_1[] = "\">";
			const char html_option_2[] = "</option>";

			for (int i = 0; i < emptyPlayers; ++i)
			{
				const size_t html_option_0_size = sizeof(html_option_0) / sizeof(*html_option_0) - 1;
				const size_t html_option_1_size = sizeof(html_option_1) / sizeof(*html_option_1) - 1;
				const size_t html_option_2_size = sizeof(html_option_2) / sizeof(*html_option_2) - 1;

				char hexIdBuffer[9] = "00";
				itoa(emptyPlayerBuffer[0].id, hexIdBuffer, 16);
				
				if (hexIdBuffer[2] != '\0' && hexIdBuffer[2] != ' ')
				{
					DEBUG_MESSAGE("Got invalid number in itoa reqpid");
					hexIdBuffer[8] = '\0';
					DEBUG_MESSAGE(hexIdBuffer);
				}

				hexIdBuffer[2] = '\0';

				// html option 0
				memcpy_P(responseBuffer + responsePosition,
					html_option_0,
					html_option_0_size);
				responsePosition += html_option_0_size;

				// html option value
				memcpy(responseBuffer + responsePosition,
					hexIdBuffer,
					2);
				responsePosition += 2;

				// html option 1
				memcpy_P(responseBuffer + responsePosition,
					html_option_1,
					html_option_1_size);
				responsePosition += html_option_1_size;

				// html option color name
				const size_t colorLength = strlen_P((const char*) emptyPlayerBuffer[i].color);
				memcpy_P(responseBuffer + responsePosition,
					(const char*) emptyPlayerBuffer[i].color,
					colorLength);
				responsePosition += html_option_1_size;

				responseBuffer[responsePosition++] = ' ';
				responseBuffer[responsePosition++] = '-';
				responseBuffer[responsePosition++] = ' ';

				// html option hex info
				memcpy(responseBuffer + responsePosition,
					hexIdBuffer,
					2);
				responsePosition += 2;

				memcpy_P(responseBuffer + responsePosition,
					html_option_2,
					html_option_2_size);
				responsePosition += html_option_2_size;
			}

			memcpy_P(responseBuffer + responsePosition,
				Website::NeutralRequestPid_sufix,
				Website::NeutralRequestPid_sufix_length);
			responsePosition += Website::NeutralRequestPid_sufix_length;
			
			DEBUG_MESSAGE("GOT FINAL LENGTH:");
			DEBUG_MESSAGE(responsePosition);

			responseBuffer[responseSize - 1] = '\0';
			request->send(200, WEB_RESPONSE_TYPE, responseBuffer);
			delete[] responseBuffer;
		}
	}
}