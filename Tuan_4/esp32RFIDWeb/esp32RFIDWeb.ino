#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "webpage.h" 

// Tên WiFi và mật khẩu
const char* ssid = "8C";
const char* password = "123456789";

WebServer server(80);
WebSocketsServer webSocket(81); 

// Chân Serial cho giao tiếp với Arduino Uno
#define RXD2 16
#define TXD2 17

// Biến lưu trữ dữ liệu
float temperature = 0.0;
float humidity = 0.0;
int lightValue = 0;
String rfidStatus = "READY"; 
String lastUID = "N/A";
String servoState = "CLOSED"; 

// Khai báo hàm
void handleRoot();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void parseSerialData(String data);
void sendDataToClient();

void sendDataToClient() {
    String json = "{";
    json += "\"temp\": " + String(temperature, 1);
    json += ",\"humidity\": " + String(humidity, 1);
    json += ",\"light\": " + String(lightValue);
    json += ",\"rfidStatus\": \"" + rfidStatus + "\"";
    json += ",\"lastUID\": \"" + lastUID + "\"";
    json += ",\"servoState\": \"" + servoState + "\"";
    json += "}";
    webSocket.broadcastTXT(json);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            sendDataToClient(); 
            break;
        }
        case WStype_TEXT:
            Serial.printf("[%u] Received Text: %s\n", num, payload);
            String command = String((char*)payload);
            command.trim();
            if (command.equals("open")) {
                Serial1.println("SERVO_OPEN");
            } else if (command.equals("close")) {
                Serial1.println("SERVO_CLOSE");
            } else if (command.equals("buzz")) {
                Serial1.println("BUZZER_ON");
            }
            break;
    }
}

void parseSerialData(String data) {
    Serial.print("Data received from Uno: ");
    Serial.println(data);
    
    if (data.startsWith("T")) {
        int h_index = data.indexOf(",H");
        int l_index = data.indexOf(",L");
        if (h_index > 0 && l_index > 0) {
            temperature = data.substring(1, h_index).toFloat();
            humidity = data.substring(h_index + 2, l_index).toFloat();
            lightValue = data.substring(l_index + 2).toInt();
            Serial.println("[ESP32] Parsed OK: T=" + String(temperature) + ", H=" + String(humidity) + ", L=" + String(lightValue));
        } else {
            Serial.println("[ESP32] Error parsing sensor data format.");
        }
    } else if (data.startsWith("RFID_SCAN:")) {
        lastUID = data.substring(10);
        Serial.println("[ESP32] RFID Scanned: " + lastUID);
    } else if (data.startsWith("STATUS:")) {
        rfidStatus = data.substring(7);
        Serial.println("[ESP32] Status Update: " + rfidStatus);
    } else if (data.equals("SERVO_OPEN_ACK")) {
        servoState = "OPEN";
        Serial.println("[ESP32] Servo Open Acknowledge.");
    } else if (data.equals("SERVO_CLOSE_ACK")) {
        servoState = "CLOSED";
        Serial.println("[ESP32] Servo Close Acknowledge.");
    } else if (data.equals("BUZZER_ACK")) {
        Serial.println("[ESP32] Buzzer Acknowledge.");
    } else if (data.equals("UID_RESET")) {
        lastUID = "N/A";
        rfidStatus = "READY";
        Serial.println("[ESP32] UID and RFID status reset.");
    } else {
        Serial.println("[ESP32] Unknown command or incomplete data.");
    }
    sendDataToClient(); 
}

void readSerial() {
    while (Serial1.available()) {
        String data = Serial1.readStringUntil('\n');
        data.trim();
        if (data.length() > 0) {
            parseSerialData(data);
        }
    }
}

void handleRoot() {
    server.send(200, "text/html", HTML_CONTENT); 
}

void setup() {
    Serial.begin(115200); 
    Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2); 
    delay(100);
    Serial.println("\nConnecting to WiFi...");
    WiFi.begin(ssid, password);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        delay(500);
        Serial.print(".");
        attempt++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        server.on("/", handleRoot);
        server.begin();
        webSocket.begin();
        webSocket.onEvent(webSocketEvent);
        Serial.println("HTTP server started");
        Serial.println("WebSocket server started");
    } else {
        Serial.println("\nWiFi connection failed. Check SSID/Password.");
    }
}

void loop() {
    readSerial();
    if (WiFi.status() == WL_CONNECTED) {
        server.handleClient();
        webSocket.loop();
    }
}