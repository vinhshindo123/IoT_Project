#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 2
#define SERVO_PIN 5
#define SERVO2_PIN 6
#define IR_PIN 3
#define LED_PIN 4
#define LDR_PIN A0

#define LOCK_ANGLE 0
#define UNLOCK_ANGLE 90
#define UNLOCK_DURATION 2000
#define DARK_THRESHOLD 300

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo doorServo;
Servo motionServo;

byte validCards[][4] = {
  {0x27, 0x1B, 0x4A, 0x01},
  {0x94, 0x80, 0xB4, 0x05}
};
const int CARD_COUNT = sizeof(validCards) / sizeof(validCards[0]);

float currentTemperature = 0.0;
float currentHumidity = 0.0;
String lightStatus = "OFF";
String doorStatus = "LOCK";
String garageStatus = "LOCK";
String motionStatus = "NONE";

const long SEND_STATUS_INTERVAL = 1000;
unsigned long lastStatusSend = 0;
bool isLcdDefault = false;

void updateDefaultLcd() {
  lcd.setCursor(0, 0);
  lcd.print("QUET THE TIEM CAN");

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(currentTemperature, 1);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print(currentHumidity, 1);
  lcd.print("%   ");
  isLcdDefault = true;
}

bool compareArrays(byte *a, byte *b, byte size) {
  for (byte i = 0; i < size; i++) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

bool checkAccess(byte *scannedUID) {
  for (int i = 0; i < CARD_COUNT; i++) {
    if (compareArrays(scannedUID, validCards[i], 4)) {
      return true;
    }
  }
  return false;
}

void beepBuzzer(int durationMs) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(durationMs);
  digitalWrite(BUZZER_PIN, LOW);
}

void parseIncomingData() {
  while (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();

    if (data.startsWith("T") && data.indexOf(",H") != -1) {
      int tempStartIndex = data.indexOf('T') + 1;
      int tempEndIndex = data.indexOf(',');
      int humStartIndex = data.indexOf('H') + 1;

      String tempStr = data.substring(tempStartIndex, tempEndIndex);
      currentTemperature = tempStr.toFloat();

      String humStr = data.substring(humStartIndex);
      currentHumidity = humStr.toFloat();

      Serial.print("NHAN DU LIEU T/H: T=");
      Serial.print(currentTemperature);
      Serial.print(", H=");
      Serial.println(currentHumidity);
      isLcdDefault = false;
    } else if (data.startsWith("HOME_DOOR:")) {
      String action = data.substring(10);
      
      if (action == "ON") {
        doorServo.write(UNLOCK_ANGLE);
        doorStatus = "UNLOCK";
        Serial.println("LENH: Mo Cua Nha");
      } else if (action == "OFF") {
        doorServo.write(LOCK_ANGLE);
        doorStatus = "LOCK";
        Serial.println("LENH: Khoa Cua Nha");
      }

    } else if (data.startsWith("GARA_DOOR:")) {
      String action = data.substring(10);
      
      if (action == "ON") {
        motionServo.write(UNLOCK_ANGLE);
        garageStatus = "UNLOCK";
        Serial.println("LENH: Mo Cua Gara");
      } else if (action == "OFF") {
        motionServo.write(LOCK_ANGLE);
        garageStatus = "LOCK";
        Serial.println("LENH: Khoa Cua Gara");
      }

    } else if (data.startsWith("GARA_LIGHT:")) {
      String action = data.substring(11);
      
      if (action == "ON") {
        digitalWrite(LED_PIN, HIGH);
        lightStatus = "ON";
        Serial.println("LENH: Bat Den Gara");
      } else if (action == "OFF") {
        digitalWrite(LED_PIN, LOW);
        lightStatus = "OFF";
        Serial.println("LENH: Tat Den Gara");
      }
    }
  }
}

void sendStatusToESP() {
  String statusMessage = "L:" + lightStatus +
                         ",D:" + doorStatus +
                         ",G:" + garageStatus +
                         ",M:" + motionStatus;

  Serial.println(statusMessage);
  Serial.print("GUI TRANG THAI: ");
  Serial.println(statusMessage);
}


void setup() {
  Serial.begin(9600); 
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  pinMode(IR_PIN, INPUT); 
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  doorServo.attach(SERVO_PIN);
  doorServo.write(LOCK_ANGLE);
  
  motionServo.attach(SERVO2_PIN); 
  motionServo.write(LOCK_ANGLE);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.print("HE THONG KHOA");
  lcd.setCursor(0, 1);
  lcd.print("DA KHOI DONG");
  delay(1500);
  
  Serial.println("System Initialized. Door Locked.");
  isLcdDefault = false; 
}


void loop() {
  parseIncomingData();

  int lightValue = analogRead(LDR_PIN);
  if (lightValue < DARK_THRESHOLD) {
    if (digitalRead(LED_PIN) == LOW) {
      digitalWrite(LED_PIN, HIGH);
      lightStatus = "ON";
    } 
  } else {
    digitalWrite(LED_PIN, LOW);
    lightStatus = "OFF"; 
  }

  int motionState = digitalRead(IR_PIN);
  if (motionState == LOW) { 
    motionStatus = "DETECT"; 
    
    if (motionServo.read() == LOCK_ANGLE) { 
      Serial.println("CHUYEN DONG PHAT HIEN! MO SERVO 2.");
      
      beepBuzzer(50); 
      
      if (!mfrc522.PICC_IsNewCardPresent()) {
          isLcdDefault = false; 
          lcd.clear();
          lcd.print("CHUYEN DONG...");
          lcd.setCursor(0, 1);
          lcd.print("MO SERVO PHU");
      }
      
      motionServo.write(UNLOCK_ANGLE);
      garageStatus = "UNLOCK"; 
      
      delay(2000); 
      
      motionServo.write(LOCK_ANGLE);
      garageStatus = "LOCK"; 
      Serial.println("Servo 2 da dong.");
      isLcdDefault = false; 
    }
  } else {
    motionStatus = "NONE"; 
  }
  
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      isLcdDefault = false; 
      
      if (checkAccess(mfrc522.uid.uidByte)) {
        
        if (motionState == HIGH) { 
          
          Serial.println("TRUY CAP THANH CONG! MO KHOA.");
          
          beepBuzzer(50);
          delay(50);
          beepBuzzer(50);
          
          lcd.clear();
          lcd.print("TRUY CAP THANH CONG");
          lcd.setCursor(0, 1);
          lcd.print("CUA DANG MO...");
          doorServo.write(UNLOCK_ANGLE);
          doorStatus = "UNLOCK"; 
          
          delay(UNLOCK_DURATION); 

          doorServo.write(LOCK_ANGLE);
          doorStatus = "LOCK"; 
          Serial.println("Cua da dong.");

        } else {
          
          Serial.println("THE DUNG NHUNG CO CHUYEN DONG!");
          
          beepBuzzer(150); 
          
          lcd.clear();
          lcd.print("THE DUNG/CO DONG");
          lcd.setCursor(0, 1);
          lcd.print("MO CUA BI TU CHOI");
          
          delay(1500);
        }
        
      } else {
        
        Serial.println("TRUY CAP BI TU CHOI!");
        
        beepBuzzer(300);
        
        lcd.clear();
        lcd.print("TRUY CAP BI");
        lcd.setCursor(0, 1);
        lcd.print("TU CHOI! X");
        
        delay(1500);
      }

      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      isLcdDefault = false; 
    }
  }
  
  if (!isLcdDefault) {
    updateDefaultLcd();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastStatusSend >= SEND_STATUS_INTERVAL) {
    lastStatusSend = currentMillis;
    sendStatusToESP();
  }
}