#include <WiFi.h>
#include <WebSocketsServer.h>
#include <WebServer.h>

// Declare secondary Serial pins for ESP32
#define RXD2 16
#define TXD2 17

// Declare WiFi information
const char* ssid = "Quang Vinh";
const char* password = "quangvinh12";

// Create a web server on port 80 to serve the HTML page
WebServer server(80);

// Initialize WebSocket Server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Web page content (HTML, CSS, JS)
const char* HTML_CONTENT = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Haunted Smart Home Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        :root {
            --house-color: #3a3a3a; /* Dark gray for a decaying look */
            --door-color: #1a0000; /* Almost black, rotting wood */
            --roof-color: #4a0000; /* Blood red/deep maroon */
            --porch-color: #2b2b2b; /* Dark, crumbling stone */
            --step-color: #202020; /* Even darker steps */
            --on-color: #00ff00; /* Eerie green light - maybe glow of spirits */
            --off-color: #8b0000; /* Blood red off light */
            --walkway-color: #101010; /* Dark, cracked path */
            --text-color: #e0e0e0; /* Faded light text */
            --dark-red-glow: rgba(139, 0, 0, 0.7); /* Deep red glow for effects */
        }
        body { 
            font-family: 'Creepster', cursive, sans-serif; /* Thêm font kinh dị nếu có */
            text-align: center; 
            margin: 0; 
            background: radial-gradient(circle at 50% 50%, #1a0000, #000000); /* Nền tối, tập trung vào giữa */
            animation: eerieBackground 20s ease infinite alternate; /* Hiệu ứng nền rùng rợn */
            color: var(--text-color);
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            overflow: hidden; /* Prevent scrollbar from fog/particles */
            position: relative; /* For fog/overlay */
        }

        /* Thêm font Creepster từ Google Fonts để tăng cảm giác kinh dị */
        @import url('https://fonts.googleapis.com/css2?family=Creepster&display=swap');

        /* Hiệu ứng nền chuyển động ám ảnh */
        @keyframes eerieBackground {
            0% { background-position: 0% 0%; }
            100% { background-position: 100% 100%; }
        }

        /* Hiệu ứng sương mù/khói nhẹ */
        body::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: 
                radial-gradient(circle at 10% 20%, rgba(50, 50, 50, 0.1) 0%, transparent 50%),
                radial-gradient(circle at 90% 80%, rgba(50, 50, 50, 0.1) 0%, transparent 50%);
            animation: fogEffect 30s linear infinite alternate;
            pointer-events: none; /* Allow interaction with elements below */
            z-index: 0;
        }
        @keyframes fogEffect {
            0% { transform: scale(1) translate(0, 0); opacity: 0.8; }
            50% { transform: scale(1.1) translate(10px, 10px); opacity: 0.9; }
            100% { transform: scale(1) translate(0, 0); opacity: 0.8; }
        }

        h1 {
            color: #ff0000; /* Blood red title */
            text-shadow: 0 0 10px var(--dark-red-glow), 0 0 20px var(--dark-red-glow); /* Glowing red */
            margin-bottom: 20px;
            font-size: 3em; /* Larger, more imposing */
        }
        .house-container {
            display: flex;
            flex-direction: column;
            align-items: center;
            position: relative;
            margin-top: 100px;
        }
        .house-model {
            position: relative;
            width: 300px;
            height: 250px;
            background-color: var(--house-color);
            border: 4px solid #1a1a1a; /* Darker, decaying border */
            border-bottom: none;
            border-top: none;
            border-radius: 5px 5px 0 0;
            box-shadow: 0 5px 15px rgba(0,0,0,0.8), inset 0 0 10px #000; /* Deep shadow, inner darkness */
            z-index: 5;
            transition: transform 0.5s ease; /* Hiệu ứng rung nhẹ */
        }
        .house-model:hover {
            transform: translateY(-5px) rotateZ(0.5deg); /* Rung nhẹ khi di chuột qua */
        }
        .roof {
            width: 0;
            height: 0;
            border-left: 154px solid transparent;
            border-right: 154px solid transparent;
            border-bottom: 100px solid var(--roof-color);
            position: absolute;
            top: -100px;
            left: 50%;
            transform: translateX(-50%);
            z-index: 6;
            filter: drop-shadow(0 0 10px rgba(0,0,0,0.5)); /* Thêm bóng mờ */
        }
        .porch {
            width: 180px;
            height: 15px;
            background-color: var(--porch-color);
            position: absolute;
            bottom: 0px;
            left: 50%;
            transform: translateX(-50%);
            z-index: 4;
            border-radius: 0 0 5px 5px;
            box-shadow: 0 3px 8px rgba(0,0,0,0.6);
        }
        .steps {
            position: absolute;
            bottom: -50px;
            left: 50%;
            transform: translateX(-50%);
            width: 200px;
            height: 45px;
            display: flex;
            flex-direction: column-reverse;
            align-items: center;
            z-index: 3;
            box-shadow: 0 3px 8px rgba(0,0,0,0.6);
        }
        .step {
            width: 100%;
            height: 15px;
            background-color: var(--step-color);
            box-shadow: inset 0 -2px 5px rgba(0,0,0,0.4);
            border-radius: 0 0 3px 3px;
        }
        .step:nth-child(1) { width: 160px; }
        .step:nth-child(2) { width: 180px; }
        .step:nth-child(3) { width: 200px; }
        .door-container {
            width: 100px;
            height: 150px;
            position: absolute;
            bottom: 0;
            left: 50%;
            transform: translateX(-50%);
            perspective: 600px;
            z-index: 6;
        }
        .door {
            width: 80px;
            height: 120px;
            background-color: var(--door-color);
            border: 3px solid #000; /* Solid black border for grim look */
            border-radius: 5px;
            position: absolute;
            bottom: 0;
            left: 50%;
            transform: translateX(-50%);
            transition: transform 0.8s ease;
            transform-origin: left;
            box-shadow: 0 0 10px rgba(0,0,0,0.9), inset 0 0 5px #000; /* Deep shadow, inner darkness */
        }
        .door.open {
            transform: translateX(-50%) rotateY(-110deg);
            box-shadow: -5px 5px 15px rgba(0,0,0,0.7), inset 0 0 5px #000;
        }
        .door-knob {
            width: 15px;
            height: 15px;
            background-color: #5a5a5a; /* Rusty knob */
            border-radius: 50%;
            position: absolute;
            right: 10px;
            top: 50px;
            border: 2px solid #333;
            box-shadow: inset 0 0 5px rgba(0,0,0,0.5); /* Tạo cảm giác cũ kỹ */
        }
        .lights-container {
            position: absolute;
            top: 30px;
            width: 100%;
            display: flex;
            justify-content: space-around;
            z-index: 7;
        }
        .light {
            width: 20px;
            height: 20px;
            background-color: #101010; /* Dark, unlit */
            border-radius: 50%;
            box-shadow: 0 0 3px rgba(0,0,0,0.7);
            transition: all 0.5s ease;
        }
        .light.on-on {
            background-color: var(--on-color);
            box-shadow: 0 0 15px 5px var(--on-color), 0 0 30px 10px var(--on-color); /* Stronger eerie glow */
        }
        .light.off-on {
            background-color: var(--off-color);
            box-shadow: 0 0 15px 5px var(--off-color), 0 0 30px 10px var(--off-color); /* Stronger blood red glow */
        }
        #status {
            font-size: 1.6em;
            font-weight: bold;
            margin-top: 60px;
            color: #ff4500; /* Fiery orange for warning */
            text-shadow: 0 0 8px rgba(255, 69, 0, 0.7); /* Eerie glow */
        }
        button {
            background-color: #6a0000; /* Darker, more ominous button */
            color: #ffd700; /* Golden/rusty text for buttons */
            border: 2px solid #ff0000; /* Red border */
            padding: 15px 30px;
            border-radius: 5px; /* Square buttons */
            font-weight: bold;
            text-transform: uppercase;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 0 10px rgba(255,0,0,0.5), inset 0 0 5px rgba(255,0,0,0.3); /* Red glow effect */
            margin: 0 10px;
            font-family: 'Creepster', cursive, sans-serif;
            font-size: 1.2em;
        }
        button:hover {
            transform: translateY(-3px) scale(1.05); /* Nhấn nhá hơn */
            box-shadow: 0 0 20px rgba(255,0,0,0.8), inset 0 0 10px rgba(255,0,0,0.5);
            background-color: #8b0000; /* Darker red on hover */
        }
        button:active {
            transform: translateY(1px);
        }
        .walkway {
            position: absolute;
            width: 80px;
            height: 120px;
            background-color: var(--walkway-color);
            bottom: 0;
            left: 50%;
            transform: translateX(-50%);
            transition: opacity 0.8s ease;
            opacity: 0;
            z-index: 2;
            border-radius: 0 0 5px 5px;
            box-shadow: inset 0 0 10px #000; /* Làm tối đường đi */
        }
        .walkway.visible {
            opacity: 1;
        }
        #sensorData {
            position: absolute;
            top: 20px;
            left: 20px;
            background-color: rgba(0, 0, 0, 0.6); /* Nền tối, trong suốt */
            padding: 10px 15px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(255, 0, 0, 0.5); /* Viền đỏ kinh dị */
            text-align: left;
            color: #ff6666; /* Màu chữ đỏ nhạt */
            font-family: 'Creepster', cursive, sans-serif;
        }
        #sensorData p {
            margin: 5px 0;
        }
    </style>
