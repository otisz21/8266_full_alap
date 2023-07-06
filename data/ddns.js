
var DDNS_adatok;
var WAN_new_IP;

/* példa:

http:laci2021:OTTwebddns2021@dynupdate.no-ip.com/nic/update?hostname=kert.ddns.net,ora01.ddns.net,irsend.ddns.net,relaytft.ddns.net,cirko.ddns.net&myip=94.21.100.136

document.getElementById("OUTPUT").innerHTML = ""+
                                "good 94.21.100.136\n"+
                                "good 94.21.100.136\n"+
                                "good 94.21.100.136\n"+
                                "good 94.21.100.136\n"+
                                "good 94.21.100.136"
*/                                  
// ----------------------------------------------------------------------      

/* var pr_json = '{"data": ['+
      '1,'+
      '"kert.ddns.net",'+
      '240,'+
      '"100.100.100.100",'+
      '"176.241.49.130",'+
      '"2023-01-23  |  14:03:25",'+
      '"WAIT",'+
      '"2023-01-23  |  14:03:25",'+
      '"200"'+
    ']}';
//console.log("pr_json: ", pr_json);
*/

  get_JSON();
//-----------------------------------------------------------------------------
  function get_JSON() {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/ddns_json", true);
        xhr.send();
        xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
                  
        DDNS_adatok = JSON.parse(xhr.responseText);
        
        // DDNS_adatok = JSON.parse(pr_json);
        
      //console.log(JSON.stringify(DDNS_adatok,null,'\t'));      // "szépen" kiírt JSON
      
      if (DDNS_adatok.data[0] == 1) document.getElementById("DDNS_ONOFF").checked = true;
      else document.getElementById("DDNS_ONOFF").checked = false;
       
      document.getElementById("DDNS_DOMAIN").value = DDNS_adatok.data[1];
      document.getElementById("DDNS_UPDATE").value = DDNS_adatok.data[2];        
      document.getElementById("WAN_IP_OLD").innerHTML = DDNS_adatok.data[3];
      
      WAN_new_IP = DDNS_adatok.data[4];
      document.getElementById("WAN_IP_NEW").innerHTML = WAN_new_IP;
      
      document.getElementById("LAST_UDP").innerHTML = DDNS_adatok.data[5];   // noip frissítés ideje             
      document.getElementById("OUTPUT").innerHTML = DDNS_adatok.data[6];     // noip_answer string        
      document.getElementById("WAN_T").innerHTML = DDNS_adatok.data[7];      // WAN IP check time
    if(DDNS_adatok.data[8] == 200){  
      document.getElementById("WAN_S").innerHTML = "&nbsp&nbsp"+DDNS_adatok.data[8]+"&nbsp&nbsp";  // WAN IP check status
      document.getElementById("WAN_S").style.backgroundColor = "lightgreen";
      document.getElementById("WAN_S").style.color = "black";}
    else{  
      document.getElementById("WAN_S").innerHTML = "&nbsp&nbsp"+DDNS_adatok.data[8]+"&nbsp&nbsp";     // WAN IP check status
      document.getElementById("WAN_S").style.backgroundColor = "red";
      document.getElementById("WAN_S").style.color = "white";}        
      KI_BE();
      }}
      } 

// ----------------------------------------------------------------------            
  function KI_BE(){
  if (document.getElementById("DDNS_ONOFF").checked == true){  // ha be van kapcsolva
      document.getElementById("kikapcs").innerHTML = "";
      document.getElementById("osszes").style.opacity = "1";
      document.getElementById("osszes").disabled = false;}
   else{                                                         // ha ki van kapcsolva
      document.getElementById("kikapcs").innerHTML = " A szolgáltatás ki van kapcsolva!";
      document.getElementById("osszes").style.opacity = "0.3";
      document.getElementById("osszes").disabled = true;}}
      
// ----------------------------------------------------------------------    
  function check_WAN_IP(){
      var xhr = new XMLHttpRequest();
      xhr.open('GET', '/check_WAN_IP', true);
      xhr.send();}

// ----------------------------------------------------------------------    
  function NO_IP_update(){
  var r = confirm("Valóban elküldöd a NO-IP-nek\r"+
  "a friisítési url-t?\r"+
  "Erre az IP-re frissít: "+WAN_new_IP);
  WAN_new_IP
  if (r == true) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', '/NO_IP_update', true); 
      xhr.send();}     
  else {
    console.log("Nem történt frissítés!");}}

// ----------------------------------------------------------------------    
  function DDNS_save(){
      if (document.getElementById("DDNS_ONOFF").checked == true){
          var DDNS_ON_OFF = 1;}
      else{
          var DDNS_ON_OFF = 0;}            
      var DDNS_DOMAIN = document.getElementById("DDNS_DOMAIN").value;           
      var DDNS_UPD = document.getElementById("DDNS_UPDATE").value;          
      var xhr = new XMLHttpRequest();
      xhr.open('GET', '/ddns_save?D_1='+DDNS_ON_OFF+'&D_2='+DDNS_UPD+
                                  '&D_3='+DDNS_DOMAIN+' ', true);
      xhr.send();        
      document.getElementById("DDNS_SAVE").style.backgroundColor = "#C5C5C5"; }  

// ----------------------------------------------------------------------   
  function valtozas () {
      document.getElementById("DDNS_SAVE").style.backgroundColor = "#ff0000";}  


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
var rec_ID = event.data.substring(0, 2);
var rec_text = event.data.substring(2);
//console.log("rec_ID: ", rec_ID);
//console.log("rec_text: ", rec_text);
//***************************************************
  //"21"+WAN_T);              // IP cím lekérdezés ideje
  //"22"+WAN_S);              // IP cím lekérdezés válasza http kódja
  //"23"+LAST_IP_CHANGED);    // NO-IP lekérdezés ideje
  //"24"+noip_answer);        // NO-IP válasz     
if (rec_ID == 21) {
  document.getElementById("WAN_T").innerHTML = rec_text;      // WAN IP check time
}

if (rec_ID == 22) {
  if(rec_text == 200){  
  document.getElementById("WAN_S").innerHTML = "&nbsp&nbsp"+rec_text+"&nbsp&nbsp";  // WAN IP check status
  document.getElementById("WAN_S").style.backgroundColor = "lightgreen";
  document.getElementById("WAN_S").style.color = "black";}
else{  
  document.getElementById("WAN_S").innerHTML = "&nbsp&nbsp"+rec_text+"&nbsp&nbsp";   // WAN IP check status
  document.getElementById("WAN_S").style.backgroundColor = "red";
  document.getElementById("WAN_S").style.color = "white";}   
}

if (rec_ID == 23) {
  document.getElementById("LAST_UDP").innerHTML = rec_text;   // noip frissítés ideje
}

if (rec_ID == 24) {
  document.getElementById("OUTPUT").innerHTML = rec_text;   // noip válasza
}  
}
