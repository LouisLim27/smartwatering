#define BLYNK_TEMPLATE_ID "TMPL6cTjY7bAv"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"

#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "M5qraoxNLuO8nGSA82icHgef_EIIaCKT"; 
char ssid[] = "Warkop";                      
char pass[] = "rina1688";                         

DHT dht(D3, DHT11); 

BlynkTimer timer;
bool Relay = 0;       
bool autoMode = true; 

#define sensor A0
#define waterPump D4

void setup() {
  Serial.begin(9600);
  Serial.println("Menghubungkan ke Wi-Fi dan Blynk...");
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  if (Blynk.connected()) {
    Serial.println("Berhasil terhubung ke Blynk!");
  } else {
    Serial.println("Gagal terhubung ke Blynk!");
  }

  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, LOW); 
  lcd.init();
  lcd.backlight();

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  
  timer.setInterval(2500L, DHT11sensor); 
  timer.setInterval(2500L, soilMoistureSensor); 
}

BLYNK_WRITE(V1) {
  Relay = param.asInt();
  if (Relay == 1) {
    autoMode = false; 
    digitalWrite(waterPump, HIGH); 
    lcd.setCursor(0, 1);
    lcd.print("Pump ON");
  } else {
    autoMode = true; 
    lcd.setCursor(0, 1);
    lcd.print("Pump OFF");
  }
}

void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  lcd.setCursor(7, 0);
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C  ");  

  lcd.setCursor(9, 1);
  lcd.print("H:");
  lcd.print(h);
  lcd.print("%  ");  

  Blynk.virtualWrite(V3, t);
  Blynk.virtualWrite(V2, h);
}

void soilMoistureSensor() {
    int rawValue = analogRead(sensor);
    int moistureValue = map(rawValue, 340, 640, 0, 100);
    moistureValue = (moistureValue - 100) * -1;

    Blynk.virtualWrite(V0, moistureValue);

   
    lcd.setCursor(0, 0);
    lcd.print("M:");
    lcd.print(moistureValue);
    lcd.print("%  ");  

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    if (autoMode) {
        if (moistureValue < 50 && temperature > 27.0) {  
            digitalWrite(waterPump, HIGH);  
            lcd.setCursor(0, 1);  
            lcd.print("Pump ON ");
            Blynk.virtualWrite(V1, "ON");
        } else {
            digitalWrite(waterPump, LOW);  
            lcd.setCursor(0, 1);  
            lcd.print("Pump OFF");
            Blynk.virtualWrite(V1, "OFF");
        }
    }
}

void loop() {
  Blynk.run(); 
  timer.run(); 
}
