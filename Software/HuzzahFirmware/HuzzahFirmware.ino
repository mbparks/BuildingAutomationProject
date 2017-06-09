#include <ESP8266WiFi.h>
#include <Servo.h>

static int PWMpin = 2;
static int baffleClosedPosition = 0;
static int baffleOpenPosition = 180;

const char mySSID[] = "myssid";
const char myPASS[] = "password";
const char ubidotsToken[] = "yoc4qzLl8qo38xx6vyX5E06VnsIK2Q";
const char ubidotsDeviceLabel[] = "Arduino101_MBA/";
const char ubidotsTempVarLabel[] = "temperature";
const char destServer[] = "things.ubidots.com";

const String httpRequest1 = "GET /api/v1.6/devices/";
const String httpRequest2 = "/values?page_size=1&token=";
const String httpRequest3 = "HTTP/1.1\n"
                            "Host: things.ubidots.com\n";

Servo baffleServo;

float temperature = 0.0;

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

void loop() {
    Serial.print("connecting to ");
    Serial.println(destServer);
    
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(destServer, httpPort)) {
        Serial.println("connection failed");
        return;
    }
    
    client.print(httpRequest1+ubidotsDeviceLabel+ubidotsTempVarLabel+httpRequest2+ubidotsToken+httpRequest3);
    delay(500);

    while(client.available()){
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    client.stop();

    if (temperature > 75.0) {
        baffleServo.write(baffleOpenPosition);
    }
    else{
        baffleServo.write(baffleClosedPosition);
    }



    delay(5000);

}