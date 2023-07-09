
var NTP_data;
    
get_JSON();
//-----------------------------------------------------------------------------
function sync() { 
get_JSON(); }

//******************************************************************* 
function get_JSON() {   
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/NTP_json", true); 
    xhr.send();
    xhr.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
    adatok = JSON.parse(xhr.responseText);
    //console.log(JSON.stringify(adatok,null,'\t'));      // "szépen" kiírt JSON    
    document.getElementById("TIME").innerHTML = adatok.ntp [0];
    document.getElementById("DATE").innerHTML = adatok.ntp [1];
    document.getElementById("DOW").innerHTML = adatok.ntp [2];
    document.getElementById("EPO_MOST").innerHTML = adatok.ntp [3];
    document.getElementById("Y_X_DAY").innerHTML = adatok.ntp [4];      
if (adatok.ntp [5] == 0) document.getElementById("ISDT").innerHTML = "téli ("+adatok.ntp [5]+") ";
else document.getElementById("ISDT").innerHTML = "nyári ("+adatok.ntp [5]+") ";
  document.getElementById("NTP_U").innerHTML = adatok.ntp [6];       
  }}   
  }

//  *** WebSocket ******************** WebSocket ***************************
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);
function initWebSocket() {
console.log('Trying to open a WebSocket connection...');
websocket = new WebSocket(gateway);
websocket.onopen = onOpen;
websocket.onclose = onClose;
websocket.onmessage = onMessage;
}

function onOpen(event) {
console.log('Connection opened');
websocket.send("Hello (WS)");
}

function onClose(event) {
console.log('Connection closed');
setTimeout(initWebSocket, 2000);
}

function onLoad(event) {
initWebSocket();
}

//***************************************************
function onMessage(event) {
  //console.log("onMessage(event): ", event.data);
  var rec_ID = parseInt(event.data.substring(0, 2));
  var rec_text = event.data.substring(2);
  //console.log("rec_ID: ", rec_ID);
  //console.log("rec_text: ", rec_text);

  if (rec_ID == 8) {
    var napok = rec_text.split("*");
    //console.log("napok: ", napok);
    document.getElementById("DATE").innerHTML = napok[0];
    document.getElementById("DOW").innerHTML = napok[1];
  }
}
