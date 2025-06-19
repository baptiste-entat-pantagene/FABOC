#include <Arduino.h>

#include <WiFi.h>

#include <DHT.h>
#include <DHT_U.h>

#include <ESP32Servo.h>

#define DHTTYPE DHT11

static bool measure_env(float *temperature, float *humidity);

static const int windowServoPin = 5;
static const int dhtPin = 17;
const int redLedPin = 2;

const char *ssid = "Je_suis_un_hamster";
const char *password = "baptiste84210";

const char *host = "api.thingspeak.com";       // This should not be changed
const int httpPort = 80;                       // This should not be changed
const String channelID = "2993719";            // Change this to your channel ID
const String writeApiKey = "CCKXXI2ECGP3AMFX"; // Change this to your Write API key
const String readApiKey = "YCML69EW76PN3RQU";  // Change this to your Read API key

// The default example accepts one data filed named "temp"
// For your own server you can ofcourse create more of them.
float temp = 0;

int numberOfResults = 3; // Number of results to be read
int fieldNumber = 1;     // Field number which will be read out

Servo window_servo;

DHT dht(dhtPin, DHTTYPE);

void setup()
{
  Serial.begin(9600);

  dht.begin();

  pinMode(redLedPin, OUTPUT);
  window_servo.attach(windowServoPin);
  window_servo.write(0);

  while (!Serial)
  {
    delay(100);
  }

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void readResponse(WiFiClient *client)
{
  unsigned long timeout = millis();
  while (client->available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client->available())
  {
    String line = client->readStringUntil('\r');
    Serial.print(line);
  }

  Serial.printf("\nClosing connection\n\n");
}

void loop()
{
  WiFiClient client;
  String footer = String(" HTTP/1.1\r\n") + "Host: " + String(host) + "\r\n" + "Connection: close\r\n\r\n";

  float temperature;
  float humidity;

  if (measure_env(&temperature, &humidity))
  {

    Serial.print("temperature = ");
    Serial.print(temperature);
    Serial.println(" degree celcius");
    Serial.print("humidity = ");
    Serial.println(humidity);

    if (temperature > 28)
    {

      window_servo.write(90);
      digitalWrite(redLedPin, LOW);
    }
    else if (temperature < 18)
    {

      window_servo.write(0);
      digitalWrite(redLedPin, HIGH);
    }
    else
    {

      window_servo.write(0);
      digitalWrite(redLedPin, LOW);
    }
  }

  temp = temperature;

  // WRITE --------------------------------------------------------------------------------------------
  if (!client.connect(host, httpPort))
  {
    return;
  }

  client.print("GET /update?api_key=" + writeApiKey + "&field1=" + temp + "&field2=" + humidity + footer);
  readResponse(&client);

  delay(5000);
}

static bool measure_env(float *temperature, float *humidity)
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("An issue just happened with the dht sensor");
    return false;
  }

  *temperature = t;
  *humidity = h;
  return true;
}
