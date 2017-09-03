#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>
#include <PushBullet.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include <ArduinoOTA.h>


//De momento añadimos el webserver completo, sólo se manejará una página que abrirá la puerta desde el enlace cuando suene el timbre.
#include <ESP8266WebServer.h>
#include "sitio_web.h"

//Constantes para configuración de WIFI SSID Y PASSWORD
static const char ssid[] = "<SSID_DE_TU_WIFI>";
static const char password[] = "<PASSWORD_DE_TU_WIFI>";

//Constantes para el token de apertura desde la notificación del timbre
#define CADUCIDAD_TOKEN 20000

//Lista de correos para notificaciones
#define NUMERO_CORREOS_NOTIFICACION	2
String lista_correos_notificacion[]={"<CORREO1@gmail.com>","<CORREO2@gmail.com>"};

//Introduce aquí tus tokens
#define NUMERO_TOKENS_IDENTIFICACION 6
#define LONGITUD_MAXIMA_UID_NFC 7
const uint8_t tokens[NUMERO_TOKENS_IDENTIFICACION][LONGITUD_MAXIMA_UID_NFC]={
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  };

//MODIFICA LA IP INTERNA O EL DNS EXTERNO
//Definiciones para notificaciones
#define TITULO_NOTIFICACION	"BAJATECNOLOGIA"
#define CUERPO_NOTIFICACION_TIMBRE	"SUENA EL TIMBRE http://192.168.1.155/abre?token=%ld"
#define CUERPO_NOTIFICACION_APERTURA_NFC  "PUERTA ABIERTA POR TOKEN NFC"
#define CUERPO_NOTIFICACION_ORDEN_APERTURA "RECIBIDA ORDEN APERTURA PUERTA"
#define CUERPO_TOKEN_AISLADO  "TOKEN APERTURA http://192.168.1.155/abre?token=%ld"
#define TIPO_NOTIFICACION "note"
#define TAM_MAXIMO_CUERPO_NOTIFICA 60


//Definiciones de pines
//Bus SPI para lector RFID/NFC
#define PN532_SCK  (12)
#define PN532_MISO (14)
#define PN532_MOSI (2)
#define PN532_SS   (0)

//Pines de E/S
#define PUERTAPIN 15 //Pin que estará conectado al pestillo eléctrico.
#define TIMBREPIN 13 //Pin conectado al timbre

//Definiciones de timeouts etc.
#define TIEMPO_APERTURA_PUERTA 1000 //Tiempo que permanecerá accionado el pestillo eléctrico (ms)

#define USUARIO_WEB   "admin"
#define PASSWORD_WEB  "BAJATECNOLOGIA"


//Variables globales, gestión de interrupciones etc.
volatile byte interruptCounter = 0;
ESP8266WebServer server(80);

//Objeto NFC
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
bool nfcIniciado=false;

//Token aleatorio de apertura desde enlace web
long tokenAleatorio=0;
bool tokenUsado=true;
long tokenGenerado=0;

//Función que genera el token aleatorio
long genera_aleatorio(void)
{
  //La semilla se generó en el Setup
  tokenAleatorio = random(1000000, 9999999);
  tokenUsado=false;
  tokenGenerado=millis();
  return tokenAleatorio;
}

//Función que caduca el token
void caduca_token(void)
{
  if ((millis()-tokenGenerado)>CADUCIDAD_TOKEN)
  {
    //El token es antiguo, lo caducamos.
    tokenUsado=true;
  }
}

//Función que inicia la conexión WIFI
//Modificar para ajustar a la configuración presente
void IniciaWifi(byte ipLastByte)
{
  IPAddress ip(192, 168, 1, ipLastByte );  
  IPAddress gateway(192, 168, 1, 1); 
  IPAddress subnet(255, 255, 255, 0); 
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    delay(2000);
    ESP.restart();
  }
}

//Función que acciona el pestillo de la puerta
void abre_puerta(void)
{
  digitalWrite(PUERTAPIN,HIGH);
  delay(TIEMPO_APERTURA_PUERTA);
  digitalWrite(PUERTAPIN,LOW);
  //Notificamos apertura de puerta
  PushBullet(TITULO_NOTIFICACION, CUERPO_NOTIFICACION_ORDEN_APERTURA ,TIPO_NOTIFICACION,(char *)lista_correos_notificacion[0].c_str());
}

//Notifica a todos que ha sonado el timbre
void notifica_todos(byte evento)
{
  int i=0;
  char cuerpo_con_token[TAM_MAXIMO_CUERPO_NOTIFICA]="";

  sprintf(cuerpo_con_token,CUERPO_NOTIFICACION_TIMBRE,genera_aleatorio());
  
  
  //Si el evento es 1 es que suena el timbre.
  if (evento==1)
  {
    for (i=0;i<NUMERO_CORREOS_NOTIFICACION;i++)
    {
      PushBullet(TITULO_NOTIFICACION,cuerpo_con_token,TIPO_NOTIFICACION,(char *)lista_correos_notificacion[i].c_str());
    }
  }
	
  //Otros eventos se programarán más adelante.
	
}

//Envía un token de apertura solicitado por web
void token_apertura(void)
{
  int i=0;
  char cuerpo_con_token[TAM_MAXIMO_CUERPO_NOTIFICA]="";

  sprintf(cuerpo_con_token,CUERPO_TOKEN_AISLADO,genera_aleatorio());
  PushBullet(TITULO_NOTIFICACION,cuerpo_con_token,TIPO_NOTIFICACION,(char *)lista_correos_notificacion[0].c_str());
  
}

