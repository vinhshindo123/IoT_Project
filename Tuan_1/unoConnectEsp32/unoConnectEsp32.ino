const int buttonPin = 4;
int buttonState = 0;
int lastButtonState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP); // Chế độ INPUT_PULLUP
}

void loop() {
  // Đọc trạng thái của nút. Nút được nhấn khi tín hiệu là LOW.
  buttonState = digitalRead(buttonPin);
  
  if (buttonState != lastButtonState) {
    // Kiểm tra nếu nút vừa được nhấn (chuyển từ HIGH xuống LOW)
    if (buttonState == LOW) {
      Serial.println("PRESSED");
    }
    delay(50);
  }
  
  lastButtonState = buttonState;
}