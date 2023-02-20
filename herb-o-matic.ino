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

#define soilMoistPin A0
#define powerLedPin D5
#define errorLedPin D2
#define pumpPin D1

WiFiClient espClient;
PubSubClient client(espClient);

int averageSoilMoistness;
boolean pumpWasActive = false;

void successBlink(){
   digitalWrite(powerLedPin, LOW);
   delay(100);
   digitalWrite(powerLedPin, HIGH);
}

void errorBlink(int numOfBlinks){
  for (int i = 0; i < numOfBlinks; i++) {
    digitalWrite(errorLedPin, HIGH);
    delay(100);
    digitalWrite(errorLedPin, LOW);
    if(i != numOfBlinks - 1){
      delay(100);  
    }
  }
}

void handleError(char* errorText){
  Serial.print("Error: ");
  Serial.print(errorText);
  errorBlink(10);
}

void connectWifi(){
  Serial.println();
  Serial.print("Connecting to ");
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
    Serial.println("WiFi connected.");
    Serial.println("IP address: "+ WiFi.localIP().toString() + ".");
    successBlink();
  }
}

int measureSoilMoistPercentage(){
  return constrain(map(analogRead(soilMoistPin), 640, 230, 0, 100), 0, 100);  
}

void calculateAverageSoilMoistness(int numOfMeasurements, int timeBetweenMeasurementsInMS){
  Serial.println();
  Serial.println("Calculating avarage soil moist percentage.");

  int sumOfMeasurements = 0;

  for (int i = 0; i < numOfMeasurements; i++) {
    sumOfMeasurements += measureSoilMoistPercentage();
    if(i != numOfMeasurements - 1){
      delay(timeBetweenMeasurementsInMS);
    }
  }

  averageSoilMoistness = sumOfMeasurements / numOfMeasurements;

  Serial.println();
  Serial.println("Average soil moistness: " + String(averageSoilMoistness) + " %.");
  successBlink();
}

void pumpWater(int pumpTimeInS){
  Serial.println();
  Serial.println("Started pumping water.");
  digitalWrite(pumpPin,HIGH);
  delay(pumpTimeInS * 1000);
  Serial.println("Stopped pumping water");
  digitalWrite(pumpPin,LOW);
}

void decideToPump(){
  if(averageSoilMoistness < 30){
    pumpWater(60);
    pumpWasActive = true;
  }else{
    Serial.println();
    Serial.println("No need to pump water.");
  }
}

void publishData(){
  Serial.println();
  Serial.println("Connecting to MQTT server...");
  if (client.connect(clientId, mqtt_user, mqtt_pwd)) {
    Serial.println("MQTT connected");
    successBlink();
    
    char payload[40];
    sprintf(payload, "{\"soilMoistMoistness\": %i, \"pumpWasActive\": %s}", averageSoilMoistness, pumpWasActive ? "true" : "false");
    boolean published = client.publish(pub_topic, payload);
    delay(500);
    
    if(published){
    Serial.println();
    Serial.println("Planter data was published:");
    Serial.println(payload); 
    successBlink();
  } else{
    handleError("Failed to publish planter data");
  }
  } else {
    handleError("Failed to connect to MQTT server.");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(D0, WAKEUP_PULLUP);
  pinMode(pumpPin,OUTPUT);  
  pinMode(powerLedPin,OUTPUT);
  pinMode(errorLedPin,OUTPUT);
  delay(200);

  digitalWrite(powerLedPin,HIGH);
  
  connectWifi();
  client.setServer(mqtt_server, mqtt_port);
  
  calculateAverageSoilMoistness(5, 200);
  decideToPump();
  publishData();
 
  ESP.deepSleep(1 * 60 * 6e7);
}

void loop() {
}
