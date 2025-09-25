// Khai báo các chân Serial phụ cho ESP32
#define RXD2 16
#define TXD2 17

// Khai báo chân LED và chân chuông trên ESP32
const int ledPin = 4;
const int buzzerPin = 5;

// Khai báo biến để lưu trạng thái hiện tại của đèn LED
int ledState = LOW;

void setup() {
  // Khởi tạo Serial mặc định để giao tiếp với máy tính
  Serial.begin(9600); 
  // Khởi tạo Serial phụ để giao tiếp với UNO
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 

  // Thiết lập chân LED và chân chuông là OUTPUT
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  Serial.println("ESP32 đã sẵn sàng.");
}

void loop() {
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n'); 
    receivedData.trim(); // Loại bỏ khoảng trắng thừa

    Serial.print("ESP32 nhận: ");
    Serial.println(receivedData);

    // Kiểm tra nếu tín hiệu nhận được là "PRESSED"
    if (receivedData.equals("PRESSED")) {
      // Phát âm thanh từ chuông
      tone(buzzerPin, 1000, 200); // Tần số 1000Hz trong 200ms
      
      // Đảo ngược trạng thái của đèn LED
      ledState = !ledState;
      Serial.print("Trang thai led: ");
      Serial.println(ledState);
      
      // Cập nhật trạng thái đèn LED
      digitalWrite(ledPin, ledState);

      // In ra trạng thái hiện tại của đèn để gỡ lỗi
      if (ledState == HIGH) {
        Serial.println("Đèn đã bật!");
        tone(buzzerPin, 1000); 
      } else {
        Serial.println("Đèn đã tắt!");
        noTone(buzzerPin);

      }
    }
  }
}