/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32
31.01.2017 by Jan Hendrik Berlin

 */

#include <WiFi.h>
#include <ESP32Servo.h>
#include "CircularBuffer.hpp"

// declaration

// const
const char *ssid = "Je_suis_un_hamster";
const char *password = "baptiste84210";

// static data
const char *html_plotly_1 = "<script charset=utf-8 src=https://cdn.plot.ly/plotly-3.0.1.min.js></script><div id=tester_";
const char *html_plotly_2 = " style=width:600px;height:250px></div>";
const char *html_plotly_2_2 = "<script>TESTER=document.getElementById(\"tester_";
const char *html_plotly_3 = "\"),Plotly.newPlot(TESTER,[{x:[";
const char *html_plotly_4 = "],y:[";
const char *html_plotly_5 = "]}],{margin:{t:0}})</script>";

const char *html_pump_on = "<div style=display:flex;flex-direction:row><p>Water pump ON</p><div style=width:50px;height:50px;background-color:green></div><a href=\"/PY\" style=\" background-color: green; color: white; padding: 5px 10px; text-decoration: none;\">Turn the pump on.</a><a href=\"/PF\" style=\" background-color: red; color: white; padding: 5px 10px; text-decoration: none;\">Turn the pump off.</a></div>";
const char *html_pump_off = "<div style=display:flex;flex-direction:row><p>Water pump OFF</p><div style=width:50px;height:50px;background-color:red></div><a href=\"/PY\" style=\" background-color: green; color: white; padding: 5px 10px; text-decoration: none;\">Turn the pump on.</a><a href=\"/PF\" style=\" background-color: red; color: white; padding: 5px 10px; text-decoration: none;\">Turn the pump off.</a></div>";

const char *html_window_on = "<div style=display:flex;flex-direction:row><p>Window open</p><div style=width:50px;height:50px;background-color:green></div><a href=\"/FY\" style=\" background-color: green; color: white; padding: 5px 10px; text-decoration: none;\">Open the window.</a><a href=\"/FF\" style=\" background-color: red; color: white; padding: 5px 10px; text-decoration: none;\">Close the window.</a></div>";
const char *html_window_off = "<div style=display:flex;flex-direction:row><p>Window close</p><div style=width:50px;height:50px;background-color:red></div><a href=\"/FY\" style=\" background-color: green; color: white; padding: 5px 10px; text-decoration: none;\">Open the window.</a><a href=\"/FF\" style=\" background-color: red; color: white; padding: 5px 10px; text-decoration: none;\">Close the window.</a></div>";

// end static data

WiFiServer server(80);

Servo window_servo;
const int WINDOW_SERVO_PIN = 17;
bool windowState = false;

const int WATER_PIN = 32;
const int LUX_PIN = 33;

enum dataType
{
  WATER_LEVEL,
  LUX_LEVEL,
  TEMP_LEVEL,
  HUMIDITY_LEVEL
};

struct data
{
  int waterLevel;
  int luxLevel;
  int temp;
  int humidity;
};

const int bufferSize = 30;
CircularBuffer<data, bufferSize> buffer;

const int PUMP_PIN = 19;
bool pumpState = false;

