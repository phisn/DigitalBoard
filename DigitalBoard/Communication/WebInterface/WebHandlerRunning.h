#pragma once

#include "../../Communication/InterfaceManager.h"

#include "../../Communication/WebInterface/WebServerManager.h"
#include "../../Communication/WebInterface/WebInterface.h"

#include "../../Game/GameController.h"

namespace
{
	using namespace Communication;
}

namespace Communication
{
	namespace Website
	{
		const char* RunningDetective PROGMEM =
			R"__(
<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="content-type" content="text/html; charset=UTF-8">
  <title>Digital Detectives</title>
 </head>
<body>
<div class="jumbotron">
  <div id="playerdata"></div>
  <p><b id="taxititle"></b></p>
  <form class="taxi" id="taxi" action="" method="post">
  </form>
 <p><b id="bustitle"></b></p>
  <form class="bus" id="bus" action="" method="post">
  </form>
 <p><b id="undergroundtitle"></b></p>
  <form class="underground" id="underground" action="" method="post">
 </form>
 </div>
</body>
<style>
 p {
  text-align: center;
}

div, h3 {
  text-align: center;
}

button {
	margin-right: 3px;
}

.black{
  background-color: black;
  padding: 20px;
  margin-bottom:10px;
}

.taxi {
  background-color: yellow;
  padding: 20px;
  margin-bottom:10px;
}

.bus {
  background-color: green;
  padding: 20px;
  margin-bottom:10px;
}

.underground {
  background-color: red;
  padding: 20px;
  margin-bottom:10px;
}

.ferry {
  background-color: blue;
  padding: 20px;
  margin-bottom:10px;
}
</style>
<script language="javascript">
    let cookie = document.cookie.replace(/(?:(?:^|.*;\s*)pid\s*\=\s*([^;]*).*$)|^.*$/, "$1");
	var pid = parseInt(cookie, 10);
	if (pid == 0 || isNaN(cookie)) {
		alert("Got invalid PlayerID, reconnecting soon");

		setTimeout(function () {
			location.reload();
		}, 300);
	}

	let socket = new WebSocket("ws://" + document.domain + "/ws?pid=" + cookie);
	socket.binaryType = "arraybuffer";
		
	socket.onmessage = function (event) {
		var d = new DataView(event.data);
    
		var type					= d.getUint8(0);

		switch (type)
		{
		case 3:
			break;
		
		default:
			alert("invalid type");
			location.reload();
 	    case 4:
			alert('ERROR 4');
			window.location = "/";
			return;
		case 5:
			window.location = "/requestpid";		
			return;
		}
			
		var playerID				= d.getUint8(2), 
			playerType				= d.getUint8(3), 
			playerCount				= d.getUint8(1), 
			position				= d.getUint8(4), 			
			yellowTicketCount		= d.getUint8(5), 
			greenTicketCount		= d.getUint8(6), 
			redTicketCount			= d.getUint8(7), 
			positionVillianReveal	= d.getUint8(8), 
			positionVillian			= d.getUint8(9), 
			villianYellowTicketCount = d.getUint8(10), 
			villianGreenTicketCount	= d.getUint8(11),
			villianRedTicketCount	= d.getUint8(12), 
			villianBlackTicketCount = d.getUint8(13), 
			villianDoubleTicketCount = d.getUint8(14), 
			color					= d.getUint8(15),
			turnYellowCount			= d.getUint8(29), 
			turnGreenCount			= d.getUint8(30), 
			turnRedCount 			= d.getUint8(31);
		var turnYellowOffset = 32,
			turnGreenOffset = 38,
			turnRedOffset = 43;
		
		if(type != 3){
		  location.reload(true);
		}

		var html = '';
		var colorStr = '';
		switch(color){
		  case 0:
		   colorStr = "Black";
		   break;
		   case 1:
		   colorStr = "Red"
		   break;
		   case 2:
		   colorStr = "Green";
		   break;
		   case 3:
		   colorStr = "Blue";
		   break;
		   case 4:
		   colorStr = "Purple";
		   break;
		   case 5:
		   colorStr = "Yellow";
		   break;
		}
		html += '<h3>Color: ' + colorStr + '</h3>';
		html += '<p><b>Tickets: </b>' + yellowTicketCount + ' Taxi, ' + greenTicketCount + ' Bus, ' + redTicketCount + ' Underground</p>';
		html += '\n';
		html += '<p><i><b>Tickets of Mr. X: </b> ' + villianBlackTicketCount + ' Black, ' + villianDoubleTicketCount + ' Double, ' + villianRedTicketCount + ' Underground, ' + villianGreenTicketCount + ' Bus, ' + villianYellowTicketCount + ' Taxi</i></p>';
		html += '\n';
		html += '<p>Position of Mr. X revealed in <b>' + positionVillianReveal + '</b> moves</p>';
		document.getElementById('playerdata').innerHTML = html;
 
		if(pid === playerID){
			if(turnYellowCount > 0 && yellowTicketCount > 0){
				document.getElementById('taxititle').innerHTML = 'Taxi';
				var tHtml = '';
				for(i = 0; i < turnYellowCount; i++){
					var station = d.getUint8(turnYellowOffset + i);
					tHtml += '<button value=\"' + station + '\" type=\"submit\" name=\"y\" formmethod="post">' + station + '</button>';
				}
				document.getElementById('taxi').innerHTML = tHtml;
			}
			if(turnGreenCount > 0 && greenTicketCount > 0){
				document.getElementById('bustitle').innerHTML = 'Bus';
				var bHtml = '';
				for(i = 0; i < turnGreenCount; i++){
				var station = d.getUint8(turnGreenOffset + i);
				bHtml += '<button value=\"' + station + '\" type=\"submit\" name=\"g\" formmethod="post">' + station + '</button>';
			}
			document.getElementById('bus').innerHTML = bHtml;
			}
			if(turnRedCount > 0 && redTicketCount > 0){
				document.getElementById('undergroundtitle').innerHTML = 'Underground';
				var uHtml = '';
				for(i = 0; i < turnRedCount; i++){
					var station = d.getUint8(turnRedOffset + i);
					uHtml += '<button value=\"' + station + '\" type=\"submit\" name=\"r\" formmethod="post">' + station + '</button>';
				}

				document.getElementById('underground').innerHTML = uHtml;
			}
		}
	}
</script>
</html>
)__";

