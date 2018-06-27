#include <LowPower.h>
#include <IRLib.h>
#include <EEPROM.h>

#define LED_PIN 7
#define IR_PIN  3
#define MOSFET_PIN  13
#define RPI_OFF_PIN 12
#define APRENDE_PIN 11
#define IR_POWER_PIN  9

#define EEPROM_ADDR 10
const int wakeUpPin = 3;
unsigned long val_ir=0;
unsigned long val_ir_actual=0;

long tiempo_encendido=0;
long ahora=0;
int estado=0;


//Create a receiver object to listen on pin 11
IRrecv Receptor(IR_PIN);

//Create a decoder object
IRdecode Decodificador;
// Use pin 2 as wake up pin

unsigned long lee_codigo_eeprom(void)
{
  unsigned long val=0;
  byte crc=0;
  unsigned long result=0;
  int i=0;
  
  for (i=0;i<4;i++)
  {
    val=EEPROM.read(EEPROM_ADDR+i);
    result=result+(val<<((3-i)*8));
    crc+=val;
  }
  val=EEPROM.read(EEPROM_ADDR+4);
  if(crc!=val)
  {
    result=-1;
  }
  return result;
}

unsigned long lee_codigo_eeprom(void)
{
  byte val=0;
  byte crc=0;
  unsigned long result=0;
  int i=0;
  
  for (i=0;i<4;i++)
  {
    val=EEPROM.read(EEPROM_ADDR+i);
    result=result+(val<<((3-i)*8));
    crc+=val;
  }
  val=EEPROM.read(EEPROM_ADDR+4);
  if(crc!=val)
  {
    result=-1;
  }
  return result;
}

void escribe_codigo_eeprom(unsigned long codigo)
{
  byte val=0;
  byte crc=0;
  
  for (i=0;i<4;i++)
  {
	val=codigo>>((3-i)*8)
    val=EEPROM.write(EEPROM_ADDR+i, val);
    crc+=val;
  }
  EEPROM.write(EEPROM_ADDR+4,crc);
}

unsigned long lee_infrarrojos(void)
{
  unsigned long cod=0;
  if (Receptor.GetResults(&Decodificador)) 
  {
    Decodificador.decode();  
    cod=Decodificador.value;
    Receptor.resume();     
  }
  return cod;
}
void aprender_codigo(void)
{
    long blink_time=0;
    
    bool codigo_valido=false;
    unsigned long codigo=0;
    bool esta_encendido=false;
    
    digitalWrite(LED_PIN,LOW);
    esta_encendido=true;
    blink_time=millis();
    do 
    {
      if (millis()-blink_time>=500)
      {
        blink_time=millis();
        digitalWrite(LED_PIN,!esta_encendido);
        esta_encendido=!esta_encendido;  
      }    
      codigo=lee_infrarrojos();
      if(codigo>0)
      {
          codigo_valido=true;
      }
      
    } while (!codigo_valido);
    //Decodificado, guardamos el valor en la EEPROM.
    escribe_codigo_eeprom(codigo);
    val_ir=codigo;
    digitalWrite(LED_PIN,LOW);
    delay(1000);
    digitalWrite(LED_PIN,HIGH);
    
}

void setup()
{
  pinMode(MOSFET_PIN,OUTPUT);
  digitalWrite(MOSFET_PIN,LOW);
  pinMode(RPI_OFF_PIN,INPUT);
  pinMode(APRENDE_PIN,INPUT);


  Receptor.enableIRIn(); // Start the receiver
  
  pinMode(IR_POWER_PIN, OUTPUT);
  digitalWrite(IR_POWER_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);//Led Apagado
  
  
  val_ir=lee_codigo_eeprom();

  if (val_ir==-1)
  {
    aprender_codigo();
    
  }
  
}
void wakeUp()
{
  val_ir_actual=lee_infrarrojos();
  ahora=millis();
  digitalWrite(LED_PIN,LOW);
}

void dormir(void)
{    
    attachInterrupt(digitalPinToInterrupt(wakeUpPin), wakeUp, LOW);
    LowPower.powerStandby(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    // Disable external pin interrupt on wake up pin.
    detachInterrupt(digitalPinToInterrupt(wakeUpPin));     
}

void parpadea(int veces)
{
  int i;
  
  for (i=0;i<veces;i++)
  {
    digitalWrite(LED_PIN,LOW);
    delay(200);
    digitalWrite(LED_PIN,HIGH);
    delay(200);
  }
  
}
void enciende_raspberry(void)
{
  digitalWrite(MOSFET_PIN,HIGH);
  delay(100);
  tiempo_encendido=millis();
  
}

void loop() 
{
  
  long  momento_encendido=0;
  
  //Después de la configuración inicial, dormimos
  
  //tiempo_despierto=millis();
  
  
  switch (estado)
  {
    
    case 0:
      //ahora=millis();
    
        dormir();       
        while((millis()-ahora<2000)&&(val_ir_actual!=val_ir))
        {
          val_ir_actual=lee_infrarrojos();
          if (val_ir_actual==val_ir)
          {
         //Encendemos la raspberry.
          estado=1;
          parpadea(10);

          enciende_raspberry();       
          }
        
        }      
        if (val_ir_actual!=val_ir)
        {
            digitalWrite(LED_PIN,HIGH);
        }
      
      
    break;
    case 1:
      //La raspberry estÃƒÂ¡ encendida. Debemos esperar la confirmaciÃƒÂ³n de arranque para pasar al estado 2.
      if (millis()-tiempo_encendido>30000)
      {
          enciende_raspberry();
          tiempo_encendido=millis();
      }
      if ((digitalRead(RPI_OFF_PIN)==HIGH))
      {
        
        estado=2;
        parpadea(3);
        digitalWrite(LED_PIN,LOW);
        
        
      }
    break;
    case 2:
      //La raspberry ha confirmado el arranque. Ahora debemos esperar un apagado o una orden para aprender infrarrojos.
	  
      if (digitalRead(RPI_OFF_PIN)==LOW)
      {
        digitalWrite(LED_PIN,HIGH);
        estado=0;
        delay(30000);//Esperamos a que el shutdown sea completo
        delay(30000);//Y otros treinta segundos por si lo que estamos haciendo es reiniciar
        digitalWrite(MOSFET_PIN,LOW);
        digitalWrite(LED_PIN,HIGH);
        digitalWrite(IR_POWER_PIN,HIGH); 
        
        
      }
      if (digitalRead(APRENDE_PIN)==HIGH)
      {
        aprender_codigo();
      }
    break; 

  }
}



