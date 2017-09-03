static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<link rel="stylesheet" href="mystyle.css">
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>BAJA TECNOLOGIA</title>
</head>
<body>
<a id="solToken" style="text-decoration:none;" class="boton_auto" href="tokenize">SOLICITAR TOKEN</a>
</body>
</html>
)rawliteral";

static const char PROGMEM CSS[] = R"rawliteral(

body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000;  }

.boton_auto
  {
    margin-top: 3px;
    -moz-box-shadow:inset 0px 1px 0px 0px #ffffff;
    -webkit-box-shadow:inset 0px 1px 0px 0px #ffffff;
    box-shadow:inset 0px 1px 0px 0px #ffffff;
    background:-webkit-gradient( linear, left top, left bottom, color-stop(0.05, #ededed), color-stop(1, #dfdfdf) );
    background:-moz-linear-gradient( center top, #ededed 5%, #dfdfdf 100% );
    filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#ededed', endColorstr='#dfdfdf');
    background-color:#ededed;
    -webkit-border-top-left-radius:6px;
    -moz-border-radius-topleft:6px;
    border-top-left-radius:6px;
    -webkit-border-top-right-radius:6px;
    -moz-border-radius-topright:6px;
    border-top-right-radius:6px;
    -webkit-border-bottom-right-radius:6px;
    -moz-border-radius-bottomright:6px;
    border-bottom-right-radius:6px;
    -webkit-border-bottom-left-radius:6px;
    -moz-border-radius-bottomleft:6px;
    border-bottom-left-radius:6px;
    text-indent:0;
    border:1px solid #dcdcdc;
    display:block;
    color:#777777;
    font-family:arial;
    font-size:15px;
    font-weight:bold;
    font-style:normal;
    height:50px;
    line-height:50px;
    width:100%
    text-decoration:none;
    text-align:center;
    text-shadow:1px 1px 0px #ffffff;
  }
  .boton_auto:active
  {
    position:relative;
    top:2px;
  } 
)rawliteral";


