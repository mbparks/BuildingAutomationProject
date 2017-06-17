//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <Servo.h>

static int PWMpin = 2;
static int baffleClosedPosition = 90;
static int baffleOpenPosition = 1;
static int loopDelay = 5000;
static float tempSetpoint = 75.0;

const char mySSID[] = "";
const char myPASS[] = "";
const char ubidotsToken[] = "";
const char ubidotsDeviceLabel[] = "Arduino101_MBA";
const char ubidotsTempVarLabel[] = "temperature";
const char destServer[] = "things.ubidots.com";

const String httpRequest1 = "GET /api/v1.6/devices/Arduino101_MBA/temperature/values?page_size=1&token=";
const String httpRequest2 = " HTTP/1.1\r\n"
                            "Host: things.ubidots.com\r\n"
                            "Connection: close\r\n\r\n";

Servo baffleServo;


//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
void setup() {
   
   Serial.begin(115200);
   delay(100);
   
   WiFi.begin(mySSID, myPASS);
   
   Serial.println();
   Serial.println();
   Serial.print("Connecting to ");
   Serial.println(mySSID);
   
   while (WiFi.status() != WL_CONNECTED) {
       delay(500);
       Serial.print(".");
    }

   pinMode(PWMpin, OUTPUT);
   baffleServo.attach(PWMpin);
}




//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
void loop() {
    float temperature = 0.0;
    String line = "";

    Serial.print("connecting to ");
    Serial.println(destServer);
    
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(destServer, httpPort)) {
        Serial.println("connection failed");
        return;
    }
    
    client.print(httpRequest1+ubidotsToken+httpRequest2);

    delay(500);

    while(client.connected()){
        line.concat(client.readStringUntil('\n'));
    }

    baffleServo.attach(PWMpin);
    //Serial.print(line);
    temperature = cleanHttpResponse(line);
    operateBaffle(temperature);
    delay(loopDelay);
    baffleServo.detach();
}



//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
bool operateBaffle(float t) {
    if (t > tempSetpoint) {
        baffleServo.write(baffleOpenPosition);
        Serial.println(F("Baffle is OPEN"));
    }
    else{
        baffleServo.write(baffleClosedPosition);
        Serial.println(F("Baffle is CLOSED"));
    }
}


//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
float cleanHttpResponse(String line) {
    line.trim();
    int valueIndexStart = line.indexOf("\"value\":");
    line.remove(0, valueIndexStart+9);
    //Serial.print("FIRST CUT: ");
    //Serial.println(line);
    valueIndexStart = line.indexOf(",");
    line.remove(valueIndexStart);
    //Serial.print("SECOND CUT: ");
    //Serial.println(line);
    float temperature = line.toFloat();
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println("F");
    return temperature;
}