 
  var Senz_type;
  JSON_adatok();

function JSON_adatok() {        // gombnyomásra is betölthetők az adatok
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/index_json", true);
  xhr.send();
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var response = xhr.responseText;
      adatok = JSON.parse(response);
      //console.log(JSON.stringify(adatok,null,'\t'));      // "szépen" kiírt JSON     

      document.getElementById("TIME").innerHTML = adatok.index[0];  // N_TIME_STRING
      document.getElementById("DATE").innerHTML = adatok.index[1];  // N_DATE_STRING   
      document.getElementById("DOW").innerHTML = adatok.index[2];   // N_DAYNAME      

      if (adatok.index[3] == 7001311) {                                 // Chip ID kültéri: 7001311
        document.getElementById("cim_color").style.backgroundColor = "lightgreen";
        document.getElementById("cim_text").innerHTML = "Kültéri hőmérséklet és páratartalom";
      }
      else if (adatok.index[3] == 13032562) {                           // Chip ID beltéri OLED: 13032562
        document.getElementById("cim_color").style.backgroundColor = "#fdfa84";
        document.getElementById("cim_text").innerHTML = "data logger (OLED)  -  " + Senz_type;
      }
      else if (adatok.index[3] == 13030075) {                           // Chip ID SPEJZ, BME280: 13030075
        document.getElementById("cim_color").style.backgroundColor = "orange";
        document.getElementById("cim_text").innerHTML = "SPEJZ  -  " + Senz_type;
      }
      else {
        document.getElementById("cim_color").style.backgroundColor = "orange";
        document.getElementById("cim_text").innerHTML = "Ismeretlen Chip! (" + adatok.index[3] + ")";
      }

      //------------------------------------------------
      if (adatok.index[4] == 1) {           // proc. újraindult    
        alarm = 1;
        document.getElementById("ALARM_1").style.display = "block";
        document.getElementById("ALARM_TEXT_1").innerHTML =
          "Processzor valami miatt újraindult!";
      }
      else {
        document.getElementById("ALARM_1").style.display = "none";
      }
    }
  }
} 

//****** JSON vége ********************************************************        

function restart_clear() {
  var r = confirm("Törlöd ezt a figyelmeztetést?");
  if (r == true) websocket.send(10);
  else console.log("Nem történt semmi!");
}

//  *** WebSocket ******************** WebSocket ***************************
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);
function initWebSocket() {
  //console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
  //console.log('Connection opened');
  websocket.send("Hello (WS)");
}

function onClose(event) {
  //console.log('Connection closed');
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

  if (rec_ID == 1) {
    document.getElementById("TIME").innerHTML = rec_text;  // N_TIME_STRING
  } 
//***************************************************
  if (rec_ID == 8) {
    var napok = rec_text.split("*");
    //console.log("napok: ", napok);
    document.getElementById("DATE").innerHTML = napok[0];
    document.getElementById("DOW").innerHTML = napok[1];
  }

  if(rec_ID == 3){
    document.getElementById("ALARM_1").style.display = "none"; }  // proc. újraindult   
}
