#include <WiFi.h>       
#include <DHT.h>
#include <PubSubClient.h>
 
#define DHTPIN 10 // Digital pin connected to the DHT sensor
#define LED 11
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "TOPNET_2670";
const char* password = "7yutleq9gu";
 
unsigned long previousMillis = 0;
unsigned long interval = 10000;
 
const char* mqttServer = "192.168.1.14";
const int mqttPort = 1883;
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
    Serial.println();
   
  String message = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message+=(char)payload[i];
  }
  Serial.println("-----------------------");
  if(String(topic)=="LED"){
    if(message=="LED ON"){
      digitalWrite(LED,HIGH);
      Serial.println("LED IS ON");
    }
    else{
      digitalWrite(LED,LOW);
    }
  }
     
}
 
WiFiClient espClient;
PubSubClient client(mqttServer, mqttPort, callback, espClient);

 void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP32Client")) {
      Serial.println("connected");  
      client.subscribe("LED");
      client.subscribe("data");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(15000);
    }
  }
}
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  setup_wifi();
  dht.begin();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

}
 
void loop() {
      if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())

    client.connect("ESP32Client");
unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if(!isnan(h)&&!isnan(t)){
      Serial.println("Temperature : " +String(t));
      Serial.println("Humidity : " +String(h));
      String toSend = String(t) + "," +String(h);
      client.publish("data",toSend.c_str());
    }
    else {
      Serial.println("Temperature : no_result");
      Serial.println("Humidity :  no_result");
      String toSend =  "no_result";
      client.publish("data",toSend.c_str());
    }
  }
}
