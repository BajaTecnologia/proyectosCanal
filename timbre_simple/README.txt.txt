Par�metros a configurar en timbre_simple.ino:

Hay que modificar el SSID y password de la WIFI en las siguientes l�neas:

//Constantes para configuraci�n de WIFI SSID Y PASSWORD

static const char ssid[] = "SSID DE TU WIFI";

static const char password[] = "PASSWORD DE TU WIFI";

La IP fija que quieres asignar al dispositivo:
IniciaWifi(158); (Por defecto tendr� la ip 192.168.1.158).

Par�metros a configurar en Pushbullet.h:
S�lo el token asignado por la cuenta de Pushbullet (ACCOUNT_TOKEN)

#define PUSHBULLET_HOST "api.pushbullet.com"
#define SECURE_PORT		443
#define ACCOUNT_TOKEN	"TU TOKEN AQU�"
#define PUSH_URL		"/v2/pushes"
#define MAX_BODY		150
