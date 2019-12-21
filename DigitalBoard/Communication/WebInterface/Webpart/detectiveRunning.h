#pragma once

#define COMMUNICATION_WEBPART_DETECTIVERUNNING R"__(
	function generate(d) {
		var type = d.getUint8(0),
			playerID = d.getUint8(2),
			playerType = d.getUint8(3),
			playerCount = d.getUint8(1),
			position = d.getUint8(4),
			yellowTicketCount = d.getUint8(5),
			greenTicketCount = d.getUint8(6),
			redTicketCount = d.getUint8(7),
			positionVillianReveal = d.getUint8(8),
			positionVillian = d.getUint8(9),
			villianYellowTicketCount = d.getUint8(10),
			villianGreenTicketCount = d.getUint8(11),
			villianRedTicketCount = d.getUint8(12),
			villianBlackTicketCount = d.getUint8(13),
			villianDoubleTicketCount = d.getUint8(14),
			color = d.getUint8(15),
			turnYellowCount = d.getUint8(29),
			turnGreenCount = d.getUint8(30),
			turnRedCount = d.getUint8(31);
		switch(type){
			case 3:
		var html = '';
		var colorStr = '';
		switch (color) {
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
		if (pid === playerID) {
			if (turnYellowCount > 0 && yellowTicketCount > 0) {
				var tHtml = '';
				for (i = 0; i < turnYellowCount; i++) {
					var station = d.getUint8(31 + i);
					tHtml += '<button value="' + station + '" type="submit" name="y" formmethod="post">' + station + '</button>';
				}
				document.getElementById('taxi').innerHTML = tHtml;
			}
			if (turnGreenCount > 0 && greenTicketCount > 0) {
				var bHtml = '';
				for (i = 0; i < turnGreenCount; i++) {
					var station = d.getUint8(37 + i);
					bHtml += '<button value="' + station + '" type="submit" name="g" formmethod="post">' + station + '</button>';
				}
				document.getElementById('bus').innerHTML = bHtml;
			}
			if (turnRedCount > 0 && redTicketCount > 0) {
				var uHtml = '';
				for (i = 0; i < turnRedCount; i++) {
					var station = d.getUint8(42 + i);
					uHtml += '<button value="' + station + '" type="submit" name="r" formmethod="post">' + station + '</button>';
				}
				document.getElementById('underground').innerHTML = uHtml;
			}
		}
		case 4:
				alert('ERROR 4');
				window.location = "/";
				break;
		}
	}
</script>
<body>
  <div class="container">
    <div id="playerdata"></div>
    <div id="doubleticket"></div>
    <form class="taxi" id="taxi" action="" method="post"> </form>
    <form class="bus" id="bus" action="" method="post"> </form>
    <form class="underground" id="underground" action="" method="post"> </form>
  </div>
</body>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const detectiveRunning = PROGMEM COMMUNICATION_WEBPART_DETECTIVERUNNING;
		const size_t detectiveRunning_size = sizeof(COMMUNICATION_WEBPART_DETETCITVERUNNING);
	}
}
