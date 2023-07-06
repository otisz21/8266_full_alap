
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
    NTP_data = JSON.parse(xhr.responseText);
    //console.log(JSON.stringify(NTP_data,null,'\t'));      // "szépen" kiírt JSON    
    document.getElementById("TIME").innerHTML = NTP_data.data [0];
    document.getElementById("DATE").innerHTML = NTP_data.data [1];
    document.getElementById("DOW").innerHTML = NTP_data.data [2];
    document.getElementById("EPO_MOST").innerHTML = NTP_data.data [3];
    document.getElementById("Y_X_DAY").innerHTML = NTP_data.data [4];      
if (NTP_data.data [5] == 0) document.getElementById("ISDT").innerHTML = "téli ("+NTP_data.data [5]+") ";
else document.getElementById("ISDT").innerHTML = "nyári ("+NTP_data.data [5]+") ";
  document.getElementById("NTP_U").innerHTML = NTP_data.data [6];       
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

if (rec_ID == '{"') {
const WS = JSON.parse(event.data);
//console.log(JSON.stringify(WS, null, '\t'));      // "szépen" kiírt JSON   
document.getElementById("TIME").innerHTML = WS.data[0];  // N_TIME_STRING
}
//***************************************************
if (rec_ID == 8) {
var napok = rec_text.split("*");
//console.log("napok: ", napok);
document.getElementById("DATE").innerHTML = napok[0];
document.getElementById("DOW").innerHTML = napok[1];
}
}
