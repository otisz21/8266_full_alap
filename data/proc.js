var year = 0;
var day = 0;
var hour = 0;
var all = 0;
var SD_OK = 0; 

document.getElementById("BUSY_1").style.visibility = "hidden";

//---- JSON adatok --------------------------------------------------      
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/proc_json", true);
  xhr.send();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
        adatok = JSON.parse(xhr.responseText);
        console.log(JSON.stringify(adatok, null, '\t'));     // "szépen" kiírt JSON        

        document.getElementById("PROC_0").innerHTML = adatok.proc[0].toFixed(2); // Proc. fesz.
        document.getElementById("PROC_1").innerHTML =
            adatok.proc[1].toLocaleString('en-US').replaceAll(",", " ");   // chip ID
        document.getElementById("PROC_2").innerHTML = adatok.proc[2];            // Core verzió
        document.getElementById("PROC_3").innerHTML = adatok.proc[3];            // SDK-verzió
        document.getElementById("PROC_4").innerHTML = adatok.proc[4];            // CPU frekv. MHz-ben
        document.getElementById("PROC_5").innerHTML =                          // Flash chip size
            adatok.proc[5].toLocaleString('en-US').replaceAll(",", " ");
        document.getElementById("PROC_6").innerHTML =                          // Flash chip size
            adatok.proc[6].toLocaleString('en-US').replaceAll(",", " ");
        //--------------------------------------------          
        document.getElementById("PROC_7").innerHTML =
            adatok.proc[7].toLocaleString('en-US').replaceAll(",", " ");   // flash chip ID
        document.getElementById("PROC_8").innerHTML =                          // Flash chip size
            adatok.proc[8].toLocaleString('en-US').replaceAll(",", " ");
        document.getElementById("PROC_9").innerHTML = adatok.proc[9];            // FLASH chip frekv. MHz-ben
        //--------------------------------------------
        document.getElementById("PROC_10").innerHTML =
            adatok.proc[10].toLocaleString('en-US').replaceAll(",", " ");   // Free Heap, byte
        document.getElementById("PROC_11").innerHTML =
            adatok.proc[11].toLocaleString('en-US').replaceAll(",", " ");   // Heap, max. block size, byte
        if (adatok.proc[12] > 25) {                                               // HEAP töredezettség %-ban, ha nagyobb mint 25%
            document.getElementById("PROC_12").innerHTML = " &nbsp " + (adatok.proc[12]).toFixed(2) + " !!! &nbsp";
            document.getElementById("PROC_12").style.backgroundColor = "red";
            document.getElementById("PROC_12").style.color = "white";
        }
        else {                                                                  // HEAP töredezettség %-ban, ha kisebb mint 25%
            document.getElementById("PROC_12").innerHTML = " &nbsp " + (adatok.proc[12]).toFixed(2) + " &nbsp ";
            document.getElementById("PROC_12").style.backgroundColor = "lightgreen";
            document.getElementById("PROC_12").style.color = "black";
        }
        //--------------------------------------------
        document.getElementById("PROC_13").innerHTML = adatok.proc[13];          // Proc. újraindult, x-szer
        document.getElementById("X_SZER").value = adatok.proc[13];               //proc. újraindult
        document.getElementById("PROC_14").innerHTML = adatok.proc[14];          // reset oka
        //-----------------------------------------------------------------------------------------------------
        document.getElementById("PROC_15").innerHTML = adatok.proc[15];          // Filesystem tipusa
        document.getElementById("PROC_16").innerHTML =
            adatok.proc[16].toLocaleString('en-US').replaceAll(",", " ");   // fs total space
        document.getElementById("PROC_17").innerHTML =
            adatok.proc[17].toLocaleString('en-US').replaceAll(",", " ");   // fs used space
        if (adatok.proc[18] > 75) {                                           // fs used %   ha a foglaltság nagyobb mint 75 %
            document.getElementById("PROC_18").innerHTML = " &nbsp " + (adatok.proc[18]).toFixed(2) + " !!! &nbsp";
            document.getElementById("PROC_18").style.backgroundColor = "red";
            document.getElementById("PROC_18").style.color = "white";
        }
        else {                                                              // fs used %   ha a foglaltság kisebb mint 75 %
            document.getElementById("PROC_18").innerHTML = " &nbsp " + (adatok.proc[18]).toFixed(2) + " &nbsp ";
            document.getElementById("PROC_18").style.backgroundColor = "lightgreen";
            document.getElementById("PROC_18").style.color = "black";
        }
        //--------------------------------------------
        document.getElementById("RUN_ALL").value = adatok.proc[19];
        document.getElementById("RUN_YEAR").value = Math.floor(adatok.proc[19] * 60 / 31536000);
        document.getElementById("RUN_DAY").value = Math.floor(((adatok.proc[19] * 60 % 31536000) / 86400));
        document.getElementById("RUN_HOUR").value = Math.floor((((adatok.proc[19] * 60 % 31536000) % 86400) / 3600));
    }
    }
    
  document.getElementById("RUN_OWER_BUTTON").style.background = "gray";
  document.getElementById("X_SZER_BUTTON").style.background = "gray";

