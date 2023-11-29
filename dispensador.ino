#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char * WIFI_SSID = "TECHLAB";
const char * WIFI_PASS = "catolica11";

const char * MQTT_BROKER = "a3v2gf9whawh53-ats.iot.us-east-2.amazonaws.com";
const int MQTT_BROKER_PORT = 8883;

const char * MQTT_CLIENT_ID = "ignacio.martinez@ucb.edu.bo";
const char * UPDATE_ACCEPTED_TOPIC = "$aws/things/dispenser/shadow/update/accepted";
const char * UPDATE_TOPIC = "$aws/things/dispenser/shadow/update";//{"state":{"desired":{"dispenser":"on"}}} {"state":{"desired":{"manual":"on"}}}
const char * PUBLISH_TOPIC = "publish/led";
const char * SUBSCRIBE_TOPIC = "subscribe/led";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wiFiClient;
PubSubClient mqttClient(wiFiClient);

String manual = "off";
String dispenser = "unknown";
const int pinLed = 25;
const int triggerPin1 = 16, echoPin1 = 17;
const int triggerPin2 = 18, echoPin2 = 19;
const int IN1 = 32, IN2 = 35;
int cm1, cm2;

long readUltrasonicDistance(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

StaticJsonDocument<JSON_OBJECT_SIZE(6)> outputDoc;
char outputBuffer[128];

void reportmanual() {
  outputDoc["state"]["reported"]["manual"] = manual.c_str();
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(UPDATE_TOPIC, outputBuffer);
}

void reportdispenser() {
  outputDoc["state"]["reported"]["dispenser"] = dispenser.c_str();
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(UPDATE_TOPIC, outputBuffer);
}

void setdispenser(String str) {
  dispenser = str;
  Serial.println(str);
  if (dispenser == "off") {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(pinLed, LOW);
  } else if (dispenser == "on") {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(pinLed, HIGH);
  }
  reportdispenser();
}

void setmanual(String str) {
  manual = str;
  Serial.println(str);
  reportmanual();
}

StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;

void callback(const char * topic, byte * payload, unsigned int lenght) {
  String message;
  for (int i = 0; i < lenght; i++) {
    message += String((char) payload[i]);
  }
  if (String(topic) == UPDATE_ACCEPTED_TOPIC) {
    Serial.println("Message from topic " + String(topic) + ":" + message);

    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
      String tmpdispenser = String(inputDoc["state"]["desired"]["dispenser"].as<const char*>());
      String tmpmanual = String(inputDoc["state"]["desired"]["manual"].as<const char*>());
      if(!tmpdispenser.isEmpty() && manual == "on") 
        setdispenser(tmpdispenser);
      if(!tmpmanual.isEmpty()) 
        setmanual(tmpmanual);
    }
  }
  if (String(topic) == SUBSCRIBE_TOPIC) {
    Serial.println("Message from topic " + String(topic) + ":" + message);
    
    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
      String action = String(inputDoc["action"].as<const char*>());
      if (action == "on") {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(pinLed, HIGH);
      } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(pinLed, LOW);
      }
    }
  }
}

boolean mqttClientConnect() {
  Serial.print("Connecting to " + String(MQTT_BROKER));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println(" DONE!");

    mqttClient.subscribe(UPDATE_ACCEPTED_TOPIC);
    mqttClient.subscribe(SUBSCRIBE_TOPIC);
    Serial.println("Subscribed to " + String(UPDATE_ACCEPTED_TOPIC));
    Serial.println("Subscribed to " + String(SUBSCRIBE_TOPIC));
  } else {
    Serial.println("Can't connect to " + String(MQTT_BROKER));
  }
  return mqttClient.connected();
}

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  Serial.begin(115200);
  Serial.print("Connecting to " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println(" DONE!");
  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  mqttClient.setCallback(callback);
}

unsigned long previousConnectMillis = 0;

unsigned long previousPublishMillis = 0;
unsigned char counter = 0;

void publishValue(int distanceGlassSensor,  int distanceLiquidSensor) {
  outputDoc["state"]["reported"]["distanceGlassSensor"] = distanceGlassSensor;
  outputDoc["state"]["reported"]["distanceLiquidSensor"] = distanceLiquidSensor;
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(PUBLISH_TOPIC, outputBuffer);
}

void loop() {
  unsigned long now = millis();
  if (!mqttClient.connected()) {
    if (now - previousConnectMillis >= 2000) {
      previousConnectMillis = now;
      if (mqttClientConnect()) previousConnectMillis = 0;
      else delay(1000);
    }
  } else {
    mqttClient.loop();
    delay(20);
  }

  if (now - previousPublishMillis >= 5000 && manual == "off") {
      previousPublishMillis = now;
      cm1 = 0.01723 * readUltrasonicDistance(triggerPin1, echoPin1);
      cm2 = 0.01723 * readUltrasonicDistance(triggerPin2, echoPin2);
      Serial.println(cm1);
      Serial.println(cm2);
      publishValue(cm1, cm2);
  }

}
