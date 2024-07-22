#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Network credentials
const char* ssid = "Tele2_5EEE46";
const char* password = "sd4s6d7f";

// Create a web server on port 80
WebServer server(80);

// Soil moisture sensor pin
const int soilSensorPin = 2; // GPIO 2 for soil sensor

bool clientConnected = false;

void handleRoot() {
  // Read the soil moisture sensor
  int soilMoistureValue = analogRead(soilSensorPin);
  float moisturePercentage = (soilMoistureValue / 4095.0) * 100; // Convert to percentage

  // Define moisture thresholds
  const int dryThreshold = 400;    // Below this value, soil is dry
  const int wetThreshold = 600;    // Above this value, soil is sufficiently moist

  // Determine moisture status
  String moistureStatus;
  String statusColor;

  if (soilMoistureValue < dryThreshold) {
    moistureStatus = "Needs Watering";
    statusColor = "#ff6f61"; // Red color for dry
  } else if (soilMoistureValue > wetThreshold) {
    moistureStatus = "Sufficiently Moist";
    statusColor = "#4CAF50"; // Green color for sufficient moisture
  } else {
    moistureStatus = "Moderate";
    statusColor = "#FFC107"; // Yellow color for moderate moisture
  }

  // Serve a styled HTML page
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<title>ESP32 Web Server</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4; text-align: center; }";
  html += "h1 { color: #333; }";
  html += "p { font-size: 18px; color: #666; }";
  html += ".container { width: 80%; max-width: 600px; margin: 0 auto; padding: 20px; background: #fff; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }";
  html += ".value { font-size: 24px; font-weight: bold; color: " + statusColor + "; }";
  html += ".status { font-size: 20px; font-weight: bold; color: " + statusColor + "; }";
  html += ".button { background-color: #4CAF50; border: none; color: white; padding: 10px 20px; text-decoration: none; font-size: 16px; cursor: pointer; border-radius: 5px; }";
  html += ".button:hover { background-color: #45a049; }";
  html += "</style></head><body>";
  html += "<div class=\"container\">";
  html += "<h1>ESP32 Web Server</h1>";
  html += "<p>Soil Moisture Percentage:</p>";
  html += "<p class=\"value\">" + String(moisturePercentage, 2) + "%" + "</p>";
  html += "<p class=\"status\">" + moistureStatus + "</p>";
  html += "<button class=\"button\" onclick=\"location.reload();\">Refresh</button>";
  html += "</div>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(soilSensorPin, INPUT); // Set the soil sensor pin as input

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) { // 15 seconds timeout
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi.");
    return; // Exit setup if Wi-Fi is not connected
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Define the handler for the root URL
  server.on("/", handleRoot);

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient(); // Handle incoming client requests

  // Print when a client connects
  if (server.client() && !clientConnected) {
    Serial.println("New client connected.");
    clientConnected = true;
  } else if (!server.client() && clientConnected) {
    Serial.println("Client disconnected.");
    clientConnected = false;
  }
}
