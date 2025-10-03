#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define RST_PIN 9
#define SS_PIN 10
#define DHT_PIN 5
#define LDR_PIN A0
#define SERVO_PIN 6
#define BUZZER_PIN 2
#define LED_RED_PIN 4
#define LED_YELLOW_PIN 7
#define LED_GREEN_PIN 3

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;
DHT dht(DHT_PIN, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const String allowedUIDs[] = {
    "9480b405", 
    "271b4a01",
};
const int numAllowedUIDs = sizeof(allowedUIDs) / sizeof(allowedUIDs[0]);

unsigned long previousMillisSensor = 0;
const long intervalSensor = 2000; 

unsigned long servoOpenTime = 0;
const long servoOpenDuration = 3000;

bool isDoorOpen = false;

// Khai báo các hàm
void resetStatus();
void setReadyState();
void accessGranted();
void accessDenied();
void readSensorsAndSend();
void readRFIDAndSend();
void processCommand(String command);

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    
    servo.attach(SERVO_PIN);
    servo.write(0);
    
    dht.begin();
    
    lcd.init();
    lcd.backlight();

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);
    pinMode(LED_GREEN_PIN, OUTPUT);
    
    setReadyState(); // Bắt đầu với trạng thái sẵn sàng
}

void loop() {
    unsigned long currentMillis = millis();

    // Tự động đóng cửa sau 3 giây và quay về trạng thái sẵn sàng
    if (isDoorOpen && currentMillis - servoOpenTime >= servoOpenDuration) {
        servo.write(0);
        isDoorOpen = false;
        servoOpenTime = 0;
        Serial.println("SERVO_CLOSE_ACK"); 
    }

    // Gửi dữ liệu cảm biến và cập nhật LCD mỗi 2 giây
    if (currentMillis - previousMillisSensor >= intervalSensor) {
        previousMillisSensor = currentMillis;
        readSensorsAndSend();
    }
    
    // Chỉ đọc RFID khi cửa đang đóng
    if (!isDoorOpen) {
        readRFIDAndSend();
    }

    // Kiểm tra lệnh từ ESP32
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        processCommand(command);
    }
}

void resetStatus() {
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, HIGH);
    
    Serial.println("STATUS:READY");
    Serial.println("UID_RESET");
}

void setReadyState() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Ready!");
    lcd.setCursor(0, 1);
    lcd.print("Ready to Scan");
    resetStatus();
}

void accessGranted() {
    Serial.println("STATUS:GRANTED"); 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Shindo Xin chao");
    
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    
    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    
    servo.write(90);
    isDoorOpen = true; 
    servoOpenTime = millis();
    Serial.println("SERVO_OPEN_ACK"); 
    
    delay(2000); // Giữ trạng thái trong 2 giây trước khi đóng
    
    servo.write(0);
    isDoorOpen = false;
    Serial.println("SERVO_CLOSE_ACK");
}

void accessDenied() {
    Serial.println("STATUS:DENIED"); 
    
    for (int i = 0; i < 5; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(50);
        digitalWrite(BUZZER_PIN, LOW);
        delay(50);
    }
    
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Xin loi");
    lcd.setCursor(0, 1);
    lcd.print("The khong hop le");
    
    delay(2000); // Giữ trạng thái lỗi trong 2 giây
}

void readSensorsAndSend() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int light = analogRead(LDR_PIN);

    // Bổ sung cập nhật LCD ngay tại đây để màn hình không bị thiếu dữ liệu
    if (isnan(h) || isnan(t)) {
        Serial.println("DHT_ERROR");
        lcd.clear();
        lcd.print("DHT Error!");
    } else {
        String sensorData = "T" + String(t, 1) + ",H" + String(h, 1) + ",L" + String(light);
        Serial.println(sensorData);
        lcd.setCursor(0, 0);
        lcd.print("T:" + String(t, 1) + "C H:" + String(h, 1) + "%");
        lcd.setCursor(0, 1);
        if (light < 300) {
            lcd.print("Light: LOW");
        } else {
            lcd.print("Light: HIGH");
        }
    }
}

void readRFIDAndSend() {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        String cardID = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
            if (rfid.uid.uidByte[i] < 0x10) cardID += "0";
            cardID += String(rfid.uid.uidByte[i], HEX);
        }
        rfid.PICC_HaltA();
        
        Serial.println("RFID_SCAN:" + cardID);

        bool isAuthorized = false;
        for (int i = 0; i < numAllowedUIDs; i++) {
            if (cardID.equalsIgnoreCase(allowedUIDs[i])) {
                isAuthorized = true;
                break;
            }
        }
        
        if (isAuthorized) {
            accessGranted();
        } else {
            accessDenied();
        }
        resetStatus();
    }
}

void processCommand(String command) {
    command.trim(); // Loại bỏ khoảng trắng
    if (command.equals("SERVO_OPEN")) {
        servo.write(90);
        isDoorOpen = true; 
        servoOpenTime = millis(); 
        Serial.println("SERVO_OPEN_ACK");
    } else if (command.equals("SERVO_CLOSE")) {
        servo.write(0);
        isDoorOpen = false;
        servoOpenTime = 0; 
        Serial.println("SERVO_CLOSE_ACK");
        resetStatus(); // Chuyển về trạng thái sẵn sàng ngay lập tức
    } else if (command.equals("BUZZER_ON")) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println("BUZZER_ACK");
    }
}