#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

class WebServerManager {
private:
    WebServer server;
    String ssid;
    String password;
    
    // Sensor data storage
    float temperature1, humidity1;
    float temperature2, humidity2;
    bool sensor1Status, sensor2Status;
    
    // Internal methods
    void handleRoot();
    void handleSensorData();
    void handleNotFound();
    String generateHTML();
    
public:
    WebServerManager(const char* ap_ssid, const char* ap_password, int port = 80);
    
    void begin();
    void handleClient();
    void updateSensorData(float temp1, float hum1, bool status1, float temp2, float hum2, bool status2);
    
    bool isConnected();
    String getIPAddress();
    int getConnectedClients();
    void printWiFiStatus();
};

#endif