#define BLYNK_TEMPLATE_ID "TMPL6TQvU94Dp"
#define BLYNK_TEMPLATE_NAME "TestBlink"
#define BLYNK_AUTH_TOKEN "sqaZSKcCP8STZd6iOZWTo89WbIixsPus"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Declare secondary Serial pins for ESP32
#define RXD2 16
#define TXD2 17

char ssid[] = "Quang Vinh";
char pass[] = "quangvinh12";

BlynkTimer timer;

// V0: Nút ấn từ app -> Uno
BLYNK_WRITE(V0) {
  int pinValue = param.asInt();
  // Gửi lệnh 'L' (LED) và giá trị trạng thái qua Serial
  Serial2.print("L"); 
  Serial2.println(pinValue);
  Serial.print("Blynk app -> Uno: L");
  Serial.println(pinValue);
}

// V3: Slider từ app -> Uno
BLYNK_WRITE(V3) {
  int pinValue = param.asInt();
  // Gửi lệnh 'T' (Temperature Threshold) và giá trị ngưỡng
  Serial2.print("T");
  Serial2.println(pinValue);
  Serial.print("Blynk app -> Uno: T");
  Serial.println(pinValue);
}

// Hàm đọc dữ liệu từ Uno và gửi lên Blynk
void readDataFromUno() {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    Serial.print("Received from Uno: ");
    Serial.println(data);

    char type = data.charAt(0);
    String value = data.substring(1);

    if (type == 'C') { // Nhiệt độ
      Blynk.virtualWrite(V1, value.toFloat());
    } else if (type == 'H') { // Độ ẩm
      Blynk.virtualWrite(V2, value.toFloat());
    } else if (type == 'L') { // Nút vật lý
      int ledState = value.toInt();
      // Đồng bộ trạng thái lên app
      Blynk.virtualWrite(V0, ledState);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Khởi tạo Serial2 để giao tiếp với Uno
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  while (!Blynk.connected()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nBlynk connected.");
  
  // Đọc dữ liệu từ Uno sau mỗi 100ms
  timer.setInterval(100L, readDataFromUno);
}

void loop() {
  Blynk.run();
  timer.run();
}