void setup()
{
  Serial.begin(9600);

  // Pin mode
  pinMode(WATER_PIN, INPUT);
  pinMode(LUX_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);

  // Initialize state
  digitalWrite(PUMP_PIN, LOW); // Ensure pump is off at startup
  // SERVO
  window_servo.attach(WINDOW_SERVO_PIN);
  window_servo.write(0);

  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void getData()
{
  // Read the water level and add it to the buffer
  int waterLevel = analogRead(WATER_PIN);
  int luxLevel = analogRead(LUX_PIN);
  buffer.push({waterLevel, luxLevel, 0, 1}); // Assuming temp and humidity are not used, set them to 0 and 1 respectively
}

void plotData(WiFiClient client, dataType type)
{
  client.print(html_plotly_1);
  client.print(type);
  client.print(html_plotly_2);
switch (type)
    {
    case WATER_LEVEL:
      client.print("water_level");
      break;
    case LUX_LEVEL:
      client.print("lux_level");
      break;
    case TEMP_LEVEL:
      client.print("temp_level");
      break;
    case HUMIDITY_LEVEL:
      client.print("humidity_level");
      break;
    default:
      client.print("unknown");
      Serial.println("Unknown data type, defaulting to 0.");
      break;
    }
  client.print(html_plotly_2_2);
  client.print(type);
  client.print(html_plotly_3);

  for (size_t i = 0; i < buffer.size() - 1; i++)
  {
    String dataToAdd = String(i);
    client.print(dataToAdd);
    client.print(",");
  }
  String dataToAdd = String(buffer.size() - 1);
  client.print(dataToAdd);

  client.print(html_plotly_4);
  for (size_t i = 0; i < buffer.size(); i++)
  {
    String dataToAdd;
    switch (type)
    {
    case WATER_LEVEL:
      dataToAdd = String(buffer[i].waterLevel);
      break;
    case LUX_LEVEL:
      dataToAdd = String(buffer[i].luxLevel);
      break;
    case TEMP_LEVEL:
      dataToAdd = String(buffer[i].temp);
      break;
    case HUMIDITY_LEVEL:
      dataToAdd = String(buffer[i].humidity);
      break;
    default:
      dataToAdd = String(0);
      Serial.println("Unknown data type, defaulting to 0.");
      break;
    }

    client.print(dataToAdd);
    client.print(",");
  }

  // Last data
  switch (type)
  {
  case WATER_LEVEL:
    dataToAdd = String(buffer[buffer.size() - 1].waterLevel);
    break;
  case LUX_LEVEL:
    dataToAdd = String(buffer[buffer.size() - 1].luxLevel);
    break;
  case TEMP_LEVEL:
    dataToAdd = String(buffer[buffer.size() - 1].temp);
    break;
  case HUMIDITY_LEVEL:
    dataToAdd = String(buffer[buffer.size() - 1].humidity);
    break;
  default:
    dataToAdd = String(0);
    Serial.println("Unknown data type, defaulting to 0.");
    break;
  }
  client.print(dataToAdd);

  client.print(html_plotly_5);
}

void loop()
{
  WiFiClient client = server.available(); // listen for incoming clients

  if (!client)
  {
    return;
  }

  // if you get a client,
  Serial.println("New Client."); // print a message out the serial port
  String currentLine = "";       // make a String to hold incoming data from the client
  while (client.connected())
  { // loop while the client's connected
    if (client.available())
    {                         // if there's bytes to read from the client,
      char c = client.read(); // read a byte, then
      // Serial.write(c);        // print it out the serial monitor
      if (c == '\n')
      { // if the byte is a newline character

        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0)
        {
          // get the data from the sensor
          getData();

          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Refresh:2;url=/");
          client.println();

          // the content of the HTTP response follows the header:
          client.println("<div style=display:flex;flex-direction:row>");
          plotData(client, WATER_LEVEL);
          plotData(client, LUX_LEVEL);
          client.println("</div>");

          client.println("<div style=display:flex;flex-direction:row>");
          plotData(client, TEMP_LEVEL);
          plotData(client, HUMIDITY_LEVEL);
          client.println("</div>");

          if (pumpState)
          {
            client.print(html_pump_on);
          }
          else
          {
            client.print(html_pump_off);
          }

          if (windowState)
          {
            client.print(html_window_on);
          }
          else
          {
            client.print(html_window_off);
          }

          client.println();
          break;
        }
        else
        {
          currentLine = "";
        }
      }
      else if (c != '\r')
      {
        currentLine += c;
      }

      if (currentLine.endsWith("GET /H"))
      {
        Serial.println("Turning window servo to 0 degrees.");
        window_servo.write(0);
        delay(100);
      }
      if (currentLine.endsWith("GET /L"))
      {
        Serial.println("Turning window servo to 90 degrees.");
        window_servo.write(360);
        delay(100);
      }
      if (currentLine.endsWith("GET /PY"))
      {
        digitalWrite(PUMP_PIN, HIGH);
        pumpState = true;
      }
      if (currentLine.endsWith("GET /PF"))
      {
        digitalWrite(PUMP_PIN, LOW);
        pumpState = false;
      }
      if (currentLine.endsWith("GET /FY"))
      {
        windowState = true;
        window_servo.write(0);
      }
      if (currentLine.endsWith("GET /FF"))
      {
        windowState = false;
        window_servo.write(90);
      }
    }
  }
  // close the connection:
  client.stop();
  Serial.println("Client Disconnected.");
}
