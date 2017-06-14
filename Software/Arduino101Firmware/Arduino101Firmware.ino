////////////////////////////////////////
//
////////////////////////////////////////
#include <TEWeatherShield.h>
#include <SoftwareSerial.h> 
#include <SparkFunESP8266WiFi.h>

static TEWeatherShield weatherShield;

const char mySSID[] = "";
const char myPASS[] = "";

const char destServer[] = "things.ubidots.com";
const char ubidotsToken[] = "";
const char humidVarId[] = "";
const char tempVarId[] = "";
const char pressVarId[] = "";

const int loopDelay = 30000;



/////////////////////////////////////////
//
////////////////////////////////////////
void setup() {
    Serial.begin(9600);
    delay(1000);
    
    // initializeESP8266() verifies communication with the WiFi
    // shield, and sets it up.
    initializeESP8266();

    // connectESP8266() connects to the defined WiFi network.
    connectESP8266();

    // displayConnectInfo prints the Shield's local IP
    // and the network it's connected to.
    displayConnectInfo();
    
    delay(2000);
    weatherShield.begin();
    delay(2000);
    
    Serial.println(F("Mouser Building Automation System Initialized."));
}



/////////////////////////////////////////
//
////////////////////////////////////////
void loop() {
    ESP8266Client client;
    float temperature = -1, pressure = -1, humidity = -1;
    getMeasurements(&temperature, &pressure, &humidity);
    temperature = (temperature * 1.8) + 32;

    int sizeOfPacket = 159;
    sizeOfPacket += getSize(temperature);
    sizeOfPacket += getSize(pressure);
    sizeOfPacket += getSize(humidity);
  

    if(client.connect(destServer, 80)) {
      Serial.println("Connected to Ubidots server, sending payload..");
      
      client.print("POST /api/v1.6/collections/values/?token=");
      client.print(ubidotsToken);
      client.println(" HTTP/1.1");
      client.println("Host: things.ubidots.com");
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(sizeOfPacket);
      client.println();
      client.print("[{\"variable\": \"");
      client.print(tempVarId);
      client.print("\", \"value\":");
      client.print(temperature);
      client.print("}, {\"variable\": \"");
      client.print(humidVarId);
      client.print("\", \"value\":");
      client.print(humidity);
      client.print("}, {\"variable\": \"");
      client.print(pressVarId);
      client.print("\", \"value\":");
      client.print(pressure);
      client.println("}]");

      Serial.print(F("POST /api/v1.6/collections/values/?token="));
      Serial.print(ubidotsToken);
      Serial.println(F(" HTTP/1.1"));
      Serial.println(F("Host: things.ubidots.com"));
      Serial.println(F("Content-Type: application/json"));
      Serial.print(F("Content-Length: "));
      Serial.println(sizeOfPacket);
      Serial.println();
      Serial.print("[{\"variable\": \"");
      Serial.print(tempVarId);
      Serial.print(F("\", \"value\":"));
      Serial.print(temperature);
      Serial.print(F("}, {\"variable\": \""));
      Serial.print(humidVarId);
      Serial.print(F("\", \"value\":"));
      Serial.print(humidity);
      Serial.print(F("}, {\"variable\": \""));
      Serial.print(pressVarId);
      Serial.print(F("\", \"value\":"));
      Serial.print(pressure);
      Serial.println(F("}]"));

      }
      else {
        Serial.println("Cannot conenct ot to Ubidots server!!!");
      }
      
  // available() will return the number of characters
  // currently in the receive buffer.
  while (client.available()) {
    Serial.write(client.read());
  }// read() gets the FIFO char
  
  // connected() is a boolean return value - 1 if the 
  // connection is active, 0 if it's closed.
  if (client.connected()) {
    client.stop(); // stop() closes a TCP connection.
  }

  delay(loopDelay);
}


/////////////////////////////////////////
//
////////////////////////////////////////
void getMeasurements(float *t, float *p, float *h) {
  weatherShield.selectMS8607();

  while(!weatherShield.MS8607.is_connected()) {
    //wait
  }

  if (weatherShield.MS8607.is_connected()) {    
    weatherShield.MS8607.set_humidity_i2c_master_mode(ms8607_i2c_hold); 
    weatherShield.MS8607.read_temperature_pressure_humidity(t, p, h);
    Serial.println();
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


void initializeESP8266()
{
  // esp8266.begin() verifies that the ESP8266 is operational
  // and sets it up for the rest of the sketch.
  // It returns either true or false -- indicating whether
  // communication was successul or not.
  // true
  int test = esp8266.begin();
  if (test != true)
  {
    Serial.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  Serial.println(F("ESP8266 Shield Present"));
}



void connectESP8266()
{
  // The ESP8266 can be set to one of three modes:
  //  1 - ESP8266_MODE_STA - Station only
  //  2 - ESP8266_MODE_AP - Access point only
  //  3 - ESP8266_MODE_STAAP - Station/AP combo
  // Use esp8266.getMode() to check which mode it's in:
  int retVal = esp8266.getMode();
  if (retVal != ESP8266_MODE_STA)
  { // If it's not in station mode.
    // Use esp8266.setMode([mode]) to set it to a specified
    // mode.
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if (retVal < 0)
    {
      Serial.println(F("Error setting mode."));
      errorLoop(retVal);
    }
  }
  Serial.println(F("Mode set to station"));

  // esp8266.status() indicates the ESP8266's WiFi connect
  // status.
  // A return value of 1 indicates the device is already
  // connected. 0 indicates disconnected. (Negative values
  // equate to communication errors.)
  retVal = esp8266.status();
  if (retVal <= 0)
  {
    Serial.print(F("Connecting to "));
    Serial.println(mySSID);
    // esp8266.connect([ssid], [psk]) connects the ESP8266
    // to a network.
    // On success the connect function returns a value >0
    // On fail, the function will either return:
    //  -1: TIMEOUT - The library has a set 30s timeout
    //  -3: FAIL - Couldn't connect to network.
    retVal = esp8266.connect(mySSID, myPASS);
    if (retVal < 0)
    {
      Serial.println(F("Error connecting"));
      errorLoop(retVal);
    }
  }
}



void displayConnectInfo()
{
  char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  // esp8266.getAP() can be used to check which AP the
  // ESP8266 is connected to. It returns an error code.
  // The connected AP is returned by reference as a parameter.
  int retVal = esp8266.getAP(connectedSSID);
  if (retVal > 0)
  {
    Serial.print(F("Connected to: "));
    Serial.println(connectedSSID);
  }

  // esp8266.localIP returns an IPAddress variable with the
  // ESP8266's current local IP address.
  IPAddress myIP = esp8266.localIP();
  Serial.print(F("My IP: ")); Serial.println(myIP);
}


// errorLoop prints an error code, then loops forever.
void errorLoop(int error)
{
  Serial.print(F("Error: ")); Serial.println(error);
  Serial.println(F("Looping forever."));
  for (;;)
    ;
}



int getSize(int value) {
  int absValue = abs(value);
  int isNegative = 0;
  if (value<0) {
    isNegative = 1;
  }
  if (absValue >= 10000) {
    return 7+isNegative;
  }
  else if (absValue >= 1000) {
    return 6+isNegative;
  }
  else if (absValue >= 100) {
    return 5+isNegative;
  }
  else if (absValue >= 10) {
    return 4+isNegative;
  }
  else {
    return 3+isNegative;
  }
}
