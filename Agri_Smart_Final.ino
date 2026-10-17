/* =========================================================================================
 * ESP32 Smart Agriculture - 3.3V/5V Relay Voltage Hack & Dashboard Sync
 * ========================================================================================= */

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid     = "TestNet";     // YOUR WIFI NAME
const char* password = "12345678";    // YOUR WIFI PASSWORD

#define SOIL_PIN        34  
#define DHTPIN          4   
#define DHTTYPE         DHT11
#define LDR_PIN         35  
#define WATER_LEVEL_PIN 32  
#define RELAY_PIN       26  

const int SOIL_DRY_VAL = 3200;
const int SOIL_WET_VAL = 1400;
const int WATER_MIN_VAL = 100;
const int WATER_MAX_VAL = 2500;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); 
WebServer server(80);

bool pumpStatus = false;
unsigned long lastLcdUpdate = 0;
int lcdPage = 0; 

void handleCors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(204);
}

void handleSensorData() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  int rawSoil = analogRead(SOIL_PIN);
  int soilPercent = constrain(map(rawSoil, SOIL_DRY_VAL, SOIL_WET_VAL, 0, 100), 0, 100);

  int rawLdr = analogRead(LDR_PIN);
  int lightPercent = constrain(map(rawLdr, 4095, 0, 0, 100), 0, 100);
  
  int rawWater = analogRead(WATER_LEVEL_PIN);
  int waterPercent = constrain(map(rawWater, WATER_MIN_VAL, WATER_MAX_VAL, 0, 100), 0, 100);

  if (isnan(temp)) temp = 28.5;
  if (isnan(hum)) hum = 68.0;

  String json = "{";
  json += "\"temperature\":" + String(temp, 1) + ",";
  json += "\"humidity\":" + String(hum, 0) + ",";
  json += "\"soil\":" + String(soilPercent) + ",";
  json += "\"light\":" + String(lightPercent) + ",";
  json += "\"waterLevel\":" + String(waterPercent) + ",";
  json += "\"pump\":" + String(pumpStatus ? "true" : "false");
  json += "}";

  server.send(200, "application/json", json);
}

// =========================================================================
// 🔥 THE SPECIAL RELAY VOLTAGE HACK FOR THE DASHBOARD BUTTONS
// =========================================================================

void handlePumpOn() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  pumpStatus = true;
  // To turn ON, we set the pin to OUTPUT and pull it to ground (0V)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
  server.send(200, "application/json", "{\"status\":\"success\",\"pump\":true}");
}

void handlePumpOff() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  pumpStatus = false;
  // To turn OFF, we change the pin to INPUT (High-Impedance). 
  // This physically disconnects the signal, forcing the 5V relay to turn off!
  pinMode(RELAY_PIN, INPUT); 
  server.send(200, "application/json", "{\"status\":\"success\",\"pump\":false}");
}
// =========================================================================

void updateLcdDisplay() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int rawSoil = analogRead(SOIL_PIN);
  int soilPercent = constrain(map(rawSoil, SOIL_DRY_VAL, SOIL_WET_VAL, 0, 100), 0, 100);
  int rawLdr = analogRead(LDR_PIN);
  int lightPercent = constrain(map(rawLdr, 4095, 0, 0, 100), 0, 100);
  int rawWater = analogRead(WATER_LEVEL_PIN);
  int waterPercent = constrain(map(rawWater, WATER_MIN_VAL, WATER_MAX_VAL, 0, 100), 0, 100);
  
  if (isnan(temp)) temp = 0.0;
  if (isnan(hum)) hum = 0.0;

  lcd.clear();
  
  if (lcdPage == 0) {
    lcd.setCursor(0, 0); lcd.print("Temp: "); lcd.print(temp, 1); lcd.print(" C");
    lcd.setCursor(0, 1); lcd.print("Hum:  "); lcd.print((int)hum); lcd.print(" %");
  } 
  else if (lcdPage == 1) {
    lcd.setCursor(0, 0); lcd.print("Soil:"); lcd.print(soilPercent); lcd.print("% L:"); lcd.print(lightPercent); lcd.print("%");
    lcd.setCursor(0, 1); lcd.print("Tank:"); lcd.print(waterPercent); lcd.print("% P:"); lcd.print(pumpStatus ? "ON " : "OFF");
  }
  
  lcdPage++; if (lcdPage > 1) lcdPage = 0; 
}

void setup() {
  Serial.begin(115200);

  // START WITH PUMP OFF USING THE HACK
  pinMode(RELAY_PIN, INPUT); 

  dht.begin();
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0); lcd.print("   AgriSmart    ");
  lcd.setCursor(0, 1); lcd.print("    Welcome!    ");

  Serial.println("\n\n========================================");
  Serial.println("🌱 ESP32 Smart Agriculture Node Starting");
  Serial.println("========================================");
  
  Serial.print("📡 WiFi Connecting to: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) { 
    delay(500); Serial.print("."); retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected Successfully!");
    Serial.println("==================================================");
    Serial.print("🎯 COPY THIS IP TO PASTE IN DASHBOARD: ");
    Serial.println(WiFi.localIP());
    Serial.println("==================================================");
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
  }

  delay(2000); 

  server.on("/sensor", HTTP_GET, handleSensorData);
  server.on("/pump/on", HTTP_GET, handlePumpOn);
  server.on("/pump/off", HTTP_GET, handlePumpOff);
  server.on("/sensor", HTTP_OPTIONS, handleCors);
  server.on("/pump/on", HTTP_OPTIONS, handleCors);
  server.on("/pump/off", HTTP_OPTIONS, handleCors);
  server.begin();
}

void loop() {
  server.handleClient(); 
  
  if (millis() - lastLcdUpdate > 3000) {
    lastLcdUpdate = millis();
    updateLcdDisplay();
  }
}