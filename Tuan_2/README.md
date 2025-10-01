## **Tóm tắt dự án**
Dự án này là một hệ thống Nhà thông minh với chủ đề "Ngôi nhà ma ám," được xây dựng trên nền tảng IoT. Hệ thống sử dụng kiến trúc hai vi điều khiển, trong đó một ESP32 đóng vai trò là cổng giao tiếp Wi-Fi và máy chủ web, còn một Arduino Uno chịu trách nhiệm điều khiển các cảm biến và thiết bị vật lý. Người dùng có thể điều khiển cánh cửa "ma ám" từ xa và theo dõi các thông số môi trường (nhiệt độ, độ ẩm) thông qua một trang web được thiết kế riêng với giao diện rùng rợn.

## **Các tính năng chính**
1. Kiến trúc hai vi điều khiển: Dự án sử dụng mô hình giao tiếp master-slave để phân chia công việc:
- ESP32: Hoạt động như "bộ não" IoT, lưu trữ và cung cấp trang web, đồng thời sử dụng WebSockets để giao tiếp theo thời gian thực với trình duyệt. Nó nhận dữ liệu cảm biến từ Arduino Uno và gửi lệnh điều khiển từ trang web đến Uno.
- Arduino Uno: Đóng vai trò là "cơ thể" của hệ thống, kết nối trực tiếp với các cảm biến DHT11, động cơ servo, đèn LED và còi báo. Nó đọc dữ liệu, điều khiển các thiết bị vật lý và giao tiếp với ESP32 qua giao tiếp Serial.
2. Giao diện web độc đáo: Trang web được thiết kế riêng với chủ đề "Ngôi nhà ma ám" mang lại trải nghiệm độc đáo.
- Hiệu ứng thị giác: Giao diện có màu sắc u ám, một mô hình ngôi nhà giả tưởng, đèn báo sáng mờ ảo (màu đỏ cho "đóng", màu xanh cho "mở"), cùng với hiệu ứng nền rùng rợn.
- Điều khiển người dùng: Các nút "Open Crypt Gate" (Mở Cổng Hầm Mộ) và "Seal the Tomb" (Niêm Phong Lăng Mộ) cho phép người dùng điều khiển hệ thống.
- Thông tin thời gian thực: Kết nối WebSocket đảm bảo trạng thái cửa và dữ liệu cảm biến (nhiệt độ và độ ẩm) được cập nhật liên tục trên trang web với các thông điệp mang tính chất "ma quái."
3. Điều khiển và phản hồi tự động: Arduino Uno xử lý tất cả các logic vật lý:
- Điều khiển cửa: Động cơ servo mô phỏng "cánh cửa hầm mộ," quay 180° khi mở và trở về 0° khi đóng.
- Chỉ thị trạng thái: Đèn LED đỏ báo hiệu cửa đóng, trong khi đèn LED xanh báo hiệu cửa mở. Một còi báo sẽ phát ra âm thanh khi cửa được mở.
- Kiểm soát cục bộ: Hệ thống cũng có thể được điều khiển trực tiếp bằng một nút nhấn trên Arduino Uno. Khi nhấn, nó sẽ chuyển đổi trạng thái cửa và gửi cập nhật lên ESP32.
- Giám sát cảm biến: Cảm biến DHT11 trên Uno định kỳ đọc nhiệt độ và độ ẩm. Uno sau đó gửi dữ liệu này đến ESP32 để hiển thị theo thời gian thực trên trang web.

## **Sơ đồ mạch điện**
![z7047988422374_d8adabf749b9364e63603fb15be679f5](https://github.com/user-attachments/assets/c8914299-c2c2-4b26-8fb0-8e3cd1b07349)
![z7047988431307_0e1d84f08d27f9c96b6648fdfc81af7b](https://github.com/user-attachments/assets/ab0bed47-0080-464e-a1e8-8593fa8d6689)

## **Video kết quả**
<div style="text-align: center;">
  <video src="https://github.com/user-attachments/assets/9b3758ee-9040-4a98-8e8d-f9f5cb3b86ee" controls style="width: 80%; max-width: 600px;">
    Trình duyệt của bạn không hỗ trợ thẻ video.
  </video>
</div>