		const char* RunningVillian PROGMEM =
			R"__(
<!DOCTYPE html>
<html>

<head>
  <meta http-equiv="content-type" content="text/html; charset=UTF-8">
  <title></title>
</head>

<body>
  <div class="jumbotron">
    <div id="playerdata"></div>
    <div id="doubleticket"></div>
    <p><b id="blacktitle"></b></p>
    <form class="black" id="black" action="" method="post">
    </form>
    <p><b id="taxititle"></b></p>
    <form class="taxi" id="taxi" action="" method="post">
    </form>
    <p><b id="bustitle"></b></p>
    <form class="bus" id="bus" action="" method="post">
    </form>
    <p><b id="undergroundtitle"></b></p>
    <form class="underground" id="underground" action="" method="post">
    </form>
  </div>
</body>
<style>
  p {
    text-align: center;
  }

  div,
  h3 {
    text-align: center;
  }

  button {
    margin-right: 3px;
  }

  .black {
    background-color: black;
    padding: 20px;
    margin-bottom: 10px;
  }

  .taxi {
    background-color: yellow;
    padding: 20px;
    margin-bottom: 10px;
  }

  .bus {
    background-color: green;
    padding: 20px;
    margin-bottom: 10px;
  }

  .underground {
    background-color: red;
    padding: 20px;
    margin-bottom: 10px;
  }

  .ferry {
    background-color: blue;
    padding: 20px;
    margin-bottom: 10px;
  }

</style>
<script language="javascript">
  var d;
  var type, playerCount, playerID, playerType, position, yellowTicketCount, greenTicketCount, redTicketCount, positionVillianReveal, blackTicketCount, doubleTicketCount, turnFerryCount, turnFerry, turnYellowCount, turnGreenCount, turnRedCount;

  let cookie = document.cookie.replace(/(?:(?:^|.*;\s*)pid\s*\=\s*([^;]*).*$)|^.*$/, "$1");
  var pid = parseInt(cookie, 10);
  if (pid == 0 || isNaN(cookie)) {
    alert("Got invalid PlayerID, reconnecting soon");

    setTimeout(function() {
      location.reload();
    }, 300);
  }

  let socket = new WebSocket("ws://" + document.domain + "/ws?pid=" + cookie);
  socket.binaryType = "arraybuffer";

