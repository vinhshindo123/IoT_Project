## **Tóm tắt dự án**
Đây là một hệ thống Giám sát và Điều khiển Môi trường IoT tích hợp. Arduino Uno xử lý việc đọc dữ liệu từ cảm biến nhiệt độ/độ ẩm DHT11 và điều khiển các thiết bị vật lý (LED, Quạt), bao gồm cả logic tự động bật quạt. ESP32 đóng vai trò là cầu nối Wi-Fi, sử dụng nền tảng Blynk để cung cấp giao diện người dùng từ xa (ứng dụng di động). Hệ thống đảm bảo đồng bộ hóa trạng thái hai chiều giữa ứng dụng Blynk, nút nhấn vật lý và các thiết bị trên Uno.

## **Cơ chế hoạt động**
1. Arduino Uno (Điều khiển Vật lý và Tự động hóa)
Arduino Uno là phần cứng thực hiện các thao tác vật lý. Nó giao tiếp với ESP32 qua Serial (RX/TX).
- Đầu vào:
  - Cảm biến DHT11 (Chân 3): Đọc nhiệt độ và độ ẩm.
  - Nút nhấn vật lý (Chân 9): Dùng để bật/tắt đèn LED cục bộ, được xử lý chống dội phím (debounce).
- Đầu ra:
  - Đèn LED (Chân 7): Được điều khiển từ ứng dụng Blynk hoặc nút nhấn vật lý.
  - Quạt/Rơ le (Chân 8): Được điều khiển tự động dựa trên nhiệt độ.
- Giao tiếp (Serial): Uno gửi và nhận các lệnh có tiền tố:
  - Nhận lệnh từ ESP32:
    - L[0/1]: Điều khiển trạng thái LED (L1 bật, L0 tắt).
    - T[giá trị]: Đặt Ngưỡng nhiệt độ (tempThreshold).
  - Gửi dữ liệu đến ESP32:
    - C[nhiệt độ]: Dữ liệu nhiệt độ hiện tại.
    - H[độ ẩm]: Dữ liệu độ ẩm hiện tại.
    - L[0/1]: Trạng thái LED sau khi người dùng nhấn nút vật lý (để đồng bộ lên Blynk).
- Logic Tự động: Hàm readAndSendDHT() tự động kiểm tra: nếu nhiệt độ (currentTemp) vượt quá ngưỡng (tempThreshold), quạt (FAN) sẽ tự động được BẬT (HIGH).

2. ESP32 (Cổng Wi-Fi và Blynk)
ESP32 là cầu nối Internet, chịu trách nhiệm kết nối với Wi-Fi và máy chủ Blynk. Nó giao tiếp với Uno qua Serial2.
- Kết nối: Sử dụng thư viện BlynkSimpleEsp32.h để kết nối với nền tảng Blynk.
- Ứng dụng Blynk -> Uno:
  - V0 (Nút ảo): Nhận lệnh từ ứng dụng và gửi lệnh L[0/1] qua Serial2 đến Uno để bật/tắt đèn.
  - V3 (Thanh trượt): Nhận giá trị ngưỡng nhiệt độ từ ứng dụng và gửi lệnh T[giá trị] qua Serial2 đến Uno để cập nhật tempThreshold.
- Uno -> Ứng dụng Blynk:
  - Hàm readDataFromUno() liên tục kiểm tra dữ liệu từ Uno.
  - Dữ liệu C (Nhiệt độ) được cập nhật lên V1 (Gauge/Graph).
  - Dữ liệu H (Độ ẩm) được cập nhật lên V2 (Gauge/Graph).
  - Lệnh L (từ nút vật lý) được sử dụng để đồng bộ hóa trạng thái của nút ảo V0 trên ứng dụng, đảm bảo ứng dụng luôn hiển thị đúng trạng thái đèn LED hiện tại.

## Hình ảnh kết quả
## Video kết quả
https://github.com/user-attachments/assets/32f15df6-64de-4a73-9cc1-d372cb3bcd8f
