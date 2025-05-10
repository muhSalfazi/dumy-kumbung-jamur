#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Galaxy";
const char* password = "#pohobanget";

// MQTT Broker
const char* mqtt_server = "192.168.244.254"; 
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("✅ WiFi terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop hingga terhubung ke MQTT
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT broker...");
    if (client.connect("ESP32Client")) {
      Serial.println("✅ Terhubung!");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Membuat data dummy random
  StaticJsonDocument<256> doc;
  doc["temperature"] = random(200, 350) / 10.0;       // 20.0°C - 35.0°C
  doc["humidity"] = random(400, 700) / 10.0;           // 40% - 70%
  doc["gas"] = random(100, 500);                      // gas concentration ppm
  doc["o2"] = random(1800, 2100) / 100.0;              // 18% - 21%
  doc["soilMoisture"] = random(300, 800) / 10.0;       // 30% - 80%
  doc["soilPH"] = random(500, 800) / 100.0;            // 5.0 - 8.0 pH
  doc["soilTemp"] = random(150, 300) / 10.0;           // 15°C - 30°C
  doc["soilConductivity"] = random(100, 2000);         // µS/cm
  
  char buffer[256];
  size_t n = serializeJson(doc, buffer);

  // Publish ke topic sensor/data
  client.publish("sensor/data", buffer);
  
  Serial.println("📤 Data dummy terkirim:");
  Serial.println(buffer);

  delay(5000); // Kirim setiap 5 detik
}
