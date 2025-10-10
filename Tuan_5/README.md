# **🏠 Dự án: Hệ thống Tự động hóa Gia đình (IoT Serial Gateway)**
Dự án này là một giải pháp Nhà thông minh (Smart Home) sử dụng kiến trúc IoT Gateway lai, kết hợp sức mạnh xử lý và kết nối của ESP32 với khả năng điều khiển thiết bị ngoại vi của Arduino UNO thông qua giao thức Serial. Hệ thống cho phép người dùng giám sát và điều khiển nhiều khu vực chức năng trong nhà (Ngoại vi/Gara, Phòng khách, Bếp) từ xa thông qua giao thức MQTT (Message Queuing Telemetry Transport).

# **🌐 Giới thiệu chi tiết từng Khu vực chức năng**
## **I. Khu vực bếp**
Khu vực bếp ưu tiên các chức năng an toàn và điều khiển thiết bị.
_Khu vực này hiện đang được cập nhật (Bùi Quang Trường)_

## **II. Khu vực phòng khách**
_Khu vực này hiện đnag được cập nhật (Phạm Thành Vinh)_
Khu vực này có vai trò chính là Giám sát Môi trường và Tự động hóa Ánh sáng dựa trên điều kiện thực tế, đồng thời cung cấp dữ liệu cho LCD của khu vực Ngoại vi.
- Giám sát Môi trường	Cảm biến DHT (Nhiệt độ/Độ ẩm)	Publish Trạng thái: home/livingroom/temp_hum
- Tự động hóa Ánh sáng	Cảm biến LDR & LED (Đèn phòng khách)	Logic: Nếu LDR phát hiện trời tối, tự động BẬT LED.
![z7101049203239_93c47b674206f36a75f09cef33e63ae0](https://github.com/user-attachments/assets/8eb1872a-e0c4-41ef-8063-da9d92009e23)


## **III. Khu vực sân và gara**
## **🛠️ Cấu trúc Hệ thống**
Hệ thống được chia thành hai module chính giao tiếp với nhau qua cổng Serial (UART):
1. Module Điều khiển (Arduino UNO)
- Chức năng: Nhận lệnh từ ESP32, đọc cảm biến, và điều khiển các thiết bị vật lý.
- Thiết bị:
  - RFID (MFRC522): Quản lý truy cập vào cửa nhà.
  - 2 x Servo Motors: Điều khiển Cửa Nhà (Khóa/Mở) và Cửa Gara.
  - IR Sensor: Phát hiện chuyển động (thường đặt ở Gara).
  - LDR: Cảm biến ánh sáng (điều khiển đèn tự động).
  - LCD I2C: Hiển thị trạng thái hệ thống và dữ liệu nhiệt độ/độ ẩm.
  - Còi (Buzzer): Cảnh báo hoặc báo hiệu truy cập.
  - Đèn led: Bật sáng khi cảm biến ánh sáng phát hiện trời tối
2. Module Gateway (ESP32)
- Chức năng: Kết nối mạng Wi-Fi, kết nối với MQTT Broker, và chuyển tiếp dữ liệu/lệnh giữa MQTT và Arduino UNO qua Serial2.

## **⚙️ Hoạt động và Giao tiếp**
1. Giao tiếp Serial (ESP32 ↔ UNO)
MQTT → ESP32 → UNO	Dữ liệu Cảm biến	T25.5,H60.0	Cập nhật giá trị T/H từ cảm biến ngoại vi (nếu có, VD: DHT).
MQTT → ESP32 → UNO	Lệnh Điều khiển	HOME_DOOR:ON / GARA_DOOR:OFF / GARA_LIGHT:ON	Điều khiển Servo và LED từ xa.
UNO → ESP32 → MQTT	Trạng thái Hệ thống	L:OFF,D:LOCK,G:LOCK,M:NONE	Gửi trạng thái của Đèn (L), Cửa Nhà (D), Cửa Gara (G), Chuyển động (M) lên Cloud.
2. Các Topic MQTT Chính
home/door/set	📥 Subscribe	Lệnh điều khiển Cửa Nhà (Servo 1)	ON, OFF
home/garage/door/set	📥 Subscribe	Lệnh điều khiển Cửa Gara (Servo 2)	ON, OFF
home/garage/light/set	📥 Subscribe	Lệnh điều khiển Đèn Gara (LED)	ON, OFF
home/livingroom/temp_hum	📥 Subscribe	Nhận dữ liệu T/H (nếu có)	T25.5,H60.0
home/door/status	📤 Publish	Trạng thái hiện tại của Cửa Nhà	LOCK, UNLOCK
home/garage/door/status	📤 Publish	Trạng thái hiện tại của Cửa Gara	LOCK, UNLOCK
home/garage/light/status	📤 Publish	Trạng thái hiện tại của Đèn Gara	ON, OFF
home/garage/motion	📤 Publish	Trạng thái Cảm biến chuyển động	DETECT, NONE

## **🔑 Hướng dẫn Cài đặt**
1. Cấu hình Arduino UNO (Module Điều khiển)
- Cài đặt Thư viện: MFRC522.h, LiquidCrystal_I2C.h, Servo.h.
- Upload Code: Upload mã Arduino UNO lên board UNO.
- Lưu ý: Thay đổi các validCards (UID thẻ RFID) theo nhu cầu của bạn.
2. Cấu hình ESP32 (Module Gateway)
- Cài đặt Thư viện: WiFi.h, PubSubClient.h.
- Cập nhật Thông tin Kết nối:
- Sửa ssid và password thành thông tin Wi-Fi của bạn.
- Sửa mqtt_server (hiện đang là test.mosquitto.org) nếu bạn sử dụng Broker khác.
- Upload Code: Upload mã ESP32 lên board ESP32.
3. Kết nối Phần cứng
- Đảm bảo các chân Serial (UART) của hai board được kết nối chéo:
- ESP32 TX2 (GPIO 17) → Arduino UNO RX (D0)
- ESP32 RX2 (GPIO 16) → Arduino UNO TX (D1)
* Lưu ý: Bạn cần sử dụng Serial2 trên ESP32 vì Serial0 thường được sử dụng cho Debugging (Monitor). Trên Arduino UNO, Serial mặc định là Serial0.

## **Kết nối phần cứng**
![z7101049195808_21f48937fcbb861948663ea1a2b8ab77](https://github.com/user-attachments/assets/7aaa707f-0928-422c-af84-3e0a774dac34)

## **Kêt quả thử nghiệm**
https://drive.google.com/file/d/1OvX6dNr4AffGOfNbIfc5BcnocNuGKVtu/view?usp=drive_link