  socket.onmessage = function(event) {
    d = new DataView(event.data);

    type = d.getUint8(0);

    switch (type) {
      case 3:
        playerCount = d.getUint8(1);
        playerID = d.getUint8(2);
        playerType = d.getUint8(3);
        position = d.getUint8(4);
        yellowTicketCount = d.getUint8(5);
        greenTicketCount = d.getUint8(6);
        redTicketCount = d.getUint8(7);
        positionVillianReveal = d.getUint8(8);
        blackTicketCount = d.getUint8(9);
        doubleTicketCount = d.getUint8(10);
        turnFerryCount = d.getUint8(26);
        turnFerry = d.getUint8(27);
        turnYellowCount = d.getUint8(29);
        turnGreenCount = d.getUint8(30);
        turnRedCount = d.getUint8(31);
        var redDetTickets = new Array(d.getUint8(11), d.getUint8(16), d.getUint8(21)); //YELLOW - GREEN - RED
        var greenDetTickets = new Array(d.getUint8(12), d.getUint8(17), d.getUint8(22));
        var blueDetTickets = new Array(d.getUint8(13), d.getUint8(18), d.getUint8(23));
        var purpleDetTickets = new Array(d.getUint8(14), d.getUint8(19), d.getUint8(24));
        var yellowDetTickets = new Uint8Array(d.getUint8(15), d.getUint8(20), d.getUint8(25));

        var html = '';
        html += '<h3>Currently on ' + position + '</h3>';
        html += '<p><b>Tickets: </b>' + blackTicketCount + ' Black, ' + doubleTicketCount + ' Double, ' + yellowTicketCount + ' Taxi, ' + greenTicketCount + ' Bus, ' + redTicketCount + ' Underground</p>'


        html += '<p><i><b>Tickets of RED: </b>' + redDetTickets[0] + ' Taxi, ' + redDetTickets[1] + ' Bus, ' + redDetTickets[2] + ' Underground</i></p>';
        html += '<p><i><b>Tickets of GREEN: </b>' + greenDetTickets[0] + ' Taxi, ' + greenDetTickets[1] + ' Bus, ' + greenDetTickets[2] + ' Underground</i></p>';
        html += '<p><i><b>Tickets of BLUE: </b>' + blueDetTickets[0] + ' Taxi, ' + blueDetTickets[1] + ' Bus, ' + blueDetTickets[2] + ' Underground</i></p>';

        if (playerCount > 4) {
          html += '<p><i><b>Tickets of PURPLE: </b>' + purpleDetTickets[0] + ' Taxi, ' + purpleDetTickets[1] + ' Bus, ' + purpleDetTickets[2] + ' Underground</i></p>';
        }
        if (playerCount > 5) {
          html += '<p><i><b>Tickets of YELLOW: </b>' + yellowDetTickets[0] + ' Taxi, ' + yellowDetTickets[1] + ' Bus, ' + yellowDetTickets[2] + ' Underground</i></p>';
        }

        html += '<p>Your position is revealed in <b>' + positionVillianReveal + '</b> moves</p>';

        if (doubleTicketCount > 0) {
          html += '<label> <input name="doubleTicket" type="checkbox" onclick="generateButtons(this.checked) "> Use Double-Ticket </label>';
        }

        document.getElementById('playerdata').innerHTML = html;
        generateButtons(false);
        break;
      case 4:
        alert('ERROR 4');
        window.location = "/";
        break;
      case 5:
		window.location = "/requestpid";		

		break;
    }
  }

