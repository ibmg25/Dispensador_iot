#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char * WIFI_SSID = "TECHLAB";
const char * WIFI_PASS = "catolica11";

const char * MQTT_BROKER = "a3v2gf9whawh53-ats.iot.us-east-2.amazonaws.com";
const int MQTT_BROKER_PORT = 8883;

const char * MQTT_CLIENT_ID = "ignacio.martinez@ucb.edu.bo";
const char * UPDATE_ACCEPTED_TOPIC = "$aws/things/dispenser/shadow/update/accepted";
const char * UPDATE_TOPIC = "$aws/things/dispenser/shadow/update";  //{"state":{"desired":{"dispenser":"on"}}}
const char * PUBLISH_TOPIC = "publish/led";
const char * SUBSCRIBE_TOPIC = "subscribe/led";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUKczsJbekmlB3TUgC17WOXTBEPTQwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMTEyMDEzNTg0
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMgYX014F1bTEvL+jDeP
g2Xg4WfBih7iEJIZm5xrBSKxw1U+Iq+EpUT+YbsuA0StHJ2br8CW13XV4pyvIlBq
Tt4A6cMGSGzpHKhWrcm5iBXmZXqjhfZ3yUxnizbKAt+9p3bd7/b4EtVsd7GwT0Lz
KU/2ajed/u2y2NPFNkys09xB7i7m5aotfKJ0dSJgP0RUQDqyrHm+aStH745ze/YK
NNshLOwOR3B5vdVlta2VRXAGJ53W6WqwW1I0fDQ2AP4c+tdL0lllEYchfwGE6jZQ
IelDqA0pwFY9r90mV71pQZIsG3/xfD6b8NorKoxk10DfTd3hljfeUPyv4l7g7Q4E
228CAwEAAaNgMF4wHwYDVR0jBBgwFoAUOgHPZI/OnUCpgYPI553kBcny3DowHQYD
VR0OBBYEFGa+U5L4ivJBgA1jN4DvqxGuXEg4MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQANjFpQWFOgnfqAWwjyeO84Y8fB
RX86SjG79vj/tao1w4I2imnVj0aPi+FmDRWLAcCYUwfZMVIEpvavASoadaWFyS2d
Emgi5Y6o8khDTwae4cT2smkG9gEKfM5hUcTfj4tgK9ch8HxClU+SBxY2tm842VhQ
D9nuVHKqGfCPpQqi3zlro1Wes70kBxhcSpmFIrFWmshcY6vf9+k2CXn4xqwt8zy5
l3wKyk+o93CzMcJ2/UFkwmNObwzgrmYYz+DIEH+zGPhn3O63zBax9IVbd+QnRNhA
FGpWb1/gkW11jtxyo+YpcEk7KrPJkT0Be31dhTPoYZXoHCXwcrC6AaSnWzpW
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAyBhfTXgXVtMS8v6MN4+DZeDhZ8GKHuIQkhmbnGsFIrHDVT4i
r4SlRP5huy4DRK0cnZuvwJbXddXinK8iUGpO3gDpwwZIbOkcqFatybmIFeZleqOF
9nfJTGeLNsoC372ndt3v9vgS1Wx3sbBPQvMpT/ZqN53+7bLY08U2TKzT3EHuLubl
qi18onR1ImA/RFRAOrKseb5pK0fvjnN79go02yEs7A5HcHm91WW1rZVFcAYnndbp
arBbUjR8NDYA/hz610vSWWURhyF/AYTqNlAh6UOoDSnAVj2v3SZXvWlBkiwbf/F8
Ppvw2isqjGTXQN9N3eGWN95Q/K/iXuDtDgTbbwIDAQABAoIBAQC5eHXmLfmcPjwj
RrUbzwfhyCfG78uiQrNPaTMnMr/o39MKUXT/MjmrEQvv33+iCcAriyLGpksC64WI
7IEm/tMpzrjfUBST9yPcGbQu9q1gd1X67lX/FhtdmeCdoXQ2heoTMTb7ughjEY6F
AsMFcLULbIIaGXYySAPZj8R8H/qk/kccCGmHTWl6K8wC3iZ3rsrm0V3xxfsmmKM3
Zz9OkpQxD5rNjHO9yBCKVGwo5oRMzB3z7p9VHlXJUYHkCMHDtZbaWc/f6quFJ95k
oDWE9SYSigOfHfsNuOpj4DM05mYjlA2WdXWn1rID4iRshYvyY1OL4sJpcOM7XTfo
uNsQPrepAoGBAPOjCgEG1vZM0t8T/gA3uYn0J6gB+BkjQeE0arztRNzVnEgKWYDh
wRvLwDqsdKN1+BHR60sCf7K+gCxsgWqxiRjWiwXEcQS5SeKtp3jpZ0wnJ0qc6x0I
2o2hajqPkcLVh+qtkQreCfR9a1X+mTyXJwnQd6j0o7d/nKgqdp2RadWNAoGBANI/
tLMub7OIlpC0n/Y6g0MBlb9wTZ5GIig6Cbq3KL7C1IwXkAOQxBPOiLMcY/djjB21
63qFG/vqCyttFsZMAdvB++ko08nZfz/MbTA2Hto8K6dHc/FN3id334izImXpP3Qq
sHYtCrCEVBSQldMkVtaIaqNDvvkiC+bk/K3SON/rAoGBAO2pq4aY33TgOAv4epxH
v6w9CbowQY50AD3pXf3yDmzFy2/S+MBqeG/e8CPMOvOBjn4Dol1HkL4SawHqzsv/
Z1ddYbw8hmsvPE3tTCL8vBFjWEbGj+uAyYHDZda+WlgO6EkbCxPoyRlAh0YZ1+1t
HV3LiA6+pj3INE8vLQpD4VO5AoGBAKBkUSnQZw6DSLmj0dQ51yEq+F5PAieYQT6G
6B001TSR1Sv6XbcanTqu+MILCogTvTR9L+ML4MlRRqweUJ1+JnzLg63fo5H9BoJZ
JYL+BpNeC79VVzHiwXNCk5jpb3hndCze1DeTLPB0QuBdXK7yrfyDnTZn3CyauJGw
ZsMD+Ld5AoGAM5KxoRicxMqbpVUg8DwiuTdaFjOB1pz/T2J7gHLIk+P0PiW6egtf
z3wP39+Qi4FCBGiDH70WIbGj9uVc7q5GPoqKiUJyTMiADbiGEQUq23oTK9GH92nU
/8OokY47Vr09CXKP7EFKbB12bVWTpfi1IswwWRj5ezBWEXGyFwH1VmY=
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wiFiClient;
PubSubClient mqttClient(wiFiClient);

String dispenser = "unknown";
//const int pinLed = 25;
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
  } else if (dispenser == "on") {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  reportdispenser();
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
      if(!tmpdispenser.isEmpty() && !tmpdispenser.equals(dispenser)) 
        setdispenser(tmpdispenser);
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
        //setdispenser("on");
      } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        //setdispenser("off");
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
  //pinMode(pinLed, OUTPUT);
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
  } else { // Connected to the MQTT Broker
    mqttClient.loop();
    delay(20);
  }

  if (now - previousPublishMillis >= 1000) {
      previousPublishMillis = now;
      cm1 = 0.01723 * readUltrasonicDistance(triggerPin1, echoPin1);
      cm2 = 0.01723 * readUltrasonicDistance(triggerPin2, echoPin2);
      Serial.println(cm1);
      Serial.println(cm2);
      // publish
      publishValue(cm1, cm2);
      //publishValue(counter++);
  }

}
