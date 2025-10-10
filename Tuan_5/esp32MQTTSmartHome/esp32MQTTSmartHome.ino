#include <WiFi.h> 
#include <PubSubClient.h> 

const char* ssid = "Quang Vinh"; 
const char* password = "quangvinh123"; 

const char* mqtt_server = "test.mosquitto.org"; 
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_Gateway_01";

const char* SUB_TOPIC_TEMP_HUM = "home/livingroom/temp_hum"; 
const char* SUB_TOPIC_GARAGE_DOOR_SET = "home/garage/door/set";
const char* SUB_TOPIC_HOME_DOOR_SET = "home/door/set"; 
const char* SUB_TOPIC_GARAGE_LIGHT_SET = "home/garage/light/set"; 

const char* PUB_TOPIC_GARAGE_MOTION = "home/garage/motion";
const char* PUB_TOPIC_GARAGE_LIGHT_STATUS = "home/garage/light/status";
const char* PUB_TOPIC_GARAGE_DOOR_STATUS = "home/garage/door/status";
const char* PUB_TOPIC_HOME_DOOR_STATUS = "home/door/status"; 

WiFiClient espClient;
PubSubClient client(espClient);

#define ESP_TX_PIN 17 
#define ESP_RX_PIN 16 

String getValue(String data, char key) {
  String keyString = String(key) + ":";
  int startIndex = data.indexOf(keyString);
  
  if (startIndex == -1) return "";

  startIndex += keyString.length();
  int endIndex = data.indexOf(',', startIndex);
  
  if (endIndex == -1) {
    endIndex = data.length();
  }
  
  return data.substring(startIndex, endIndex);
}

void setup_wifi() {
  delay(10);
  Serial.print("Dang ket noi den ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi da ket noi");
  Serial.print("Dia chi IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Nhan lenh/du lieu tu Topic: ");
  Serial.println(topic);

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Noi dung: ");
  Serial.println(message);

  String commandToSend = "";
  String currentTopic = String(topic);

  if (currentTopic == SUB_TOPIC_TEMP_HUM) {
    commandToSend = message; 
    Serial.print("GUI DU LIEU T/H DEN UNO: ");
    Serial.println(commandToSend);
  } else if (currentTopic == SUB_TOPIC_GARAGE_DOOR_SET) {
    commandToSend = "GARA_DOOR:" + message; 
  } else if (currentTopic == SUB_TOPIC_HOME_DOOR_SET) {
    commandToSend = "HOME_DOOR:" + message; 
  } else if (currentTopic == SUB_TOPIC_GARAGE_LIGHT_SET) {
    commandToSend = "GARA_LIGHT:" + message; 
  }

  if (commandToSend.length() > 0) {
    Serial2.println(commandToSend);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Dang co gang ket noi MQTT...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("da ket noi");
      
      client.subscribe(SUB_TOPIC_TEMP_HUM); 
      client.subscribe(SUB_TOPIC_GARAGE_DOOR_SET);
      client.subscribe(SUB_TOPIC_HOME_DOOR_SET);
      client.subscribe(SUB_TOPIC_GARAGE_LIGHT_SET);
      Serial.println("Da dang ky cac topic thanh cong.");
    } else {
      Serial.print("that bai, rc=");
      Serial.print(client.state());
      Serial.println(" thu lai sau 5 giay");
      delay(5000);
    }
  }
}

void processAndPublishStatus(String data) {
    
  String motionStatus = getValue(data, 'M');
  if (motionStatus.length() > 0) {
    client.publish(PUB_TOPIC_GARAGE_MOTION, motionStatus.c_str());
  }

  String lightStatus = getValue(data, 'L');
  if (lightStatus.length() > 0) {
    client.publish(PUB_TOPIC_GARAGE_LIGHT_STATUS, lightStatus.c_str());
  }
  
  String garageDoorStatus = getValue(data, 'G');
  if (garageDoorStatus.length() > 0) {
    client.publish(PUB_TOPIC_GARAGE_DOOR_STATUS, garageDoorStatus.c_str());
  }
  
  String homeDoorStatus = getValue(data, 'D');
  if (homeDoorStatus.length() > 0) {
    client.publish(PUB_TOPIC_HOME_DOOR_STATUS, homeDoorStatus.c_str()); 
    Serial.print("PUBLISH D: "); Serial.println(homeDoorStatus);
  }
}

void setup() {
  Serial.begin(115200); 
  Serial2.begin(9600, SERIAL_8N1, ESP_RX_PIN, ESP_TX_PIN);

  Serial.println("ESP32 Serial & MQTT Gateway khoi dong.");

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Ket noi voi Arduino UNO da san sang.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();
    
    if (data.length() > 0) {
      Serial.print("NHAN TRANG THAI TU UNO: ");
      Serial.println(data);
      
      processAndPublishStatus(data);
    }
  }
}