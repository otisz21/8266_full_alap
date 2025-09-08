
var xhr = new XMLHttpRequest();
xhr.open("GET", "/setup_json", true);
xhr.send();
xhr.onreadystatechange = function () {
  if (this.readyState == 4 && this.status == 200) {
    adatok = JSON.parse(xhr.responseText);
    //console.log(JSON.stringify(adatok,null,'\t'));      // "szépen" kiírt JSON
    //------------------------------------------------        
    document.getElementById("J_SV").innerHTML = adatok.setup[0];
    document.getElementById("J_ST").innerHTML = adatok.setup[1];
    document.getElementById("J_BOARD").innerHTML = adatok.setup[2];
    document.getElementById("J_SRUN").innerHTML = msToTime(adatok.setup[3]);
    document.getElementById("J_SRUN_ALL").innerHTML = percToTime(adatok.setup[4]);
    //------------------------------------------------
    if (adatok.setup[5] == 1) document.getElementById("MAKE_LOG").checked = true;
    else document.getElementById("MAKE_LOG").checked = false;
    //------------------------------------------------        
    if (adatok.setup[6] == 1) document.getElementById("S_DEBUG").checked = true;
    else document.getElementById("S_DEBUG").checked = false;
    //------------------------------------------------        
    if (adatok.setup[7] == 1) document.getElementById("BLUE_LED").checked = true;
    else document.getElementById("BLUE_LED").checked = false;
    //------------------------------------------------
    document.getElementById("STR_1").value = adatok.setup[8];
    document.getElementById("STR_2").value = adatok.setup[9];
    document.getElementById("STR_3").value = adatok.setup[10];
    document.getElementById("STR_4").value = adatok.setup[11];
    document.getElementById("PRIOR").value = adatok.setup[12];
    document.getElementById("DEVICE").value = adatok.setup[13];

    document.getElementById("GOMB_SEND").style.backgroundColor = "lightgray";
    document.getElementById("GOMB_SEND").style.color = "black";
  }
}     

//----------------------------------------------------------------------------- 
function msToTime(ms) {
  let yers = Math.floor(ms / 31536000000);                                  // évek száma
  let days = Math.floor(((ms % 31536000000) / 86400000));                   // napok száma
  let hours = Math.floor((((ms % 31536000000) % 86400000) / 3600000));      // teljes órák
  let minutes = Math.floor(((((ms % 31536000000) % 86400000) % 3600000) / 60000));  // percek
  let mps = ((ms - (yers * 31536000000) - (days * 86400000) - (hours * 3600000) - (minutes * 60000)) / 1000).toFixed(0);
  if (yers > 0) return yers + " év, " + days + " nap, " + hours + " óra, " + minutes + " perc";
  else if (days > 0) return days + " nap, " + hours + " óra, " + minutes + " perc";
  else if (hours > 0) return hours + " óra, " + minutes + " perc";
  else if (minutes > 0) return minutes + " perc, " + mps + " mp";
  else return mps + " mp";
}

//----------------------------------------------------------------------------- 
function percToTime(perc) {
   let ms = perc * 60 * 1000;
   let y = Math.floor(ms / 31536000000);                                  // évek száma
   let d = Math.floor(((ms % 31536000000) / 86400000));                   // napok száma
   let h = Math.floor((((ms % 31536000000) % 86400000) / 3600000));       // teljes órák
   let m = Math.floor(((((ms % 31536000000) % 86400000) % 3600000) / 60000));  // percek
  var yDisplay = y > 0 ? y + (y == 1 ? " év, " : " év, ") : "";
  var dDisplay = d > 0 ? d + (d == 1 ? " nap, " : " nap, ") : "";
  var hDisplay = h > 0 ? h + (h == 1 ? " óra, " : " óra, ") : "";
  var mDisplay = m > 0 ? m + (m == 1 ? " perc, " : " perc, ") : "";
  return yDisplay + dDisplay + hDisplay + mDisplay;
 }

//----------------------------------------------------------------------------- 
function valtozas() {
  document.getElementById("GOMB_SEND").style.backgroundColor = "red";
  document.getElementById("GOMB_SEND").style.color = "white";
}

//----------------------------------------------------------------------------- 
function valtozas_pr() {
  document.getElementById("GOMB_SEND_PR").style.backgroundColor = "red";
  document.getElementById("GOMB_SEND_PR").style.color = "white";
}
//----------------------------------------------------------------------------- 
function save() {
  if (document.getElementById("MAKE_LOG").checked == true) var make_log = 1;
  else var make_log = 0;

  if (document.getElementById("S_DEBUG").checked == true) var s_debug = 1;
  else var s_debug = 0;

  if (document.getElementById("BLUE_LED").checked == true) var blue_led = 1;
  else var blue_led = 0;

  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/setup_save?make_log=' + make_log + '&s_debug=' + s_debug + '&blue_led=' + blue_led, true);
  xhr.send();
  document.getElementById("GOMB_SEND").style.backgroundColor = "lightgray";
  document.getElementById("GOMB_SEND").style.color = "black";
}       
      
function save_pr() {
  var str_1 = document.getElementById("STR_1").value;
  var str_2 = document.getElementById("STR_2").value;
  var str_3 = document.getElementById("STR_3").value;
  var str_4 = document.getElementById("STR_4").value;
  var device = document.getElementById("DEVICE").value;
  var prior = document.getElementById("PRIOR").value;

  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/setup_save_pr?str_1=' + str_1 + '&str_2=' + str_2 + '&str_3=' + str_3
  + '&str_3=' + str_3 + '&str_4=' + str_4 + '&device=' + device + '&prior=' + prior, true);
  xhr.send();
  document.getElementById("GOMB_SEND_PR").style.backgroundColor = "lightgray";
  document.getElementById("GOMB_SEND_PR").style.color = "black";
}  

//------------------------------------------ 
function BUTTON(ez) {
  console.log("Press buttun: ", ez);
  if (ez == 3) {
    websocket.send(1280);
  }
  else if (ez == 4) {
    websocket.send("Hello");
  }    
  else {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/B_" + ez, true);
    xhr.send();
    xhr.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        var response = xhr.responseText;
        console.log(response);
      }
    }
  }
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
function onMessage(event) {
  //console.log("onMessage(event): ", event.data);
  var rec_ID = parseInt(event.data.substring(0, 2));
  var rec_text = event.data.substring(2);
  //console.log("rec_ID: ", rec_ID);
  //console.log("rec_text: ", rec_text);
}
