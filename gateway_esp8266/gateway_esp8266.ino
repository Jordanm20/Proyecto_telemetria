#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>

// Declara las credenciales de tu WiFi
const char* ssid = "NETLIFE-Espinoza";
const char* password = "96_Aldamosq";

// Declara tus API keys para ThingSpeak
String apiKeyChannel1 = "KIHRRJXDCZ0PV6QD";
String apiKeyChannel2 = "8IRBZNB8V830D4DF";

WiFiClient client;

RF24 radio(D1, D2);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

struct Data {
  uint8_t nodeID;
  float tempDHT;
  float tempDS18B20;
  float hum;
} __attribute__((packed));

void setup() {
  Serial.begin(9600);
  mesh.setNodeID(0);
  mesh.begin(108);

  // Conexión al WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectándose a WiFi...");
  }
  Serial.println("Conectado al WiFi!");
}

void loop() {
  mesh.update();

  while (network.available()) {
    RF24NetworkHeader header;
    Data data;

    network.read(header, &data, sizeof(data));
    displayData(data, header);
    sendDataToThingSpeak(data, header);
  }

  delay(1000);
}

void displayData(Data data, RF24NetworkHeader header) {
  Serial.println("=====================================");
  Serial.print("Recibido desde: 0");
  Serial.println(header.from_node, OCT);
  Serial.print("           Nodo ID: ");
  Serial.println(data.nodeID);
  Serial.print("  Temperatura DHT: ");
  Serial.print(data.tempDHT);
  Serial.println(" C");
  Serial.print("  Temperatura DS18B20: ");
  Serial.print(data.tempDS18B20);
  Serial.println(" C");
  Serial.print("  Humedad: ");
  Serial.print(data.hum);
  Serial.println(" %");
  Serial.println("=====================================");
}

void sendDataToThingSpeak(Data data, RF24NetworkHeader header) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url;

    if (data.nodeID == 1 || data.nodeID == 2) {
      url = "http://api.thingspeak.com/update?api_key=" + apiKeyChannel1;
      if (data.nodeID == 1) {
        url += "&field1=" + String(header.from_node) +
               "&field2=" + String(data.tempDHT) +
               "&field3=" + String(data.tempDS18B20) +
               "&field4=" + String(data.hum);
      } else if (data.nodeID == 2) {
        url += "&field5=" + String(header.from_node) +
               "&field6=" + String(data.tempDHT) +
               "&field7=" + String(data.tempDS18B20) +
               "&field8=" + String(data.hum);
      }
    } else if (data.nodeID == 3) {
      url = "http://api.thingspeak.com/update?api_key=" + apiKeyChannel2;
      url += "&field1=" + String(header.from_node) +
             "&field2=" + String(data.tempDHT) +
             "&field3=" + String(data.tempDS18B20) +
             "&field4=" + String(data.hum);
    }

    http.begin(client, url);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      Serial.println("Datos enviados correctamente a ThingSpeak!");
    } else {
      Serial.print("Error en el envío: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}
