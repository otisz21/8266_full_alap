
var WIFI_adatok;
var CONFIG_SSID;
var data;

var xhr = new XMLHttpRequest();
xhr.open("GET", "/wifi_json", true);
xhr.send();
xhr.onreadystatechange = function () {
  if (this.readyState == 4 && this.status == 200) {
    data = JSON.parse(xhr.responseText);
    console.log(JSON.stringify(data, null, '\t'));

    // -- milyen mód van elmentve, mi látszódjon  ----
    if (data.wifi_alap[0] == "WIFI_STA") {                          // STA mód
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

    // if (WIFI_adatok.rssi < 0){  
    //     document.getElementById("WIFI_0").innerHTML = "Client mode";}
    // else{  
    //     document.getElementById("WIFI_0").innerHTML = "Access Point";} 

    JSON_refresh();
  }
}                

//************************************************************************
// ----------------------------------------------------
function JSON_refresh (){               
  document.getElementById("egysem_txt").style.display = "none";
  document.getElementById("_1").style.display = "none";
  document.getElementById("_2").style.display = "none";
  document.getElementById("_3").style.display = "none";
  document.getElementById("_4").style.display = "none";
  document.getElementById("_5").style.display = "none";    
  document.getElementById("_6").style.display = "none";
  document.getElementById("_7").style.display = "none";
  document.getElementById("_8").style.display = "none";        
  document.getElementById("w_db").innerHTML = data.wifi_scan[0];

  if (data.wifi_scan[0] < 1) {
    document.getElementById("egysem_txt").style.display = "block";
  }
  else {
    for (var i = 1; i < data.wifi_scan[0]+1; i++) {
      document.getElementById("_" + i).style.display = "block";
    }
  }
  
  document.getElementById("R_1").innerHTML = data.wifi_scan[1];
  document.getElementById("S_1").innerHTML = data.wifi_scan[2];   
  document.getElementById("R_2").innerHTML = data.wifi_scan[3];
  document.getElementById("S_2").innerHTML = data.wifi_scan[4];
  document.getElementById("R_3").innerHTML = data.wifi_scan[5];
  document.getElementById("S_3").innerHTML = data.wifi_scan[6];   
  document.getElementById("R_4").innerHTML = data.wifi_scan[7];
  document.getElementById("S_4").innerHTML = data.wifi_scan[8];
  document.getElementById("R_5").innerHTML = data.wifi_scan[9];
  document.getElementById("S_5").innerHTML = data.wifi_scan[10];
  document.getElementById("R_6").innerHTML = data.wifi_scan[11];
  document.getElementById("S_6").innerHTML = data.wifi_scan[12];
  document.getElementById("R_7").innerHTML = data.wifi_scan[13];
  document.getElementById("S_7").innerHTML = data.wifi_scan[14];
  document.getElementById("R_8").innerHTML = data.wifi_scan[15];
  document.getElementById("S_8").innerHTML = data.wifi_scan[16];        
  }   
  
  //----------------------------------------------------------------------------- 
  function SSID_1_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_1").innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }
  //----------------------------------------------------------------------------- 
  function SSID_2_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_2").innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }
  //----------------------------------------------------------------------------- 
  function SSID_3_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_3").innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }
  //----------------------------------------------------------------------------- 
  function SSID_4_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_4").innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }
  //----------------------------------------------------------------------------- 
  function SSID_5_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_5").innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }
  //----------------------------------------------------------------------------- 
  function SSID_6_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_6").innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }
  //----------------------------------------------------------------------------- 
  function SSID_7_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_7").innerHTML;
    document.getElementById("SAVE_BUTTON").style.backgroundColor = "red";
  }
  //----------------------------------------------------------------------------- 
  function SSID_8_TXT() {
    document.getElementById("ssid_data").value = document.getElementById("S_8").innerHTML;
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
  document.getElementById("S_1").innerHTML = "WAIT!";
  document.getElementById("w_db").innerHTML = "WAIT!";
  document.getElementById("rescan_B").style.backgroundColor = "yellow";
  document.getElementById("rescan_B").value = "WAIT!";} 
  
  //-------------------------------------------------------------------------
  
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
  var rec_ID_num = parseInt(rec_ID);
  //console.log("rec_ID: ", rec_ID);
  //console.log("rec_text: ", rec_text);

  //***************************************************
  if (rec_ID_num == 5) {    //WIFI-rescan
    data = JSON.parse(rec_text);
    document.getElementById("rescan_B").style.backgroundColor = "lightgreen";
    document.getElementById("rescan_B").value = "Wi-Fi Rescan";
    JSON_refresh();
    console.log(JSON.stringify(data, null, '\t'));      // "szépen" kiírt JSON    
  }

} 

