
var WIFI_adatok;
var CONFIG_SSID;
var data;

/* var json_proba = '{' +
'"wifi_alap": ['+
'"WIFI_STA",'+
'"DIGI-WiFi",'+
'"192.168.0.106",'+
'"8266_full_alap",'+
'80,'+
'-57,'+
'"C8:C9:A3:6A:D2:C1",'+
'"WL_CONNECTED",'+
'"full_alap_CONFIG"'+
'],'+
'"w_scan_db": ['+
'5,'+
'5'+
'],'+
'"wifi_scan": ['+
'-56,'+
'"DIGI-WiFi",'+
'-60,'+
'"ASUS",'+
'-85,'+
'"ASUS_RPT",'+
'-92,'+
'"c685d978",'+
'-92,'+
'"Telekom-773af0"'+
']'+
'}'; */

document.getElementById("SAVE_BUTTON").style.backgroundColor = "gray";

var xhr = new XMLHttpRequest();
xhr.open("GET", "/wifi_json", true);
xhr.send();
xhr.onreadystatechange = function () {
  if (this.readyState == 4 && this.status == 200) {
    data = JSON.parse(xhr.responseText);
    //data = JSON.parse(json_proba);
    console.log(JSON.stringify(data, null, '\t'));

    // -- milyen mód van elmentve, mi látszódjon  ----
    if (data.wifi_alap[0] === "WIFI_STA") {                          // STA mód
      document.getElementById("NO_WIFI").checked = false;
      document.getElementById("WIFI_VAN").style.display = "block";
      document.getElementById("NO_WIFI_INFO").style.display = "none";
      document.getElementById("WIFI_3").innerHTML = "http://" + data.wifi_alap[3] + ".local";      
    }
    else {                                                          // AP mód
      document.getElementById("NO_WIFI").checked = true;
      document.getElementById("WIFI_VAN").style.display = "none";
      document.getElementById("NO_WIFI_INFO").style.display = "block";
      document.getElementById("WIFI_3").innerHTML = "not available";
    }
    document.getElementById("WIFI_0").innerHTML = data.wifi_alap[0];
    document.getElementById("WIFI_1").innerHTML = data.wifi_alap[1];
    document.getElementById("WIFI_2").innerHTML = data.wifi_alap[2];
    
    document.getElementById("MDNS_txt").innerHTML = "http://" + data.wifi_alap[3] + ".local";
    document.getElementById("WIFI_4").innerHTML = data.wifi_alap[4];
    document.getElementById("WIFI_5").innerHTML = data.wifi_alap[5];
    document.getElementById("WIFI_6").innerHTML = data.wifi_alap[6];
    document.getElementById("WIFI_7").innerHTML = data.wifi_alap[7];

    document.getElementById("AP_SSID").innerHTML = data.wifi_alap[8];
    document.getElementById("ssid_data").value = data.wifi_alap[9];
    document.getElementById("pass_data").value = data.wifi_alap[10];    
    JSON_refresh();
  }
}                

// ---------------------------------------------------------------------------
function JSON_refresh() {
  for (var i = 1; i < 9; i++) {
    document.getElementById("_" + i).style.display = "none";
  }
  document.getElementById("egysem_txt").style.display = "none";
  if (data.w_scan_db[1] <= 8) {
    document.getElementById("w_db").innerHTML = data.w_scan_db[0];
  }
  else {
    document.getElementById("w_db").innerHTML = data.w_scan_db[1] + "/" + data.w_scan_db[0];
  }

  if (data.w_scan_db[0] < 1) {
    document.getElementById("egysem_txt").style.display = "block";
  }
  else {
    var R_data = 0;
    var S_data = 1;
    for (var i = 1; i < data.w_scan_db[0] + 1; i++) {
      document.getElementById("_" + i).style.display = "block";
      document.getElementById("R_" + i).innerHTML = data.wifi_scan[R_data];
      document.getElementById("S_" + i).innerHTML = data.wifi_scan[S_data];
      R_data = R_data + 2;
      S_data = S_data + 2;
    }
  }    
}   
  
//-----------------------------------------------------------------------------
function PW_visible() {
  pass_data.setAttribute("type", "text");
  setTimeout(PW_hidden, 1500);
}

//-----------------------------------------------------------------------------
function PW_hidden() {
  pass_data.setAttribute("type", "password");
}

  //----------------------------------------------------------------------------- 
  function SSID_TXT(ez) {
    document.getElementById("ssid_data").value = document.getElementById("S_"+ez).innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }

//----------------------------------------------------------------------------- 
function no_wifi() {
  if (document.getElementById("NO_WIFI").checked == true) {
    document.getElementById("WIFI_VAN").style.display = "none";
    document.getElementById("NO_WIFI_INFO").style.display = "block";
  }
  else {
    document.getElementById("WIFI_VAN").style.display = "block";
    document.getElementById("NO_WIFI_INFO").style.display = "none";
  }
  document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
}

function valtozas_ssid() {
  document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
}
  //----------------------------------------------------------------------------- 
  function SAVE_WIFI_DATA (){
  var wifi_mode;
  if(document.getElementById("NO_WIFI").checked == true){   // AP-mód
    wifi_mode = 0;}                                         // wifi hálózat, 0->AP-mód
  else{                                                     // STA-mód
    wifi_mode = 1;}                                         // wifi hálózat, 1->STA mód(client)
  var ssid = document.getElementById("ssid_data").value;  
  var pasw = document.getElementById("pass_data").value;   
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/W_data?wifi_mode="+wifi_mode+"&ssid="+ssid+"&pasw="+pasw, true);
  xhr.send();}
  
  //----------------------------------------------------------------------------- 
  function ONLY_RESET (){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/wifi_reset", true);
  xhr.send();}  
  
  //-----------------------------------------------------------------------------    
  function rescan (){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/wifi_rescan", true);
  xhr.send();
  for (var i = 1; i < 9; i++) {
    document.getElementById("_" + i).style.display = "none";
  }  
  //document.getElementById("S_1").innerHTML = "WAIT!";
  document.getElementById("w_db").innerHTML = "WAIT!";
  document.getElementById("rescan_B").style.backgroundColor = "yellow";
  document.getElementById("rescan_B").value = "WAIT!";} 
  
  //-------------------------------------------------------------------------
  
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
  var rec_ID_num = parseInt(rec_ID);
  //console.log("rec_ID: ", rec_ID);
  //console.log("rec_text: ", rec_text);

  //***************************************************
  if (rec_ID_num == 5) {    //WIFI-rescan
    data = JSON.parse(rec_text);
    document.getElementById("rescan_B").style.backgroundColor = "lightgreen";
    document.getElementById("rescan_B").value = "Wi-Fi Rescan";
    JSON_refresh();
    //console.log(JSON.stringify(data, null, '\t'));      // "szépen" kiírt JSON    
  }

} 

