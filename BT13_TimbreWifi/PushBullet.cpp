#include <WiFiClientSecure.h>

#include <PushBullet.h>




void PushBullet(char *titulo, char *cuerpo, char *tipo, char *email)
{
  WiFiClientSecure client;
  char body[MAX_BODY];
  int longitud=0;
  
  sprintf(body,"{\"body\":\"%s\",\"title\":\"%s\",\"type\":\"%s\",\"email\":\"%s\"}",cuerpo, titulo, tipo, email);
  longitud=strlen(body);

  Serial.print(body);
  

  if (client.connect(PUSHBULLET_HOST, SECURE_PORT)) 
  {
  
    client.print
    (
      String("POST ") + PUSH_URL + " HTTP/1.1\r\n" +
      "Host: " + PUSHBULLET_HOST + "\r\n" +
      "User-Agent: ESP8266\r\n" +
      "Access-Token: " + ACCOUNT_TOKEN + "\r\n" +
      "Content-length: " + longitud + "\r\n"
      "Content-Type: application/json\r\n" +
      "Connection: close\r\n\r\n" + String(body)
    );
  }
 
}