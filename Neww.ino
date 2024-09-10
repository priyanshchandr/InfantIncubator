#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711_ADC.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);


const char* ssid = " ";    // Replace with your WiFi network name (SSID)
const char* password = " ";  // Replace with yo

// Pin definitions
const int fanRelayPin = D6;       // Relay for fan
const int peltierRelayPin = D5;   // Relay for Peltier
const int dhtPin = D1;            // DHT sensor
const int loadCellDoutPin = D4;   // HX711 load cell DOUT pin
const int loadCellSckPin = D3;    // HX711 load cell SCK pin
float given_mass=0;

HX711_ADC LoadCell(loadCellDoutPin, loadCellSckPin);
const int calVal_eepromAdress = 0;

float calibrateLoadCell() {
  LoadCell.begin();
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);

  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    Serial.println("Timeout, check MCU > HX711 wiring and pin designations");
    return 0.0;
  } else {
    LoadCell.setCalFactor(1.0);
  }

  LoadCell.update();
  LoadCell.tareNoDelay();
  
  Serial.println("Place the load cell on a level stable surface. Remove any load.");
  Serial.println("Send 't' from the serial monitor to set the tare offset.");

  while (LoadCell.getTareStatus() == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      if (Serial.read() == 't') {
        LoadCell.tareNoDelay();
      }
    }
  }

  Serial.println("Place the known mass on the load cell. Then send its weight from the serial monitor.");

  float known_mass = 0;
  while (known_mass == 0) {
    LoadCell.update();
    known_mass = Serial.parseFloat();
    given_mass = known_mass;

  }

  LoadCell.refreshDataSet();
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass);

  Serial.print("New calibration value: ");
  Serial.println(newCalibrationValue);
  return newCalibrationValue;
}



// DHT sensor
DHT dht(dhtPin, DHT11);

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address may vary, change if needed

void setup() {
  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  Serial.println("Starting calibration...");
  float calibrationValue = calibrateLoadCell();
  Serial.print("Calibration value: ");
  Serial.println(calibrationValue);
  calibrationValue = calibrationValue;
  LoadCell.setCalFactor(calibrationValue);


  // Initialize relay pins
  pinMode(fanRelayPin, OUTPUT);
  pinMode(peltierRelayPin, OUTPUT);

  // Initialize load cell
  // LoadCell.begin(loadCellDoutPin, loadCellSckPin);
  LoadCell.start(2000); // Stabilization time of 2 seconds

  // Initialize LCD
  lcd.init();
  lcd.backlight();



  // Initialize DHT sensor
  dht.begin();

  // Display initial message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Turn off relay
  digitalWrite(fanRelayPin, LOW);
  digitalWrite(peltierRelayPin, LOW);

  server.on("/temperature", HTTP_GET, []() {
    server.send(200, "text/plain", temperature); // Replace with your temperature value
  });

  server.on("/weight", HTTP_GET, []() {
    server.send(200, "text/plain", weight); // Replace with your weight value
  });

  server.on("/humidity", HTTP_GET, []() {
    server.send(200, "text/plain", humidity); // Replace with your humidity value
  });

  server.begin();
}

void loop() {

  server.handleClient();

  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read weight from load cell
  LoadCell.update();
  float weight = LoadCell.getData();
  weight = weight + given_mass;
  if (weight<0)
  {
    weight=0;
  }

  // Print data to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println("g");

  // Update data on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.print("Hum:");
  lcd.print(humidity);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Weight: ");
  lcd.print(weight);
  lcd.print("gm");

  // Control the fan and Peltier based on temperature
  if (temperature > 38) {
    digitalWrite(fanRelayPin, HIGH);  // Turn on the fan
    digitalWrite(peltierRelayPin, LOW); // Turn off the Peltier
    // delay(5000);
  } else if (temperature < 35) {
    digitalWrite(fanRelayPin, LOW);    // Turn off the fan
    digitalWrite(peltierRelayPin, HIGH); // Turn on the Peltier
    // delay(5000);
  } else {
    digitalWrite(fanRelayPin, LOW);    // Turn off the fan
    digitalWrite(peltierRelayPin, LOW); // Turn off the Peltier
  }

  delay(1000);  // Delay for 5 seconds (adjust as needed)
}



#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "your-ssid";         // Replace with your Wi-Fi network name
const char* password = "your-password"; // Replace with your Wi-Fi network password

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Define API endpoints
  server.on("/temperature", HTTP_GET, []() {
    server.send(200, "text/plain", "23.5"); // Replace with your temperature value
  });

  server.on("/weight", HTTP_GET, []() {
    server.send(200, "text/plain", "3.2"); // Replace with your weight value
  });

  server.on("/humidity", HTTP_GET, []() {
    server.send(200, "text/plain", "55.0"); // Replace with your humidity value
  });

  server.begin();
}

