#include "WebServerManager.h"

WebServerManager::WebServerManager(const char* ap_ssid, const char* ap_password, int port) 
    : server(port), ssid(ap_ssid), password(ap_password) {
    temperature1 = 0.0;
    humidity1 = 0.0;
    temperature2 = 0.0;
    humidity2 = 0.0;
    sensor1Status = false;
    sensor2Status = false;
}

void WebServerManager::begin() {
    Serial.println("Starting WiFi Access Point...");
    
    // Tắt WiFi Station mode trước
    WiFi.mode(WIFI_OFF);
    delay(100);
    
    // Thiết lập WiFi Access Point với cấu hình tối ưu
    WiFi.mode(WIFI_AP);
    delay(100);
    
    // Cấu hình IP tĩnh cho AP
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
    
    // Khởi tạo AP với channel cố định (1) và max 4 connections
    bool result = WiFi.softAP(ssid.c_str(), password.c_str(), 1, 0, 4);
    
    if (result) {
        Serial.println("WiFi AP started successfully");
        Serial.print("SSID: ");
        Serial.println(ssid);
        Serial.print("Password: ");
        Serial.println(password);
        Serial.print("AP IP: ");
        Serial.println(WiFi.softAPIP());
        Serial.print("AP MAC: ");
        Serial.println(WiFi.softAPmacAddress());
        Serial.print("Channel: 1");
        Serial.println();
    } else {
        Serial.println("Failed to start WiFi AP");
        return;
    }
    
    // Đợi một chút để AP ổn định
    delay(1000);
    
    // Thiết lập các route
    Serial.println("Setting up web server routes...");
    server.on("/", [this]() { handleRoot(); });
    server.on("/data", [this]() { handleSensorData(); });
    server.onNotFound([this]() { handleNotFound(); });
    
    server.begin();
    Serial.println("Web server started successfully");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("You can now connect to the WiFi and visit the IP address above");
}

void WebServerManager::handleClient() {
    server.handleClient();
}

void WebServerManager::handleRoot() {
    Serial.println("=== ROOT REQUEST ===");
    Serial.print("Client IP: ");
    Serial.println(server.client().remoteIP());
    Serial.print("Request URI: ");
    Serial.println(server.uri());
    Serial.print("Method: ");
    Serial.println(server.method() == HTTP_GET ? "GET" : "POST");
    
    String html = generateHTML();
    Serial.print("HTML Length: ");
    Serial.println(html.length());
    
    server.send(200, "text/html", html);
    Serial.println("Response sent successfully");
    Serial.println("====================");
}

void WebServerManager::handleSensorData() {
    Serial.println("=== DATA REQUEST ===");
    DynamicJsonDocument doc(512);
    
    doc["temperature1"] = temperature1;
    doc["humidity1"] = humidity1;
    doc["temperature2"] = temperature2;
    doc["humidity2"] = humidity2;
    doc["sensor1Status"] = sensor1Status;
    doc["sensor2Status"] = sensor2Status;
    doc["timestamp"] = millis();
    
    String response;
    serializeJson(doc, response);
    
    Serial.print("JSON Response: ");
    Serial.println(response);
    
    server.send(200, "application/json", response);
    Serial.println("Data response sent");
    Serial.println("===================");
}

void WebServerManager::handleNotFound() {
    Serial.println("404 - Page not found: " + server.uri());
    server.send(404, "text/plain", "Not Found");
}

String WebServerManager::generateHTML() {
    String html = "<html><head><title>DSS Tool</title>";
    html += "<style>";
    html += "body{font-family:Arial;background:#000;color:#fff;padding:20px}";
    html += ".container{max-width:600px;margin:0 auto}";
    html += ".sensor{background:#111;padding:15px;margin:10px;border-radius:5px}";
    html += ".status{color:#0f0}";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<h1>DSS Tool Temp & Fuel</h1>";
    html += "<div class='sensor'>";
    html += "<h3>Sensor 1 (0x44)</h3>";
    html += "<p>Temperature: <span id='t1'>25.0</span>C</p>";
    html += "<p>Humidity: <span id='h1'>60.0</span>%</p>";
    html += "</div>";
    html += "<div class='sensor'>";
    html += "<h3>Sensor 2 (0x45)</h3>";
    html += "<p>Temperature: <span id='t2'>24.5</span>C</p>";
    html += "<p>Humidity: <span id='h2'>65.0</span>%</p>";
    html += "</div>";
    html += "<script>";
    html += "function update(){";
    html += "fetch('/data').then(r=>r.json()).then(d=>{";
    html += "document.getElementById('t1').innerText=d.temperature1.toFixed(1);";
    html += "document.getElementById('h1').innerText=d.humidity1.toFixed(1);";
    html += "document.getElementById('t2').innerText=d.temperature2.toFixed(1);";
    html += "document.getElementById('h2').innerText=d.humidity2.toFixed(1);";
    html += "});} setInterval(update,2000); update();";
    html += "</script>";
    html += "</div></body></html>";
    return html;
}

void WebServerManager::updateSensorData(float temp1, float hum1, bool status1, float temp2, float hum2, bool status2) {
    temperature1 = temp1;
    humidity1 = hum1;
    sensor1Status = status1;
    temperature2 = temp2;
    humidity2 = hum2;
    sensor2Status = status2;
}

bool WebServerManager::isConnected() {
    return WiFi.softAPgetStationNum() > 0;
}

String WebServerManager::getIPAddress() {
    return WiFi.softAPIP().toString();
}

int WebServerManager::getConnectedClients() {
    return WiFi.softAPgetStationNum();
}

void WebServerManager::printWiFiStatus() {
    Serial.println("=== WiFi Status ===");
    Serial.print("AP Status: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? "Active" : "Inactive");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("Connected Clients: ");
    Serial.println(WiFi.softAPgetStationNum());
    Serial.print("Channel: 1");
    Serial.println();
    Serial.print("Signal Strength: Strong (Local AP)");
    Serial.println();
    Serial.println("===================");
}