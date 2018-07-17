#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>

#include <ArduinoOTA.h>

#include "PushBullet.h"

//Constantes para configuración de WIFI SSID Y PASSWORD
static const char ssid[] = "SSID DE TU WIFI";
static const char password[] = "PASSWORD DE TU WIFI";


//Lista de correos para notificaciones
#define NUMERO_CORREOS_NOTIFICACION	1
String lista_correos_notificacion[]={"MAIL ASOCIADO A LA CUENTA DE PUSHBULLET"};

//MODIFICA LA IP INTERNA O EL DNS EXTERNO
//Definiciones para notificaciones
#define TITULO_NOTIFICACION	"BAJATECNOLOGIA"
#define CUERPO_NOTIFICACION_TIMBRE	"SUENA EL TIMBRE"
#define TIPO_NOTIFICACION "note"
#define TAM_MAXIMO_CUERPO_NOTIFICA 60

//Pines de E/S
#define TIMBREPIN 13 //Pin conectado al timbre

//Variables globales, gestión de interrupciones etc.
volatile byte interruptCounter = 0;

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
//Notifica a todos que ha sonado el timbre
void notifica_todos(byte evento)
{
  int i=0;
  
  for (i=0;i<NUMERO_CORREOS_NOTIFICACION;i++)
  {
    PushBullet(TITULO_NOTIFICACION,CUERPO_NOTIFICACION_TIMBRE,TIPO_NOTIFICACION,(char *)lista_correos_notificacion[i].c_str());
  }
  
  //Otros eventos se programarán más adelante.
	
}


//Función de configuración inicial
void setup() 
{
  IniciaWifi(158);
  #ifdef DEBUG
  Serial.begin(115200);//debug
  #endif  

  //Configuración de interrupción
  pinMode(TIMBREPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TIMBREPIN), handleInterrupt, FALLING);
 
  #ifdef DEBUG
  Serial.println("Iniciado y configurado.");
  #endif
  
  //Para programar "Over the Air"
  ArduinoOTA.begin();
  
}
 
//Función que maneja las interrupciones
void handleInterrupt() 
{
  interruptCounter++;
  
}


//Bucle principal
void loop() 
{
  static long last_interrupt=0;

  //Programación Over the Air
  ArduinoOTA.handle();
  
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
  
}


