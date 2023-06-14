#include <WiFi.h>
#include <PubSubClient.h>
#include<SPI.h>

#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <BH1750.h>

const char* ssid = "iPhone 11";
const char* password = "Hieu2002";
const char* mqtt_server = "broker.mqttdashboard.com"; //broker.mqttdashboard.com


#define MQTT_LED_TOPIC "MQTT_ESP32/LED"
#define MQTT_PUMP_TOPIC "MQTT_ESP32/PUMP"
#define MQTT_ION_TOPIC "MQTT_ESP32/ION"
#define MQTT_DHT22_t_TOPIC "MQTT_ESP32/DHT22/t"
#define MQTT_DHT22_h_TOPIC "MQTT_ESP32/DHT22/h"
#define MQTT_BH1750_TOPIC "MQTT_ESP32/BH1750"
#define MQTT_HCSR04_TOPIC "MQTT_ESP32/HCSR04"

WiFiClient espClient;
PubSubClient client(espClient);

const int BUTTON_PIN_LED = 34;
const int LED_PIN = 19;

const int BUTTON_PIN_PUMP = 32;
const int PUMP_PIN = 15;

const int BUTTON_PIN_ION = 35;
const int ION_PIN = 13;

const int BUTTON_PIN_WF = 33;

int curButtonState_LED = 0;
int preButtonState_LED = 0;
int ButtonState_LED = 0;

int curButtonState_PUMP = 0;
int preButtonState_PUMP = 0;
int ButtonState_PUMP = 0;

int curButtonState_ION = 0;
int preButtonState_ION = 0;
int ButtonState_ION = 0;

const int DHTPIN = 4;
const int DHTTYPE = DHT11;
DHT dht(DHTPIN, DHTTYPE);
const int trig = 5;     
const int echo = 18;

BH1750 lightMeter;

unsigned long now = 0;

byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
//  char status[20];
  Serial.println("-------NEW MESSAGE FROM BROKER-----");
  Serial.print("TOPIC: ");
  Serial.println(topic);
  Serial.print("MESSAGE: ");
  Serial.write(payload, length);
  Serial.println();
//  for(int i = 0; i<length; i++)
//  {
//    status[i] = payload[i];
//  }
//  Serial.println(status);
  if(String(topic) == MQTT_LED_TOPIC)
  {
    if((char)payload[0] == '0')
    {
      ButtonState_LED = 0;
      digitalWrite (LED_PIN, LOW);
      Serial.println("ION OFF");
    }
    else if((char)payload[0] == '1')
    {
      ButtonState_LED = 1; 
      digitalWrite (LED_PIN, HIGH);
      Serial.println("LED ON");       
    }
  } else if (String(topic) == MQTT_PUMP_TOPIC)
  {
    if((char)payload[0] == '0')
    {
      ButtonState_PUMP = 0;
      digitalWrite (PUMP_PIN, LOW);
      Serial.println("PUMP OFF");
    }
    else if((char)payload[0] == '1')
    {
      ButtonState_PUMP = 1;   
      digitalWrite (PUMP_PIN, HIGH);
      Serial.println("PUMP ON");     
    }
  } else if (String(topic) == MQTT_ION_TOPIC) 
  {
    if((char)payload[0] == '0')
    {
      ButtonState_ION = 0;
      digitalWrite (ION_PIN, LOW);
      Serial.println("ION OFF"); 
    }
    else if((char)payload[0] == '1')
    {
      ButtonState_ION = 1;  
      digitalWrite (ION_PIN, HIGH);
      Serial.println("ION ON");       
    }
  }
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//
//  //network.update(); // Check the network regularly
//
//  Serial.print(F("Sending... "));
//  int data = 0;
//
//  if ((char)payload[0] == '0') {
//    data = 0;
//  } else if ((char)payload[0] == '1') {
//    data = 1;
//  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "clientId-DtIVwtD9Vi";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //      client.publish("EFG", "hello world");
      // ... and resubscribe
      client.subscribe("MQTT_ESP32/#");
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
  pinMode(BUTTON_PIN_LED, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN_PUMP, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(BUTTON_PIN_ION, INPUT);
  pinMode(ION_PIN, OUTPUT);
  pinMode(BUTTON_PIN_WF, INPUT);
  Serial.begin(9600);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Wire.begin(); 
  dht.begin();
  
  lightMeter.begin();
  pinMode(trig,OUTPUT);   // chân trig sẽ phát tín hiệu
  pinMode(echo,INPUT);

  now = millis();
}