  function generateButtons(useDoubleTicket) {
    switch (type) {
      case 3:
        if (pid === playerID) {
          var dname = '';
          if (useDoubleTicket) {
            dname = 'd'
          }

          var blackTurns = new Array(17);

          var gi = 0;

          for(var i = 0; i < turnFerryCount; i++){
            blackTurns[gi] = d.getUint8(i+27);
            gi++;
          }
          for(var i = 0; i < turnYellowCount; i++){
            blackTurns[gi] = d.getUint8(i+32);
            gi++;
          }
          for(var i = 0; i < turnGreenCount; i++){
            blackTurns[gi] = d.getUint8(i+38);
            gi++;
          }
          for(var i = 0; i < turnRedCount; i++){
            blackTurns[gi] = d.getUint8(i+43);
            gi++;
          }

          var _bT = new Array(17);
          for (var i = 0; i < blackTurns.length; i++) {
            var val = blackTurns[i];
            if (!_bT.includes(val)) {
              _bT[i] = val;
            }
          }

          if ((!_bT.every(i => i === 0))) {
            document.getElementById('blacktitle').innerHTML = 'Black';
            var _html = '';
            for (var i = 0; i < _bT.length; i++) {
              var station = _bT[i];
              if (typeof station != 'undefined') {
                _html += '<button value=\"' + station + '\" type=\"submit\" name=\"' + dname + 'b\" formmethod="post">' + station + '</button>';
              }
            }
            document.getElementById('black').innerHTML = _html;
          }
          if (turnYellowCount > 0 && yellowTicketCount > 0) {
            document.getElementById('taxititle').innerHTML = 'Taxi';
            var _html = '';
            for (var i = 0; i < turnYellowCount; i++) {
              var station = d.getUint8(i + 32);
              _html += '<button value=\"' + station + '\" type=\"submit\" name=\"' + dname + 'y\" formmethod="post">' + station + '</button>';
            }
            document.getElementById('taxi').innerHTML = _html;
          }
          if (turnGreenCount > 0 && greenTicketCount > 0) {
            document.getElementById('bustitle').innerHTML = 'Bus';
            var _html = '';
            for (var i = 0; i < turnGreenCount; i++) {
              var station = d.getUint8(i + 38);
              _html += '<button value=\"' + station + '\" type=\"submit\" name=\"' + dname + 'g\" formmethod="post">' + station + '</button>';
            }
            document.getElementById('bus').innerHTML = _html;
          }
          if (turnRedCount > 0 && redTicketCount > 0) {
            document.getElementById('undergroundtitle').innerHTML = 'Underground';
            var _html = '';
            for (var i = 0; i < turnRedCount; i++) {
              var station = d.getUint8(i + 43);
              _html += '<button value=\"' + station + '\" type=\"submit\" name=\"' + dname + 'r\" formmethod="post">' + station + '</button>';
            }
            document.getElementById('underground').innerHTML = _html;
          }
        }
        break;
      case 4:
        alert('ERROR 4');
        window.location = "/";
        break;
    }
  }

</script>

</html>

			)__";
	}

	namespace
	{
		Game::Ticket CharacterToTicket(const char character)
		{
			switch (character)
			{
			case 'y': return Game::Ticket::Yellow;
			case 'g': return Game::Ticket::Green;
			case 'r': return Game::Ticket::Red;
			case 'b': return Game::Ticket::Black;
			case 'd': return Game::Ticket::Double;
			default: return Game::Ticket::_Invalid;
			}
		}

		bool IsPlayerDetective(const Game::PlayerId playerId)
		{
			return Game::GameManager::ReadPlayer(playerId)->data->type == Game::PlayerData::Type::Detective;
		}

		FlashString fault_invalid_turn = DEVICE_FAULT_MESSAGE("Got invalid turn on POST         ");
	}

	namespace WebHandler
	{
		void HandleRunningRequest(AsyncWebServerRequest* const request)
		{
			DEBUG_MESSAGE("GET running");

			if (Game::Controller::GetState() != Game::GameState::Running)
			{
				request->redirect(WEB_DIR_COMMON);
				return;
			}

			const Game::PlayerId playerId = WebServerManager::EnforcePlayerId(request);

			if (playerId == 0)
			{
				request->redirect(WEB_DIR_REQPID);
				return;
			}

			if (!InterfaceManager::ExistsInterface(playerId))
			{
				InterfaceManager::CreateLinkedInterface<WebInterface>(playerId);
			}

			request->send_P(200, WEB_RESPONSE_TYPE,
				IsPlayerDetective(playerId)
				? Website::RunningDetective
				: Website::RunningVillian);
		}

		void HandleRunningPost(AsyncWebServerRequest* const request)
		{
			if (Game::Controller::GetState() != Game::GameState::Running)
			{
				request->redirect(WEB_DIR_COMMON);
				return;
			}

			const Game::PlayerId playerId = WebServerManager::EnforcePlayerId(request);

			if (playerId == 0)
			{
				request->redirect(WEB_DIR_REQPID);
				return;
			}

			request->redirect(WEB_DIR_RUNNING);
			AsyncWebParameter* const parameter = request->getParam(0);

			Game::Turn turn;

			turn.doubleTicket = false;
			turn.position = 0;

			turn.ticket = CharacterToTicket(parameter->name().charAt(0));

			if (turn.ticket == Game::Ticket::_Invalid)
			{
				return;
			}

			if (turn.ticket == Game::Ticket::Double)
			{
				turn.doubleTicket = true;

				if (parameter->name().length() != 2)
				{
					return;
				}

				turn.ticket = CharacterToTicket(parameter->name().charAt(1));

				if (turn.ticket == Game::Ticket::_Invalid)
				{
					return;
				}
			}

			for (int i = 0; i < parameter->value().length(); ++i)
			{
				turn.position *= 10;
				turn.position += parameter->value()[i] - '0';
			}

			if (turn.position < 1 || turn.position > 199)
			{
				return;
			}

			Game::GameManager::TurnResult result = Game::GameManager::MakeTurn(playerId, turn);
			if (result != Game::GameManager::TurnResult::Success)
			{
				Device::FaultHandler::Handle(
					{
						Device::FaultModule::WebInterface,
						2,
						fault_invalid_turn
					},
					false);
			}

			/*
				Serial.println("Args:");

				for (int i = 0; i < request->args(); ++i)
				{
					Serial.print(request->arg(i));
					Serial.print("=");
					Serial.println(request->argName(i));
				}

				Serial.println("Params:");
				for (int i = 0; i < request->args(); ++i)
				{
					AsyncWebParameter* const parameter = request->getParam(i);

					Serial.print(parameter->isFile());
					Serial.print(" ");
					Serial.print(parameter->isPost());
					Serial.print(" ");
					Serial.print(parameter->size());
					Serial.print(" ");
					Serial.print(parameter->name());
					Serial.print("=");
					Serial.println(parameter->value());
				}
			*/
		}
	}
}