// -------------------------------------------------------------------------  
function valtozas_1() {       // ha az év, nap, óra mezőkbe írunk
    document.getElementById("RUN_OWER_BUTTON").style.background = "red";
    year = document.getElementById("RUN_YEAR").value;
    day = document.getElementById("RUN_DAY").value;
    hour = document.getElementById("RUN_HOUR").value;
    document.getElementById("RUN_ALL").value = (year * 525600) + (day * 1440) + (hour * 60);
}
// -------------------------------------------------------------------------  
function valtozas_2() {      // ha a "last_time_u_64:" mezőbe írunk
    document.getElementById("RUN_OWER_BUTTON").style.background = "red";
    all = document.getElementById("RUN_ALL").value;
    document.getElementById("RUN_YEAR").value = Math.trunc(all / 525600);
    document.getElementById("RUN_DAY").value = Math.trunc((all % 525600) / 1440);
    document.getElementById("RUN_HOUR").value = Math.trunc(((all % 525600) % 1440) / 60);
}

   // -------------------------------------------------------------------------  
function valtozas_3() {      // ha a "Proc újraindult:" mezőbe írunk
    document.getElementById("X_SZER_BUTTON").style.background = "red";
}

// -------------------------------------------------------------------------
function EE_CLEAR (){
  let text;    
  let pw = prompt("Ezzel minden elmentett adat törlődik!\r"+
  "Kivétel a teljes futásídő elmentett értéke. \r"+
  "\r"+
  "Valóban törlöd a memóriát?\r"+
  "Ehhez a funkcióhoz jelszó szükséges!\r",    
  "PW (évszám?)");            // fr. polgári forradalom?   
if (pw == 1789) { 
    document.getElementById("BUSY_1").style.visibility = "visible";     
    var xhr = new XMLHttpRequest();    
    xhr.open("GET", "/EE_clear" , true);
    xhr.send();}
 else if (pw == null) {}
 else{   
    alert(pw +"  - Jelszó helytelen!");}}

// -------------------------------------------------------------------------
function RUN_write (){
  var BIT_32 = document.getElementById("RUN_ALL").value;  //percek
  //let text;    
  let pw = prompt("Ezzel megváltoztatod, vagy törlöd\r"+
  "a teljes futásidő elmentett értékét.\r"+
  "\r"+
  "Valóban felülírod?\r"+
  "Ehhez a funkcióhoz jelszó szükséges!\r",
  "PW (évszám?)");            // fr. polgári forradalom?   
if (pw == 1789) {  
    document.getElementById("RUN_OWER_BUTTON").style.background = "gray";      
    var xhr = new XMLHttpRequest();    
    xhr.open('GET', '/ALL_RUN?data='+BIT_32+'' , true);
    xhr.send();}
 else if (pw == null) {}
 else{   
    alert(pw +"  - Jelszó helytelen!");}}

     // -------------------------------------------------------------------------
function XSZER_write (){
    var x_szer = document.getElementById("X_SZER").value;  //proc. újraindult
    //let text;    
    let pw = prompt("Ezzel megváltoztatod, vagy törlöd\r"+
    "a processor újraindulások elmentett értékét.\r"+
    "\r"+
    "Valóban felülírod?\r"+
    "Ehhez a funkcióhoz jelszó szükséges!\r",
    "PW (évszám?)");            // fr. polgári forradalom?   
  if (pw == 1789) {  
      document.getElementById("X_SZER_BUTTON").style.background = "gray";      
      var xhr = new XMLHttpRequest();    
      xhr.open('GET', '/X_SZER?data='+x_szer+'' , true);
      xhr.send();}
   else if (pw == null) {}
   else{   
      alert(pw +"  - Jelszó helytelen!");}}


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
if (rec_ID == 30) {    // EEPROM törlése vége  
   document.getElementById("BUSY_1").style.visibility = "hidden";}           
}