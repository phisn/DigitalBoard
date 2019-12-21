#pragma once

#define COMMUNICATION_WEBPART_VILLIANRUNNING R"__(
	var d;
	var type, playerCount, playerID, playerType, position, yellowTicketCount, greenTicketCount, redTicketCount, positionVillianReveal, blackTicketCount, doubleTicketCount, turnFerryCount, turnFerry, turnYellowCount, turnGreenCount, turnRedCount;
	var blur = false;

	function generate(d){
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
				var yellowDetTickets = new Array(d.getUint8(15), d.getUint8(20), d.getUint8(25));

				var html = '';
				html += '<h3>Currently on ' + position + '</h3>';
		html += '<button class="hid" onclick="hide()">HIDE POSITION</button>';
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
					html += '<label> <input name="doubleTicket" type="checkbox" onclick="generateButtons(this.checked)"> Use Double-Ticket </label>';
				}

				document.getElementById('playerdata').innerHTML = html;
				generateButtons(false);
				break;
			case 4:
				alert('ERROR 4');
				window.location = "/";
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
						var _html = '';
						for (var i = 0; i < turnYellowCount; i++) {
							var station = d.getUint8(i + 32);
							_html += '<button value=\"' + station + '\" type=\"submit\" name=\"' + dname + 'y\" formmethod="post">' + station + '</button>';
						}
						document.getElementById('taxi').innerHTML = _html;
					}
					if (turnGreenCount > 0 && greenTicketCount > 0) {
						var _html = '';
						for (var i = 0; i < turnGreenCount; i++) {
							var station = d.getUint8(i + 38);
							_html += '<button value=\"' + station + '\" type=\"submit\" name=\"' + dname + 'g\" formmethod="post">' + station + '</button>';
						}
						document.getElementById('bus').innerHTML = _html;
					}
					if (turnRedCount > 0 && redTicketCount > 0) {
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

	function hide(){
		var style = 'h3, button:not(.hid) {filter: blur(0px);}';
		if(!blur){
		style = 'h3, button:not(.hid) {filter: blur(10px);}';
		}
		var styleSheet = document.createElement("style");
		styleSheet.type = "text/css";
		styleSheet.innerText = style;
		document.head.appendChild(styleSheet);
		blur = !blur;
	}
</script>
<body> 
	<div class="container"> 
		<div id="playerdata">
		</div>
		<p>
			<b id="taxititle">
			</b>
		</p>
			<form class="taxi" id="taxi" action="" method="post"> 
			</form> 
		<p>
			<b id="bustitle">
			</b>
		</p>
		<form class="bus" id="bus" action="" method="post"> 
		</form> 
		<p>
			<b id="undergroundtitle">
			</b>
		</p>
		<form class="underground" id="underground" action="" method="post"> 
		</form> 
	</div>
</body>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const villianRunning = PROGMEM COMMUNICATION_WEBPART_VILLIANRUNNING;
		const size_t villianRunning_size = sizeof(COMMUNICATION_WEBPART_VILLIANRUNNING);
	}
}
