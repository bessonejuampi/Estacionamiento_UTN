//Librerias
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>


//Configuracion red WiFi
#define WIFI_SSID "Utn_Libre Max" //Nombre de la red a conectarse
#define WIFI_PASSWORD "" //Contraseña de la red

//Configuracion base de datos Firebase
#define API_KEY "AIzaSyBU-fLHvQuzOFLs9jE-2RJPBJyysTBQtIY" //Clave API del proyecto
#define USER_EMAIL "francovalverde.fv@gmail.com" //Usuario administrador del proyecto
#define USER_PASSWORD "UtnIot2022" //Contraseña usuario
#define DATABASE_URL "https://esp8266-iot-utn-frsf-default-rtdb.firebaseio.com/" //Direccion de la base de datos del proyecto
#define DATABASE_SECRET "DATABASE_SECRET"
FirebaseData firebaseDataObject;
FirebaseAuth auth;
FirebaseConfig config;


//Variables del sensor
unsigned long dataMillis = 0;
int estadoAnterior = 0;
int estado = 0;
bool estaOcupado = false
const int pinTrigger = D4;  //Pin "trigger" del sensor de proximidad
const int pinEcho = D5;   //Pin "echo" del sensor de proximidad
const int pinLed = D2;

void setup()
{
  //Configuracion de los pines con el sensor de proximidad
  Serial.begin(115200);
  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEcho, INPUT);
  digitalWrite(pinTrigger, LOW);
  pinMode(pinLed, OUTPUT);

  //Conexion a internet
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Conexion con la base de datos
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  firebaseDataObject.setResponseSize(4096);
  String base_path = "/Nodo 1/";
  config.token_status_callback = tokenStatusCallback; 
  Firebase.begin(&config, &auth);
}

void loop()
{
  //Lectura del sensor
  if (millis() - dataMillis > 5000 && Firebase.ready())
  {
    dataMillis = millis();
    unsigned long tiempoRespuesta;
    float distancia;
    digitalWrite(pinTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinTrigger, LOW);
    tiempoRespuesta = pulseIn(pinEcho, HIGH);
    distancia = tiempoRespuesta * 0.000001 * 34300.0 / 2.0; //Obtenemos la distancia considerando que la señal recorre dos veces la distancia a medir y que la velocidad del sonido es 343m/s
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.print("cm");
    Serial.println();

    //Cambio del estado del estacionamiento
    switch (estado)
    {
      case 0:
        if (distancia < 30)
        {
          int i = 0;
          bool sePuedeGuardar = true;
          //prender led
          digitalWrite(pinLed, HIGH);          
          Serial.println("Se prende el led");          
          while (sePuedeGuardar && i < 5)
          {
            digitalWrite(pinTrigger, HIGH);
            delayMicroseconds(10);
            digitalWrite(pinTrigger, LOW);
            tiempoRespuesta = pulseIn(pinEcho, HIGH);
            distancia = tiempoRespuesta * 0.000001 * 34300.0 / 2.0;
            if (distancia > 30)
            {
              sePuedeGuardar = false;
              //apagar led;
              digitalWrite(pinLed, LOW);
              Serial.println("Se apaga el led");
            }
            i++;
            delay(2000);  
          }
          if (sePuedeGuardar)
          {
            estado = 1;
          }
        }
        break;
      case 1:
        if (distancia > 30)
        {
          digitalWrite(pinLed, LOW);
          estado = 0;
          delay(2000);
        }
        break;
    }

    //Actualizacion de la base de datos si cambia el estado del estacionamiento
    if(estadoAnterior != estado){
      estadoAnterior = estado;
      if(estado==0){
        estaOcupado = false  
      }else{
        estaOcupado = true
      }

      Serial.println("Se guarda el estado");
      String path = "/Nodos/Nodo 1/Actual";
      Serial.printf("Set int... %s\n", Firebase.RTDB.setBool(&firebaseDataObject, path, estaOcupado) ? "ok" : firebaseDataObject.errorReason().c_str());
      path = "/Nodos/Nodo 1/Historia";
      Serial.printf("Set int... %s\n", Firebase.RTDB.pushInt(&firebaseDataObject, path, estaOcupado) ? "ok" : firebaseDataObject.errorReason().c_str());
    }
  }
}
