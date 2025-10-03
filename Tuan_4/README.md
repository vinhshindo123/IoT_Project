## **Hệ Thống An Ninh và Cảm Biến Smart Home**
Dự án Hệ Thống An Ninh và Cảm Biến Smart Home là một giải pháp tự động hóa thông minh, kết hợp giữa phần cứng và phần mềm để giám sát, điều khiển an ninh và môi trường trong nhà. Hệ thống này được xây dựng trên nền tảng vi điều khiển phổ biến, sử dụng các cảm biến và công nghệ kết nối không dây để tạo ra một ngôi nhà an toàn và tiện nghi.

## **Các Tính Năng Chính**
- Kiểm Soát Ra/Vào bằng RFID: Người dùng có thể sử dụng thẻ RFID để mở khóa cửa. Hệ thống sẽ xác thực UID của thẻ, nếu hợp lệ sẽ cho phép truy cập và tự động đóng cửa sau một khoảng thời gian nhất định.
- Điều Khiển Qua Giao Diện Web: Một giao diện web trực quan được phát triển, cho phép người dùng theo dõi trạng thái hệ thống và điều khiển thủ công các thiết bị như đóng/mở cửa (Servo) và kích hoạt còi báo động từ xa qua Wi-Fi.
- Giám Sát Môi Trường: Hệ thống liên tục thu thập dữ liệu từ các cảm biến:
  - Cảm biến DHT11: Đo nhiệt độ và độ ẩm không khí.
  - Cảm biến LDR: Phát hiện cường độ ánh sáng môi trường.
  - Các dữ liệu này được hiển thị theo thời gian thực trên giao diện web và màn hình LCD tại chỗ, giúp người dùng nắm bắt được điều kiện môi trường.
-Hệ Thống Cảnh Báo Thông Minh: Khi một thẻ không hợp lệ được quét, hệ thống sẽ ngay lập tức kích hoạt còi báo động và hiển thị cảnh báo lỗi trên màn hình LCD, đồng thời cập nhật trạng thái lỗi lên giao diện web.

## **Cấu Trúc Phần Cứng**
Dự án sử dụng hai bo mạch vi điều khiển chính để tối ưu hóa hiệu suất và phân chia nhiệm vụ:
- Arduino Uno: Đóng vai trò là "bộ não" xử lý chính. Uno chịu trách nhiệm đọc dữ liệu từ các cảm biến (RFID, DHT11, LDR), điều khiển các thiết bị đầu ra (Servo, còi báo, đèn LED), và hiển thị thông tin lên màn hình LCD.
- ESP32: Đóng vai trò là "bộ xử lý giao tiếp". ESP32 kết nối với Arduino Uno qua giao tiếp Serial, nhận dữ liệu trạng thái và cảm biến. Đồng thời, nó tạo ra một điểm truy cập Wi-Fi để lưu trữ và hiển thị giao diện web, cũng như quản lý kết nối WebSocket để nhận lệnh điều khiển từ người dùng.

## **Hoạt Động**
Dữ liệu được truyền hai chiều giữa Arduino Uno và ESP32:
- Từ Uno đến ESP32: Uno gửi các dữ liệu cảm biến (nhiệt độ, độ ẩm, ánh sáng), trạng thái RFID (thẻ hợp lệ/không hợp lệ), và trạng thái Servo (mở/đóng).
- Từ ESP32 đến Uno: ESP32 nhận lệnh điều khiển từ giao diện web (đóng/mở cửa, bật còi) và gửi các lệnh này tới Uno để thực thi.

## **Kết nối**
![z7076475731966_1e8bdd922e31137e44324bbe955859c0](https://github.com/user-attachments/assets/9b4e61b1-2988-40f2-9020-cfa1186e78a3)


## **Kết quả**
https://drive.google.com/drive/u/0/folders/1P12apuBqbcmFlyhHdIh6TfywlmrqLxuV
