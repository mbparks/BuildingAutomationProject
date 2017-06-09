//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <Servo.h>

static int PWMpin = 2;
static int baffleClosedPosition = 0;
static int baffleOpenPosition = 180;

const char mySSID[] = "yourssid";
const char myPASS[] = "yourwepkey";
const char ubidotsToken[] = "yourtoken";
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
   pinMode(PWMpin, OUTPUT);
   baffleServo.attach(PWMpin);
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
}




//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
void loop() {
    float temperature = 0.0;

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

    while(client.available()){
        String line = client.readStringUntil('\n');
        Serial.print(line);
    }

    operateBaffle(temperature);
    delay(5000);
}



//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
bool operateBaffle(float t) {
    if (t > 75.0) {
        baffleServo.write(baffleOpenPosition);
    }
    else{
        baffleServo.write(baffleClosedPosition);
    }
}