</head>
<body>
    <div id="sensorData">
        <p id="temperature">Temperature: -- °C</p>
        <p id="humidity">Humidity: -- %</p>
    </div>
    <h1>The Haunted Mansion</h1>
    <div class="house-container">
        <div class="roof"></div>
        <div class="house-model">
            <div class="lights-container">
                <div class="light" id="greenLight"></div>
                <div class="light" id="redLight"></div>
            </div>
            <div class="walkway" id="walkway"></div>
            <div class="door-container">
                <div class="door" id="door">
                    <div class="door-knob"></div>
                </div>
            </div>
        </div>
        <div class="porch"></div>
        <div class="steps">
            <div class="step"></div>
            <div class="step"></div>
            <div class="step"></div>
        </div>
    </div>
    <p id="status">Status: The spirits are restless...</p>
    <div class="button-group">
        <button onclick="toggleDoor('ON')">Open Crypt Gate</button>
        <button onclick="toggleDoor('OFF')">Seal the Tomb</button>
    </div>
    
    <script>
        var ws;
        var statusElement = document.getElementById('status');
        var doorElement = document.getElementById('door');
        var redLightElement = document.getElementById('redLight');
        var greenLightElement = document.getElementById('greenLight');
        var walkwayElement = document.getElementById('walkway');
        var temperatureElement = document.getElementById('temperature');
        var humidityElement = document.getElementById('humidity');

        function connectWebSocket() {
            ws = new WebSocket("ws://" + window.location.hostname + ":81/");
            ws.onopen = function(evt) {
                statusElement.innerText = "Status: Connected to the other side...";
            };
            ws.onmessage = function(evt) {
                var message = evt.data;
                console.log("Received: " + message);
                updateUI(message);
            };
            ws.onclose = function(evt) {
                statusElement.innerText = "Status: Connection severed...";
                setTimeout(connectWebSocket, 5000); // Thử kết nối lại sau thời gian dài hơn để tạo cảm giác bị cắt đứt
            };
            ws.onerror = function(evt) {
                console.log("Error: " + evt.data);
                statusElement.innerText = "Status: An eerie presence blocks connection...";
            };
        }

        function toggleDoor(command) {
            if (ws.readyState === WebSocket.OPEN) {
                ws.send(command);
                console.log("Sent: " + command);
                updateUI(command);
                // Thêm hiệu ứng âm thanh rùng rợn khi mở/đóng cửa
                // var audio = new Audio('creaky_door_sound.mp3'); // Cần có file âm thanh này
                // audio.play();
            }
        }
        
        function updateUI(status) {
            if (status.includes("ON")) {
                statusElement.innerText = "Status: Crypt Gate Opened, Beware!";
                doorElement.classList.add('open');
                walkwayElement.classList.add('visible');
                greenLightElement.classList.add('on-on');
                redLightElement.classList.remove('off-on');
            } else if (status.includes("OFF")) {
                statusElement.innerText = "Status: Tomb Sealed, Rest in Peace.";
                doorElement.classList.remove('open');
                walkwayElement.classList.remove('visible');
                greenLightElement.classList.remove('on-on');
                redLightElement.classList.add('off-on');
            } else if (status.includes("TEMP:")) {
                var temp = parseFloat(status.split(":")[1]);
                temperatureElement.innerText = "Temperature: " + temp.toFixed(1) + " °C (Chilling...)";
            } else if (status.includes("HUMIDITY:")) {
                var humid = parseFloat(status.split(":")[1]);
                humidityElement.innerText = "Humidity: " + humid.toFixed(1) + " % (Damp and Clammy...)";
            }
        }

        window.onload = function() {
            connectWebSocket();
            // Thêm một âm thanh nền rùng rợn khi trang tải
            // var eerie_music = new Audio('eerie_background_music.mp3'); // Cần có file âm thanh này
            // eerie_music.loop = true;
            // eerie_music.volume = 0.5;
            // eerie_music.play();
        };
    </script>
