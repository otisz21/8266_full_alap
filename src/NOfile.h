
//<h1>A kérés nem teljesíthető!</h1>   // így nem jó, hiába UTF-8 -as a weboldal!
// A többi HTML oldal fájlként áthelyezve az SPIFFS (SPI Flash fájlrendszer) területre 
//******** HA a kérés nem teljesíthető, NOfile_html weboldal HTML kódja, programmemóriában tárolva   ***********
const char NOfile_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head> 
<title>Font Awesome Icons</title>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'> 
  <style>
  h1{color:blue;
   font-size: 1.5rem;
   box-sizing: border-box;
   width: 300px;
   padding: 20px;  
   border: 2px solid black;
   box-position: center;
   margin: 0 auto;
   text-align: center;}  
  p{color:#FC170C;
    font-size: 3.0rem;
    text-align: center;
    margin-top:2px;
    margin-bottom:2px;}
 .tab{display: inline-block;
    margin-left: 60px;}
  h2{text-align: center;
    font-size:25px;} 
  </style></head>
<body>
  <p><i class='fa fa-bug'></i><span class='tab'></span>
     <i class='fa fa-bug' style='color:black;'></i><span class='tab'>
     <i class='fa fa-bug'></i></p>
     <h2>- &nbsp 404 &nbsp - &nbsp 404 &nbsp - &nbsp 404 &nbsp -</h2>
  <h1>A k&eacute;r&eacute;s nem teljes&iacute;thet&#337;!</h1>
     <h2>- &nbsp 404 &nbsp - &nbsp 404 &nbsp - &nbsp 404 &nbsp -</h2>
  <p><i class='fa fa-bug'style='color:black;'></i><span class='tab'></span>
     <i class='fa fa-bug' ></i><span class='tab'>
     <i class='fa fa-bug'style='color:black;'></i></p>
</body>   
</html>)rawliteral";
