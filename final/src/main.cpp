#include <Arduino.h>

#include <WiFi.h>

const char *ssid = "Je_suis_un_hamster";
const char *password = "baptiste84210";

const char* host = "api.thingspeak.com"; // This should not be changed
const int httpPort = 80; // This should not be changed
const String channelID   = "2993719"; // Change this to your channel ID
const String writeApiKey = "CCKXXI2ECGP3AMFX"; // Change this to your Write API key
const String readApiKey = "YCML69EW76PN3RQU"; // Change this to your Read API key

// The default example accepts one data filed named "temp"
// For your own server you can ofcourse create more of them.
int temp = 0;

int numberOfResults = 3; // Number of results to be read
int fieldNumber = 1; // Field number which will be read out

void setup()
{
    Serial.begin(9600);
    while(!Serial){delay(100);}

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void readResponse(WiFiClient *client){
  unsigned long timeout = millis();
  while(client->available() == 0){
    if(millis() - timeout > 5000){
      Serial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client->available()) {
    String line = client->readStringUntil('\r');
    Serial.print(line);
  }

  Serial.printf("\nClosing connection\n\n");
}

void loop(){
  WiFiClient client;
  String footer = String(" HTTP/1.1\r\n") + "Host: " + String(host) + "\r\n" + "Connection: close\r\n\r\n";

  // WRITE --------------------------------------------------------------------------------------------
  if (!client.connect(host, httpPort)) {
    return;
  }

  client.print("GET /update?api_key=" + writeApiKey + "&field1=" + temp + footer);
  readResponse(&client);

  // READ --------------------------------------------------------------------------------------------

  String readRequest = "GET /channels/" + channelID + "/fields/" + fieldNumber + ".json?results=" + numberOfResults + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Connection: close\r\n\r\n";

  if (!client.connect(host, httpPort)) {
    return;
  }

  client.print(readRequest);
  readResponse(&client);

  // -------------------------------------------------------------------------------------------------

  ++temp;
  delay(10000);
}
