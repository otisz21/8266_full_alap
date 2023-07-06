
var WIFI_adatok;
var CONFIG_SSID;

      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/wifi_json", true);
      xhr.send();
      xhr.onreadystatechange = function() {
  if (this.readyState == 4 && this.status == 200) {
      WIFI_adatok = JSON.parse(xhr.responseText);
    //console.log(JSON.stringify(WIFI_adatok,null,'\t')); 
      document.getElementById("J_ssid").innerHTML = WIFI_adatok.ssid;
      document.getElementById("J_ip").innerHTML = WIFI_adatok.ip;
      document.getElementById("J_port").innerHTML = WIFI_adatok.port;
      document.getElementById("J_rssi").innerHTML = WIFI_adatok.rssi;       
      document.getElementById("J_mac").innerHTML = WIFI_adatok.wifi_mac;  
      document.getElementById("W_status").innerHTML = WIFI_adatok.W_status;
   
    if (WIFI_adatok.rssi < 0){  
        document.getElementById("J_wifi_mode").innerHTML = "Client mode";}
    else{  
        document.getElementById("J_wifi_mode").innerHTML = "Access Point";}                  
      }}                

//************************************************************************
function W_RESET (){                 
  var r = confirm("Az eszköz újraindul!\r"+
  "Rövid ideig nem lesz elérhető.\r"+
  "Ha a kapcsolódási adatok jók, újracsatlakozik.");   
  if (r == true) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/WIFI_save?x=10', true);
    xhr.send();}
  else {
    console.log("Nem történt semmi!");}}