//Inicializa el lector de nfc
void inicializa_nfc(void)
{
  
  if (!nfcIniciado)
  {
    //Inicializamos el lector de NFC
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (versiondata) 
    { 
      // configure board to read RFID tags
      nfc.SAMConfig();
      nfcIniciado=true;
    }
  }
}

//Función que abre la puerta si se entra con un token de un solo uso.
void handleAperturaToken()
{
  bool abierto=false;
  char token_local[20]="";
 
  Serial.println(server.args());
  Serial.println(server.arg(0));
  sprintf(token_local,"%ld",tokenAleatorio);
  if (server.args()>0)
  {
    if (!strcmp(token_local,server.arg(0).c_str()))
    {
      if (!tokenUsado)
      {
        tokenUsado=true;
        abre_puerta();
        server.send(200, "text/plain", "Puerta Abierta");
        abierto=true;
      }
    }
  
  }
  if (!abierto)
  {
    server.send(200, "text/plain", "Token invalido");
  }
  
}

//Función que abre la puerta si se entra en modo autenticado a una página.
void handleGeneraToken()
{

  //Si no está autenticado, se solicita autenticación.
  if(!server.authenticate(USUARIO_WEB, PASSWORD_WEB))
  {
    return server.requestAuthentication();
  }
 
  server.send_P(200, "text/html", INDEX_HTML);
  
}

//Función que maneja el envío de token y muestra de nuevo el INDEX
void handleEnviaToken()
{
  //Si no está autenticado, se solicita autenticación.
  if(!server.authenticate(USUARIO_WEB, PASSWORD_WEB))
  {
    return server.requestAuthentication();
  }
  token_apertura();
  server.send_P(200, "text/html", INDEX_HTML);
  
}

//Función que envía el contenido de estilos CSS
void handleCSS(void)
{
  server.send_P(200, "text/css", CSS);
}

//Función de configuración inicial
void setup() 
{
  IniciaWifi(155);
  #ifdef DEBUG
  Serial.begin(115200);//debug
  #endif  

  //Pin accionador de la puerta en modo salida y en estado bajo
  pinMode(PUERTAPIN, OUTPUT);
  digitalWrite(PUERTAPIN, LOW);

  //Configuración de interrupción
  pinMode(TIMBREPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TIMBREPIN), handleInterrupt, FALLING);
 
  #ifdef DEBUG
  Serial.println("Iniciado y configurado.");
  #endif
  
  //Semilla para los números aleatorios del token de apertura
  randomSeed(analogRead(A0));


  //Iniciamos el servidor web para poder abrir la puerta remotamente
  server.on("/abre", handleAperturaToken);
  server.on("/", handleGeneraToken);
  server.on("/mystyle.css", handleCSS);
  server.on("/tokenize", handleEnviaToken);
  server.begin();
  
  //Para programar "Over the Air"
  ArduinoOTA.begin();
  
}
 
//Función que maneja las interrupciones
void handleInterrupt() 
{
  interruptCounter++;
  
}

//Compara dos identificadores, aunque el leído sea de 4 bytes, se almacena en un buffer de 7 completado con 0
bool compara_uids(const uint8_t *uid,const uint8_t *uid2 )
{
  int i=0;
  bool fallido=false;

  while ((i<(LONGITUD_MAXIMA_UID_NFC))&&(!fallido))
  {
    if (uid[i]!=uid2[i])
    {
      fallido=true;
    }
    i++;
  }

  return !fallido;
  
}

//Valida la tarjeta leída contra todo el inventario
bool valida_tarjeta(const uint8_t *uid,uint8_t longitud)
{
  int i=0;
  bool validado=false;

  while ((i<NUMERO_TOKENS_IDENTIFICACION)&&(!validado))
  {
    validado=compara_uids(uid,longitud,tokens[i]);
    i++;
  }
  
  return validado;
  
}


//Bucle principal
void loop() 
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer para almacenar la lectura de tarjeta.
  uint8_t uidLength=7;                        //Longitud del token leido

  static long last_reading=0;
  static long last_interrupt=0;

  //Programación Over the Air
  ArduinoOTA.handle();
  
  //Para el servidor web temporal 
  server.handleClient();
  
  //Inicializamos el nfc sólo se inicializará si no lo está ya.
  inicializa_nfc();

  //Caducamos un posible token generado
  caduca_token();
  
  //Si hay interrupción de timbre, y hace más de 3 segundos que sonó por última vez, lanzamos la notificación a todos.
  if (interruptCounter>0)
  {
    #ifdef DEBUG
    Serial.print("Recibida interrupción timbre ");
    #endif
    interruptCounter=0;  
    if (millis()-last_interrupt>3000)
    {
        notifica_todos(1);
        
    }
    last_interrupt=millis();
  }
  //Si hace más de un segundo que intentamos leer nfc por última vez, volvemos a intentarlo
  if (millis()-last_reading>1000)
  {
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength,500);
    last_reading=millis();

    if (success) 
    {
       if (valida_tarjeta(uid,uidLength))
       {
          abre_puerta(); 
          //Se envía notificación de apertura al master
          PushBullet(TITULO_NOTIFICACION,CUERPO_NOTIFICACION_APERTURA,TIPO_NOTIFICACION,(char *)lista_correos_notificacion[0].c_str());        
       }
    }
  }
}



