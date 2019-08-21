#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <Wire.h>
//#define LED_PIN01 D0
//#define LED_PIN02 D1
//#define PUSH_BUTTON D3

int ledpin01 = 16;  //pin D0
int ledpin02 = 5;   //pin D1
int PUSHBUTTON = 0; //pin D3

const char* ssid = "BlueFrost X";
const char* password = "starfrost49";
const char* mqtt_server = "broker.mqtt-dashboard.com";  // Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker

WiFiClient espClient;
PubSubClient client(espClient);

//const int currentPin = A0;              //Assign CT input to pin A0
//double kilos = 0;
//double peakPower = 0;
float notifier = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
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
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "room/alert") {
  Serial.print("Changing output to ");
  if (messageTemp == "true") {
    Serial.println("on");
    digitalWrite(ledPin02, HIGH);
    digitalWrite(ledPin01,LOW);
  }
  else if (messageTemp == "off") {
    Serial.println("off");
    digitalWrite(ledPin, HIGH);
//    digitalWrite(ledPin02, LOW);
  }
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
//      client.subscribe("room/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() { 
  Wire.begin();
  Serial.begin(9600);            //Start serial communication
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("Running");
  digitalWrite(ledpin01,LOW);
  digitalWrite(ledpin02,LOW);
}

void loop() {
   
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  int pushbutton = digitalRead(PUSHBUTTON);
  if(pushbutton == LOW) {
    notifier = 1;
//    client.publish("room/notify", notifier);
    digitalWrite(ledpin01,HIGH);
    digitalWrite(ledpin02,LOW);
  }
  else if(pushbutton == HIGH) {
    notifier = 0;
    digitalWrite(ledpin01,LOW);
    digitalWrite(ledpin02,LOW);
  }

  static char notifyNODE[7];
  dtostrf(notifier, 6, 2, notifyNODE);
  client.publish("room/notify", notifyNODE);
  Serial.println(notifyNODE);

  client.subscribe("room/alert");
}
  
//  int current = 0;
//  int maxCurrent = 0;
//  int minCurrent = 1000;
//  for (int i=0 ; i<=200 ; i++)  //Monitors and logs the current input for 200 cycles to determine max and min current
//  {
//    current = analogRead(currentPin);    //Reads current input and records maximum and minimum current
//    if(current >= maxCurrent)
//      maxCurrent = current;
//    else if(current <= minCurrent)
//      minCurrent = current;
//  }
//  if (maxCurrent <= 517)
//  {
//    maxCurrent = 516;
//  }
//  double RMSCurrent = ((maxCurrent - 516)*0.707)/11.8337;    //Calculates RMS current based on maximum value
//  double RMSPower = 220*RMSCurrent;    //Calculates RMS Power Assuming Voltage 220VAC, change to 110VAC accordingly
//  if (RMSPower > peakPower)
//  {
//    peakPower = RMSPower;
//  }
//  kilos = kilos + (RMSPower * (2.05/60/60/1000));    //Calculate kilowatt hours used
//  delay (2000);
//
//  static char currentElectricity[7];
//  dtostrf(RMSCurrent, 6, 2, currentElectricity);
//
//  static char powerElectricity[7];
//  dtostrf(RMSPower, 6, 2, powerElectricity);
//
//  static char powerConsumption[7];
//  dtostrf(kilos, 6, 2, powerConsumption);
//
//  static char peakPower[7];
//  dtostrf(peakPower, 6, 2, peakPower);
//
//  // Publishes Temperature and Humidity values
//  client.publish("room/current", RMSCurrent);
//  client.publish("room/power", RMSPower);
//  client.publish("room/powerconsumption", kilos);
//  client.publish("room/peakPower", peakPower);
    
//    Serial.println("====================");
//    Serial.print("Current: ");
//    Serial.print(RMSCurrent);
//    Serial.println("A");
//    Serial.print("Power: ");
//    Serial.print(RMSPower);
//    Serial.println("W");  
//    Serial.print("Power Consumption: ");
//    Serial.print(kilos);
//    Serial.println("kWh");
//    Serial.print("Peak Power: ");
//    Serial.print(peakPower);
//    Serial.println("W");
//    Serial.println("");
}