</body>
</html>
)rawliteral";

// Function Prototype Declaration
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

// Handler for the main web page
void handleRoot() {
  server.send(200, "text/html", HTML_CONTENT);
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up the web server to serve the HTML page
  server.on("/", handleRoot);
  server.begin();

  // Start the WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  server.handleClient();
  
  if (Serial2.available()) {
    String unoStatus = Serial2.readStringUntil('\n');
    unoStatus.trim();
    if (unoStatus == "ON" || unoStatus == "OFF") {
      webSocket.broadcastTXT(unoStatus);
      Serial.print("Received from UNO, broadcasting to web: ");
      Serial.println(unoStatus);
    } else if (unoStatus.startsWith("TEMP:") || unoStatus.startsWith("HUMIDITY:")) {
      webSocket.broadcastTXT(unoStatus);
      Serial.print("Received sensor data, broadcasting to web: ");
      Serial.println(unoStatus);
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      webSocket.sendTXT(num, "Welcome!");
      break;
    }
    case WStype_TEXT:
      String command = String((char*)payload);
      Serial.printf("[%u] Received from client: %s\n", num, command.c_str());
      
      if (command == "ON" || command == "OFF") {
        Serial2.println(command);
        Serial.print("Sending command to UNO: ");
        Serial.println(command);
      }
      break;
  }
}