const char HTML_CONTENT[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hệ Thống An Ninh và Cảm Biến</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <style>
        body { font-family: 'Inter', sans-serif; background-color: #f7fafc; }
        @keyframes flashRed { 0%, 100% { opacity: 1; } 50% { opacity: 0.3; } }
        @keyframes flashGreen { 0%, 100% { opacity: 1; } 50% { opacity: 0.5; } }
        .red-flash { animation: flashRed 0.3s ease-in-out infinite; }
        .green-flash { animation: flashGreen 0.5s ease-in-out infinite; }
        @keyframes pulse { 0% { transform: scale(1); } 50% { transform: scale(1.1); } 100% { transform: scale(1); } }
        .buzzer-active {
            animation: pulse 0.5s infinite;
            box-shadow: 0 0 10px #ff9900, 0 0 20px #ffaa00;
        }
        .door-panel {
            transition: transform 0.7s ease-in-out, opacity 0.7s;
            transform-origin: left center;
            transform: perspective(600px) rotateY(0deg) translateX(0);
        }
        .door-open {
            transform: perspective(600px) rotateY(-90deg) translateX(-100%);
            opacity: 0.8;
        }
    </style>
</head>
<body class="p-4 bg-gray-100 min-h-screen">
    <div class="max-w-4xl mx-auto">
        <h1 class="text-3xl font-bold text-center text-gray-800 mb-6 border-b-2 pb-2">
            HỆ THỐNG AN NINH & CẢM BIẾN SMART HOME
        </h1>
        <div class="bg-white p-6 rounded-xl shadow-lg mb-6">
            <h2 class="text-xl font-semibold mb-4 text-gray-700">Trạng Thái Hệ Thống</h2>
            <div id="status-panel" class="p-3 rounded-lg text-center font-bold text-white transition-all duration-300 shadow-md bg-yellow-500">
                <span id="current-status-text">Đang Khởi Động...</span>
            </div>
            <p class="mt-4 text-sm text-gray-500">UID Quét Gần Nhất: <span id="last-uid" class="font-mono text-gray-700 bg-gray-200 px-2 py-1 rounded">N/A</span></p>
        </div>
        <div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 mb-6">
            <div class="bg-blue-600 text-white p-5 rounded-xl shadow-lg flex flex-col items-center">
                <svg class="w-8 h-8 mb-2" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10 2a4 4 0 014 4v7a4 4 0 01-8 0V6a4 4 0 014-4zM7 16a5 5 0 0010 0M5 16h14"></path></svg>
                <span class="text-xs opacity-75">Nhiệt Độ</span>
                <span id="temp-display" class="text-4xl font-extrabold mt-1">--.- °C</span>
            </div>
            <div class="bg-teal-500 text-white p-5 rounded-xl shadow-lg flex flex-col items-center">
                <svg class="w-8 h-8 mb-2" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 15a4 4 0 004 4h9a5 5 0 10-.1-9.9m-2.29-1.39l.64-.813m2.07-2.73l.64-.813m2.07 2.73l.64-.813"></path></svg>
                <span class="text-xs opacity-75">Độ Ẩm</span>
                <span id="humid-display" class="text-4xl font-extrabold mt-1">--.- %</span>
            </div>
            <div class="bg-yellow-400 text-gray-900 p-5 rounded-xl shadow-lg flex flex-col items-center">
                <svg class="w-8 h-8 mb-2" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z"></path></svg>
                <span class="text-xs opacity-75">Ánh Sáng (LDR Raw)</span>
                <span id="light-display" class="text-4xl font-extrabold mt-1">----</span>
            </div>
        </div>
        <div class="bg-white p-6 rounded-xl shadow-lg">
            <h2 class="text-xl font-semibold mb-4 text-gray-700">Mô Phỏng và Điều Khiển</h2>
            <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                <div class="p-4 bg-gray-50 rounded-lg border border-gray-200">
                    <h3 class="font-bold text-lg mb-4 border-b pb-2">Tình Trạng Thiết Bị Thực Tế</h3>
                    <div class="mb-6 p-4 bg-white rounded-xl shadow-md">
                        <p class="font-medium text-lg mb-3">Cửa Ra Vào (Servo): <span id="door-status-text" class="font-extrabold text-red-500">Đóng</span></p>
                        <div class="flex items-end h-32 border-4 border-gray-800 bg-gray-900 rounded-lg overflow-hidden relative perspective-simulation">
                            <div class="absolute inset-0 flex justify-center items-center">
                                <span class="text-sm text-gray-400 font-mono">FRAME</span>
                            </div>
                            <div id="door-simulation" class="door-panel absolute left-0 h-full w-1/2 bg-yellow-700 shadow-2xl border-r-8 border-yellow-900 flex justify-center items-center">
                                <span class="text-lg font-bold text-white">CỬA</span>
                            </div>
                        </div>
                    </div>
                    <div class="flex justify-around items-center p-4 bg-white rounded-xl shadow-md">
                        <div class="text-center">
                            <div id="led-red" class="w-10 h-10 rounded-full bg-gray-300 border-2 border-red-800 transition-all duration-300 shadow-inner"></div>
                            <span class="text-xs mt-1 block font-semibold text-gray-600">Lỗi (Đỏ)</span>
                        </div>
                        <div class="text-center">
                            <div id="led-yellow" class="w-10 h-10 rounded-full bg-yellow-400 border-2 border-yellow-800 shadow-lg shadow-yellow-200 transition-all duration-300"></div>
                            <span class="text-xs mt-1 block font-semibold text-gray-600">Chờ (Vàng)</span>
                        </div>
                        <div class="text-center">
                            <div id="led-green" class="w-10 h-10 rounded-full bg-gray-300 border-2 border-green-800 transition-all duration-300 shadow-inner"></div>
                            <span class="text-xs mt-1 block font-semibold text-gray-600">OK (Xanh)</span>
                        </div>
                        <div class="text-center">
                            <div id="buzzer-simulation" class="w-10 h-10 rounded-full bg-gray-400 border-4 border-gray-600 flex items-center justify-center transition-all duration-300">
                                <svg class="w-6 h-6 text-gray-800" fill="currentColor" viewBox="0 0 20 20" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-8-3a1 1 0 00-.893.553L6.12 11.23a2 2 0 00-.148 1.488l.056.284a1 1 0 00.957.734h7.93a1 1 0 00.957-.734l.056-.284a2 2 0 00-.148-1.488l-3-4.678A1 1 0 0010 7z" clip-rule="evenodd"></path></svg>
                            </div>
                            <span class="text-xs mt-1 block font-semibold text-gray-600">Còi Báo</span>
                        </div>
                    </div>
                </div>
                <div class="p-4 bg-gray-50 rounded-lg border border-gray-200 flex flex-col">
                    <h3 class="font-bold text-lg mb-4 border-b pb-2">Điều Khiển Thủ Công (Web)</h3>
                    <div class="flex flex-col space-y-3 mb-6">
                        <button onclick="sendCommand('open')" class="bg-green-600 hover:bg-green-700 text-white font-bold py-3 px-4 rounded-lg shadow-xl transition duration-300 transform hover:scale-105">
                            <i class="fas fa-door-open mr-2"></i> MỞ CỬA BẰNG WEB
                        </button>
                        <button onclick="sendCommand('close')" class="bg-red-600 hover:bg-red-700 text-white font-bold py-3 px-4 rounded-lg shadow-xl transition duration-300 transform hover:scale-105">
                            <i class="fas fa-door-closed mr-2"></i> ĐÓNG CỬA BẰNG WEB
                        </button>
                        <button onclick="sendCommand('buzz')" class="bg-orange-600 hover:bg-orange-700 text-white font-bold py-3 px-4 rounded-lg shadow-xl transition duration-300 transform hover:scale-105">
                            <i class="fas fa-bell mr-2"></i> KÍCH HOẠT CÒI CẢNH BÁO
                        </button>
                    </div>
                    <h3 class="font-bold text-lg mt-auto mb-3 border-t pt-2">Trạng Thái Ánh Sáng Môi Trường</h3>
                    <div id="light-environment" class="p-4 text-center rounded-xl font-bold text-white transition-all duration-500 bg-gray-800 shadow-inner">
                        <span id="light-level-text" class="text-lg">Đang Đọc...</span>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <script>
        document.head.innerHTML += '<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css">';
        const ELEMENTS = {
            temp: document.getElementById('temp-display'),
            humid: document.getElementById('humid-display'),
            light: document.getElementById('light-display'),
            statusPanel: document.getElementById('status-panel'),
            statusText: document.getElementById('current-status-text'),
            lastUID: document.getElementById('last-uid'),
            doorSim: document.getElementById('door-simulation'),
            doorText: document.getElementById('door-status-text'),
            ledRed: document.getElementById('led-red'),
            ledYellow: document.getElementById('led-yellow'),
            ledGreen: document.getElementById('led-green'),
            buzzerSim: document.getElementById('buzzer-simulation'),
            lightEnv: document.getElementById('light-environment'),
            lightText: document.getElementById('light-level-text')
        };
        
        // Cập nhật lại logic reset LED để nó reset về trạng thái READY (vàng)
        function resetLeds() {
            ELEMENTS.ledRed.classList.remove("bg-red-500", "shadow-lg", "red-flash");
            ELEMENTS.ledRed.classList.add("bg-gray-300", "shadow-inner");
            ELEMENTS.ledYellow.classList.remove("bg-yellow-400", "shadow-lg", "shadow-yellow-200");
            ELEMENTS.ledYellow.classList.add("bg-gray-300", "shadow-inner");
            ELEMENTS.ledGreen.classList.remove("bg-green-500", "shadow-lg", "green-flash");
            ELEMENTS.ledGreen.classList.add("bg-gray-300", "shadow-inner");
        }

        function updateUI(data) {
            ELEMENTS.temp.textContent = data.temp !== undefined ? data.temp.toFixed(1) + " °C" : "--.- °C";
            ELEMENTS.humid.textContent = data.humidity !== undefined ? data.humidity.toFixed(1) + " %" : "--.- %";
            ELEMENTS.light.textContent = data.light !== undefined ? data.light : "----";
            ELEMENTS.lastUID.textContent = data.lastUID || "N/A";

            const lightVal = parseInt(data.light);
            if (!isNaN(lightVal)) {
                if (lightVal < 300) {
                    ELEMENTS.lightEnv.className = "p-4 text-center rounded-xl font-bold text-white transition-all duration-500 bg-gray-800 shadow-lg shadow-gray-700";
                    ELEMENTS.lightText.textContent = "MÔI TRƯỜNG TỐI/THIẾU ÁNH SÁNG";
                } else {
                    ELEMENTS.lightEnv.className = "p-4 text-center rounded-xl font-bold text-white transition-all duration-500 bg-yellow-500 shadow-lg shadow-yellow-300";
                    ELEMENTS.lightText.textContent = "MÔI TRƯỜNG ĐỦ ÁNH SÁNG";
                }
            } else {
                ELEMENTS.lightEnv.className = "p-4 text-center rounded-xl font-bold text-white transition-all duration-500 bg-gray-400";
                ELEMENTS.lightText.textContent = "ĐANG ĐỌC DỮ LIỆU...";
            }
            
            resetLeds();
            ELEMENTS.buzzerSim.classList.remove('buzzer-active');
            ELEMENTS.statusPanel.className = "p-3 rounded-lg text-center font-bold text-white transition-all duration-300 shadow-md";

            if (data.rfidStatus === "GRANTED") {
                ELEMENTS.statusText.textContent = "TRUY CẬP THÀNH CÔNG! (Shindo Xin Chao)";
                ELEMENTS.statusPanel.classList.add("bg-green-600");
                ELEMENTS.ledGreen.classList.remove("bg-gray-300", "shadow-inner");
                ELEMENTS.ledGreen.classList.add("bg-green-500", "shadow-lg", "green-flash");
            } else if (data.rfidStatus === "DENIED") {
                ELEMENTS.statusText.textContent = "TRUY CẬP BỊ TỪ CHỐI! (Thẻ không hợp lệ)";
                ELEMENTS.statusPanel.classList.add("bg-red-600");
                ELEMENTS.ledRed.classList.remove("bg-gray-300", "shadow-inner");
                ELEMENTS.ledRed.classList.add("bg-red-500", "shadow-lg", "red-flash");
                ELEMENTS.buzzerSim.classList.add('buzzer-active');
            } else {
                ELEMENTS.statusText.textContent = "HỆ THỐNG SẴN SÀNG (Chờ Quét Thẻ)";
                ELEMENTS.statusPanel.classList.add("bg-yellow-500");
                ELEMENTS.ledYellow.classList.remove("bg-gray-300", "shadow-inner");
                ELEMENTS.ledYellow.classList.add("bg-yellow-400", "shadow-lg", "shadow-yellow-200");
            }
            
            if (data.servoState === "OPEN") {
                ELEMENTS.doorSim.classList.add('door-open');
                ELEMENTS.doorText.textContent = "Mở";
                ELEMENTS.doorText.classList.remove('text-red-600');
                ELEMENTS.doorText.classList.add('text-green-600');
            } else { // CLOSED
                ELEMENTS.doorSim.classList.remove('door-open');
                ELEMENTS.doorText.textContent = "Đóng";
                ELEMENTS.doorText.classList.remove('text-green-600');
                ELEMENTS.doorText.classList.add('text-red-600');
            }
        }

        const ws = new WebSocket(`ws://${window.location.hostname}:81/ws`);
        ws.onmessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                updateUI(data);
            } catch (e) {
                console.error('Failed to parse JSON:', e);
            }
        };

        ws.onopen = () => {
            console.log('Connected to WebSocket server');
        };

        ws.onclose = () => {
            console.log('Disconnected from WebSocket server');
        };

        ws.onerror = (error) => {
            console.error('WebSocket Error:', error);
        };
        
        function sendCommand(cmd) {
            if (ws.readyState === WebSocket.OPEN) {
                ws.send(cmd);
                console.log(`Command sent: ${cmd}`);
            } else {
                console.error("WebSocket not connected.");
            }
        }
    </script>
</body>
</html>
)rawliteral";