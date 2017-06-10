/////////////////////////////////////////
//
////////////////////////////////////////
#include <TEWeatherShield.h>
#include <SoftwareSerial.h> 
#include <SparkFunESP8266WiFi.h>

static TEWeatherShield weatherShield;
ESP8266Client client;

static int PWMpin = 2;
const char mySSID[] = "myssid";
const char myPASS[] = "password";
const char ubidotsToken[] = "yoc4qzLl8qo38xx6vyX5E06VnsIK2Q";
const char ubidotsDeviceName[] = "Arduino101_MBA";
const char destServer[] = "things.ubidots.com";

const String httpRequest1 = "POST /api/v1.6/devices/Arduino101_MBA?token=";
const String httpRequest2 = "HTTP/1.1\n"
                            "Content-Type: application/json\n"
                            "Host: things.ubidots.com\n"
                            "Content-Length: ";

/////////////////////////////////////////
//
////////////////////////////////////////
void setup() {
    pinMode(PWMpin, OUTPUT);
    weatherShield.begin();

    Serial.begin(9600);

    while (esp8266.begin() != true)
    {
      Serial.print("Error connecting to ESP8266.");
	    delay(1000);
    }

    if (esp8266.status() <= 0)
    {

      int retVal;
      retVal = esp8266.connect("mySSID", "myPASS");
      if (retVal < 0)
      {
        Serial.print(F("Error connecting: "));
        Serial.println(retVal);
      }
    }
    delay(1000);
    Serial.println(F("Initializng Mouser Building Automation System..."));
}



/////////////////////////////////////////
//
////////////////////////////////////////
void loop() {
    float temperature, pressure, humidity;
    getMeasurements(&temperature, &pressure, &humidity);

    String tempPacket = "{\"temperature\": {\"value\":"+String(temperature)+"}, ";
    String pressurePacket = "\"pressure\": {\"value\":"+String(pressure)+"}, ";
    String humidityPacket = "\"humidity\": {\"value\":"+String(humidity)+"}}";
    String packet = tempPacket+pressurePacket+humidityPacket;

    int retVal = client.connect(destServer, 80);
    if (retVal <= 0)
    {
      Serial.println(F("Failed to connect to server."));
      return;
    }

  client.print(httpRequest1+ubidotsToken+httpRequest2);
  client.print(String(sizeof(packet)));
  client.print(packet);
  

  while (client.available()) {
    Serial.write(client.read()); // read() gets the FIFO char
  }

  if (client.connected()) {
    client.stop(); // stop() closes a TCP connection.
  }

  delay(5000);
}


/////////////////////////////////////////
//
////////////////////////////////////////
void getMeasurements(float *t, float *p, float *h) {
  weatherShield.selectMS8607();

  if (weatherShield.MS8607.is_connected()) {    
    weatherShield.MS8607.set_humidity_i2c_master_mode(ms8607_i2c_hold); 
    weatherShield.MS8607.read_temperature_pressure_humidity(t, p, h);
    
    Serial.print(F("Temperature : "));
    Serial.print(*t);
    Serial.println(F("C"));

    Serial.print(F("- Pressure : "));
    Serial.print(*p);
    Serial.println(F(" hPa"));
    
    Serial.print(F("- Humidity : "));
    Serial.print(*h);
    Serial.println(F("%RH"));
    Serial.println();
  }

  else {
    Serial.println(F("MS8607 is disconnected"));
  }
}