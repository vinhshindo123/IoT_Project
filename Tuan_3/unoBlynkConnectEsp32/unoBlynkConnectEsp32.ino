#include "DHT.h"

// Cài đặt chân cắm và các biến
const int DHT_PIN = 3;
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

const int LED_PIN = 7; 
const int BUTTON_PIN = 9; 
const int FAN_PIN = 8; 

int LED_STATE = 0; 
int FAN_STATE = 0; 
float currentTemp = 0;
int tempThreshold = 38; 

// Các biến cho logic chống dội phím
int buttonState;             // Trạng thái hiện tại của nút
int lastButtonState = HIGH;  // Trạng thái cuối cùng đã biết của nút
unsigned long lastDebounceTime = 0;
const long debounceDelay = 50;

// Hàm đọc và gửi dữ liệu DHT11
void readAndSendDHT() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    return;
  }
  
  currentTemp = t;
  Serial.print("C");
  Serial.println(t);
  Serial.print("H");
  Serial.println(h);
  
  // Kiểm tra ngưỡng nhiệt độ và tự động bật/tắt quạt
  if (currentTemp > tempThreshold) {
    if (FAN_STATE == 0) {
      FAN_STATE = 1;
      digitalWrite(FAN_PIN, HIGH);
    }
  } else {
    if (FAN_STATE == 1) {
      FAN_STATE = 0;
      digitalWrite(FAN_PIN, LOW);
    }
  }
}

void setup() {
  Serial.begin(9600); // Giao tiếp với ESP32
  dht.begin();
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
}

void loop() {
  // Đọc dữ liệu từ ESP32
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    char command = data.charAt(0);
    int value = data.substring(1).toInt();
    
    if (command == 'L') { // Lệnh bật/tắt LED
      LED_STATE = value;
      digitalWrite(LED_PIN, LED_STATE);
    } else if (command == 'T') { // Lệnh đặt ngưỡng nhiệt độ
      tempThreshold = value;
    }
  }

  // Xử lý nút vật lý và gửi lệnh về ESP32
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) { // Nút đã được nhấn
        LED_STATE = !LED_STATE; // Đảo trạng thái của đèn
        digitalWrite(LED_PIN, LED_STATE); // Bật/tắt đèn
        
        // In ra màn hình Serial để gỡ lỗi
        Serial.print("Button pressed. LED State: ");
        Serial.println(LED_STATE);
        
        // Gửi lệnh bật/tắt LED về ESP32
        Serial.print("L"); 
        Serial.println(LED_STATE);
      }
    }
  }
  lastButtonState = reading;

  // Đọc DHT mỗi 1 giây và gửi về ESP32
  static unsigned long lastDHTRead = 0;
  if (millis() - lastDHTRead > 1000) {
    lastDHTRead = millis();
    readAndSendDHT();
  }
}