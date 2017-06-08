#include <TEWeatherShield.h>
#include <SparkFunESP8266WiFi.h>

static TEWeatherShield weatherShield;

static int PWMpin = 2;

void setup() {
    pinMode(PWMpin, OUTPUT);
    Serial.begin(9600);
    weatherShield.begin();
    Serial.println("Initializng Mouser Building Automation System...");
}

void loop() {
    measure_ms8607();
}

void measure_ms8607() {
  float temperature;
  float pressure;
  float humidity;
  
  Serial.println("============ MS8607 ============");    
  weatherShield.selectMS8607();

  if (weatherShield.MS8607.is_connected()) {    
    weatherShield.MS8607.set_humidity_i2c_master_mode(ms8607_i2c_hold); 
    weatherShield.MS8607.read_temperature_pressure_humidity( &temperature, &pressure, &humidity);
    
    Serial.print("- Tempeature : ");
    Serial.print(temperature);
    Serial.print((char)176);
    Serial.println("C");
    
    Serial.print("- Pressure : ");
    Serial.print(pressure);
    Serial.println(" hPa");
    
    Serial.print("- Humidity : ");
    Serial.print(humidity);
    Serial.println("%RH");
    Serial.println();
  }
  else {
    Serial.println("MS8607 is disconnected");
  }
}