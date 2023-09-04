//codigo nodo arduinouno
// Incluir las bibliotecas necesarias
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <DHT.h>
#include <OneWire.h> // Biblioteca para DS18B20
#include <DallasTemperature.h> // Biblioteca para DS18B20

// Definir los pines para los sensores
#define DHTPIN 2
#define DHTTYPE DHT22
#define SOIL_MOISTURE_PIN A1
#define DS18B20_PIN 3 // Definir el pin para DS18B20

// Inicializar el sensor DHT11, DS18B20 y el módulo RF24
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
RF24 radio(9, 10); // CE, CSN
RF24Network network(radio);
RF24Mesh mesh(radio, network);

// Estructura para almacenar los datos recolectados por los sensores
struct Data {
  uint8_t nodeID;
  float tempDHT;
  float tempDS18B20; // Temperatura del DS18B20
  float hum;
};

void setup() {
  // Iniciar la comunicación serial
  Serial.begin(9600);
  
  // Iniciar el sensor DHT11 y DS18B20
  dht.begin();
  sensors.begin();
  
  // Configurar el ID del nodo
  mesh.setNodeID(0);
  
  // Iniciar el módulo RF24Mesh con el canal 108
  mesh.begin(108);

  // Inicializar el generador de números aleatorios
  randomSeed(analogRead(0));

  // Asignar dirección al nodo
  mesh.DHCP();
}

void loop() {
  // Actualizar el estado de la malla
  mesh.update();
  sensors.requestTemperatures();
  
  // Leer los datos de los sensores
  Data data;
  data.nodeID = 2;
  data.tempDHT = dht.readTemperature();
  data.tempDS18B20 = sensors.getTempCByIndex(0);
  data.hum = dht.readHumidity();

  // Verificar si el DS18B20 está siendo leído correctamente
  if (data.tempDS18B20 == -127.00) {
    Serial.println("Error: DS18B20 no está siendo leído correctamente.");
  }

  // Mostrar los datos en el monitor serial
  displayData(data);

  // Enviar los datos al gateway
  if (!mesh.write(&data, 'M', sizeof(Data))) {
    Serial.println("Error enviando datos");
  }

  // Esperar 10 segundos antes de leer y enviar datos de nuevo
  delay(1000); // Cambiado a 10 segundos para evitar saturar el monitor serial
}

// Función para mostrar los datos de los sensores
void displayData(Data data) {
  Serial.println("=====================================");
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
