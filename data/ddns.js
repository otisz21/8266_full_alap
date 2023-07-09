
//-----------------------------------------------------------------------------
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/ddns_json", true);
  xhr.send();
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      adatok = JSON.parse(xhr.responseText);
      console.log(JSON.stringify(adatok, null, '\t'));      // "szépen" kiírt JSON

      document.getElementById("TIME").innerHTML = adatok.ddns[0];
      document.getElementById("DATE").innerHTML = adatok.ddns[1];
      document.getElementById("DOW").innerHTML = adatok.ddns[2];
    }
  } 
      
// ----------------------------------------------------------------------    
function check_WAN_IP() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/check_WAN_IP', true);
  xhr.send();
}

// ----------------------------------------------------------------------    
function DDNS_save() {
  document.getElementById("DDNS_SAVE").style.backgroundColor = "#C5C5C5";
}

// ----------------------------------------------------------------------   
function valtozas() {
  document.getElementById("DDNS_SAVE").style.backgroundColor = "#ff0000";
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
    //***************************************************
function onMessage(event) {
  //console.log("onMessage(event): ", event.data);
  var rec_ID = parseInt(event.data.substring(0, 2));
  var rec_text = event.data.substring(2);
  //console.log("rec_ID: ", rec_ID);
  //console.log("rec_text: ", rec_text);
  //***************************************************   
  if (rec_ID == 1) {
    document.getElementById("TIME").innerHTML = rec_text;
  }

  if (rec_ID == 8) {
    var napok = rec_text.split("*");
    //console.log("napok: ", napok);
    document.getElementById("DATE").innerHTML = napok[0];
    document.getElementById("DOW").innerHTML = napok[1];
  }
}
