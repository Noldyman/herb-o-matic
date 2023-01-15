#include "credentials.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = CRED_SSID;
const char* password = CRED_PASSWORD;
const char* mqtt_server = CRED_MQTT_SERVER;
const int mqtt_port = CRED_MQTT_PORT;
const char* mqtt_user = CRED_MQTT_USER;
const char* mqtt_pwd = CRED_MQTT_PASSWD;
const char* clientId = CRED_CLIENT_ID;
const char* pub_topic = CRED_PUB_TOPIC;

#define SOILMOISTPIN A0

uint32_t averageSoilMoistValue;

void connectWifi(){
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  int count = 0 ;
  while (WiFi.status() != WL_CONNECTED && count <= 15) {
    delay(1000);
    Serial.print(".");
    count++;
  }
  
  if(WiFi.status() != WL_CONNECTED){
    handleError("Failed to connect to Wifi.");
  } else{
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();  
  }
}

uint32_t measureSoilMoistPercentage(){
  return constrain(map(analogRead(SOILMOISTPIN), 640, 300, 0, 100), 0, 100);  
}

void calculateAverageSoilMoistPercentage(){
  uint32_t value1 = measureSoilMoistPercentage();
  delay(1000);
  uint32_t value2 = measureSoilMoistPercentage();
  delay(1000);
  uint32_t value3 = measureSoilMoistPercentage();
  averageSoilMoistValue = (value1+value2+value3)/3;
}

void pumpWater(uint32_t pumpTime){
  Serial.println("Started pumping water.");
  digitalWrite(D1,HIGH);
  delay(pumpTime);
  Serial.println("Stopped pumping water");
  digitalWrite(D1,LOW);

}

void handleError(char* errorText){
  Serial.println();
  Serial.println("Error: ");
  Serial.print(errorText);
}

void setup() {
  Serial.begin(9600);
  pinMode(D1,OUTPUT);
  delay(200);
  
  connectWifi();
  calculateAverageSoilMoistPercentage();
  Serial.println("Average soil moistness: ");
  Serial.print(averageSoilMoistValue);

  // pumpWater(5000);
}

void loop() {
}
