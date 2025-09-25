#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <DHT.h> // Thêm thư viện DHT

// Khai báo các chân cho thiết bị trên Arduino Uno
const int redLedPin = 8; 
const int greenLedPin = 7; 
const int buzzerPin = 9; 
const int servoPin = 10; 
const int buttonPin = 2;

// Sử dụng cảm biến DHT11 để đo nhiệt độ và độ ẩm
const int DHTPIN = A1; // Chân analog A1 để kết nối DHT11
#define DHTTYPE DHT11

// Khởi tạo đối tượng DHT
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo myServo;

bool isOn = false;
volatile bool buttonPressed = false; 

// Biến cho bộ đếm thời gian
const long sensorInterval = 2000; // 2 giây
unsigned long previousMillis = 0;

void onButton() {
  if (digitalRead(buttonPin) == LOW) {
    delay(100); 
    if (digitalRead(buttonPin) == LOW) {
      buttonPressed = true;
    }
  }
}

void updateState(bool state) {
  isOn = state;
  if (isOn) {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    tone(buzzerPin, 1000); 
    myServo.write(180);
    lcd.setCursor(0, 1);
    lcd.print("Cua mo, moi vao"); 
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    noTone(buzzerPin);
    myServo.write(0);
    lcd.setCursor(0, 1);
    lcd.print("Cua dong       ");
  }
}

void setup() {
  Serial.begin(9600); 
  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  myServo.attach(servoPin);
  lcd.init();
  lcd.backlight();
  dht.begin(); // Khởi động cảm biến DHT
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("He thong cua");
  
  updateState(isOn); 

  attachInterrupt(digitalPinToInterrupt(buttonPin), onButton, FALLING);
}

void loop() {
  // 1. Kiểm tra nút nhấn trên UNO
  if (buttonPressed) {
    buttonPressed = false;
    bool newState = !isOn;
    updateState(newState);
    
    // Gửi tín hiệu đến ESP32
    if (newState) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
  }

  // 2. Nhận lệnh từ ESP32
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "ON" && !isOn) {
      updateState(true);
    } else if (command == "OFF" && isOn) {
      updateState(false);
    }
  }

  // 3. Gửi giá trị nhiệt độ và độ ẩm mỗi 2 giây
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sensorInterval) {
    previousMillis = currentMillis;

    // Đọc giá trị từ cảm biến DHT
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    if (isnan(humidity) || isnan(temperature)) { // Kiểm tra xem việc đọc có thành công không
      Serial.println("Failed to read from DHT sensor!");
    } else {
      // Gửi giá trị nhiệt độ và độ ẩm qua Serial với tiền tố
      Serial.println("TEMP:" + String(temperature));
      Serial.println("HUMIDITY:" + String(humidity));
    }
  }
}