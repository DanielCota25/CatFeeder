  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include "time.h"
  #include "SPIFFS.h"
  #include "esp32-hal-ledc.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "Prostibulo_Suino";
const char* password = "404_not_found";

const char* PARAM_INPUT_1 = "tamanho";
const char* PARAM_INPUT_2 = "inicio";
const char* PARAM_INPUT_3 = "intervalo";

const int output = 2;
boolean state = LOW;

const int servPin = 33;
const int touchB = 13;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;
const int   daylightOffset_sec = 0;

int nivel = 80;
const char* automode = "{\"Automode\":[{ \"hora\": \"08:00\", \"tamanho\": \"M\", \"state\": \"checked\"},{ \"hora\": \"12:00\", \"tamanho\": \"M\", \"state\": \"\"},{ \"hora\": \"16:00\", \"tamanho\": \"M\", \"state\": \"checked\"},{ \"hora\": \"20:00\", \"tamanho\": \"M\", \"state\": \"\"},{ \"hora\": \"22:00\", \"tamanho\": \"G\", \"state\": \"checked\"}]}";



void acionarMotor(){
     for (int i=1638; i < 7864 ; i=i+100) {
      ledcWrite(1, i);
   }
   delay(1000);
   for (int i=7864; i > 1638 ; i=i-100) {
      ledcWrite(1, i);
}
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
   Serial.println("SPIFFS mounted successfully");
  }
}

IPAddress local_IP(10, 0, 0, 15);
// Set your Gateway IP address
IPAddress gateway(10, 0, 0, 2);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  Serial.println("STA Failed to configure");
}
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());

  ledcSetup(1, 50, 16);
  ledcAttachPin(servPin, 1);
  ledcWrite(1, 1638);
  pinMode(touchB, INPUT);
  
  initFS();
  
  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
    // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    //request->send_P(200, "text/html", String(nivel).c_str());
  });


  server.serveStatic("/", SPIFFS, "/");

  server.on("/manual", HTTP_GET, [] (AsyncWebServerRequest *request) {
    state = !state;
    String message;
        if (request->hasParam(PARAM_INPUT_1)) {
            String tamanho = request->getParam(PARAM_INPUT_1)->value();
            Serial.print(message);
        } else {
            message = "No message sent";
        }
    digitalWrite(output, state);
    acionarMotor();
    request->send(200, "text/plain", "ok");
  });


    server.on("/auto", HTTP_GET, [] (AsyncWebServerRequest *request) {
    state = !state;
    String message;
        if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
            String tamanho = request->getParam(PARAM_INPUT_1)->value();
            String autoStart = request->getParam(PARAM_INPUT_2)->value();
            // String autoStartIntervalo = request->getParam(PARAM_INPUT_3)->value();
            Serial.println(autoStart);
        } else {
            message = "No message sent";
        }
    digitalWrite(output, state);
    request->send(200, "text/plain", "ok");
  });

    server.on("/nivel", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",String(nivel).c_str());
    nivel = nivel - 5;
    });

    server.on("/pacote", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",automode);
    });
  
  server.onNotFound(notFound);
  server.begin();
  };

void loop() {
  if (digitalRead(touchB) == HIGH) {
  acionarMotor();
  }
}