void loop() {

 if (!client.connected()) {
   reconnect();
 }
 
 client.loop();
   // LED
 preButtonState_LED = curButtonState_LED;
 curButtonState_LED = digitalRead(BUTTON_PIN_LED);
 
 if (curButtonState_LED == 1 && preButtonState_LED == 0 && ButtonState_LED == 0) {
   ButtonState_LED = 1;
   client.publish(MQTT_LED_TOPIC, "1");
 } else if (curButtonState_LED == 1 && preButtonState_LED == 0 && ButtonState_LED == 1) {
   ButtonState_LED = 0;
   client.publish(MQTT_LED_TOPIC, "0");
 }
//  if (ButtonState_LED == 1) 
//  {
//   digitalWrite (LED_PIN, HIGH);
//   client.publish(MQTT_LED_TOPIC, "ON");
//   Serial.println("LED ON");
//  } else {
//   digitalWrite (LED_PIN, LOW);
//   client.publish(MQTT_LED_TOPIC, "OFF");
//   Serial.println("LED OFF");
//  } 
//PUMP
 preButtonState_PUMP = curButtonState_PUMP;
 curButtonState_PUMP = digitalRead(BUTTON_PIN_PUMP);
 
 if (curButtonState_PUMP == 1 && preButtonState_PUMP == 0 && ButtonState_PUMP == 0) {
   ButtonState_PUMP = 1;
   client.publish(MQTT_PUMP_TOPIC, "1");
 } else if (curButtonState_PUMP == 1 && preButtonState_PUMP == 0 && ButtonState_PUMP == 1) {
   ButtonState_PUMP = 0;
   client.publish(MQTT_PUMP_TOPIC, "0");
 }
// if (ButtonState_PUMP == 1) {
//   digitalWrite (PUMP_PIN, HIGH);
//   client.publish(MQTT_PUMP_TOPIC, "ON");
//   Serial.println("PUMP ON");
// } else {
//   digitalWrite (PUMP_PIN, LOW);
//   client.publish(MQTT_PUMP_TOPIC, "OFF");
//   Serial.println("PUMP OFF");
// } 

    //ION
   preButtonState_ION = curButtonState_ION;
 curButtonState_ION = digitalRead(BUTTON_PIN_ION);
 
 if (curButtonState_ION == 1 && preButtonState_ION == 0 && ButtonState_ION == 0) {
   ButtonState_ION = 1;
   client.publish(MQTT_ION_TOPIC, "1");
 } else if (curButtonState_ION == 1 && preButtonState_ION == 0 && ButtonState_ION == 1) {
   ButtonState_ION = 0;
   client.publish(MQTT_ION_TOPIC, "0");
 }
// if (ButtonState_ION == 1) {
//   digitalWrite (ION_PIN, HIGH);
//   client.publish(MQTT_ION_TOPIC, "ON");
//   Serial.println("ION ON");
// } else {
//   digitalWrite (ION_PIN, LOW);
//   client.publish(MQTT_ION_TOPIC, "OFF");
//   Serial.println("ION OFF");
// } 
  
  float t=0, h=0, lux=0;
  int distance=0;

  if ( (unsigned long) (millis() - now) > 5000)
  {
  
      // Thay đổi trạng thái đèn led
      float h = dht.readHumidity();
      float t = dht.readTemperature();
    
      if (isnan(t) || isnan(h)) { 
      } 
      else {
         Serial.print(h);
         Serial.print(" ");
         Serial.println(t);
    //    lcd.setCursor(10,0);
    //    lcd.print(t,1);
    //    lcd.write(1);
    //    lcd.print("C");
    //
    //    lcd.setCursor(10,1);
    //    lcd.print(h,1);
    //    lcd.print("%");    
      }
    
      float lux = lightMeter.readLightLevel();//định nghĩa 1 kiểu số thực
      Serial.print("Light: ");
      Serial.println(lux, 2);
    
      unsigned long duration; // biến đo thời gian
      int distance;           // biến lưu khoảng cách
      
      /* Phát xung từ chân trig */
      digitalWrite(trig,0);   // tắt chân trig
      delayMicroseconds(2);
      digitalWrite(trig,1);   // phát xung từ chân trig
      delayMicroseconds(5);   // xung có độ dài 5 microSeconds
      digitalWrite(trig,0);   // tắt chân trig
      //digitalWrite(13,LOW);
      
      /* Tính toán thời gian */
      // Đo độ rộng xung HIGH ở chân echo. 
      duration = pulseIn(echo,HIGH);  
      // Tính khoảng cách đến vật.
      distance = int(duration/2/29.412);
      Serial.println(distance);
      
      // cập nhật lại biến time
      now = millis();
      
  }
  String T = String(t);
  String H = String(h);
  String LUX = String(lux);
  String DISTANCE = String(distance);
  client.publish(MQTT_DHT22_t_TOPIC, T.c_str());
  client.publish(MQTT_DHT22_h_TOPIC, H.c_str());
  client.publish(MQTT_BH1750_TOPIC, LUX.c_str());
  client.publish(MQTT_HCSR04_TOPIC, DISTANCE.c_str());
  
}
