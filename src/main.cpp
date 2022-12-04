#include <secrets.cpp>
#include <Arduino.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <iostream>

#define ONE_WIRE_BUS 0 // D3 pin of wemos d1 mini

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature.

Adafruit_BME280 bme;

WiFiClient client;

void initWifi(void)
{
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(SECRET_SSID, SECRET_PASS); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}

void initBME280(void)
{
  if (!bme.begin(0x76))
  {
    Serial.println("BME280 Sensor nicht gefunden!");
    while (1)
      ;
  }
}

void setup(void)
{
  Serial.begin(115200);

  sensors.begin();
  initBME280();

  ThingSpeak.begin(client); // Initialize ThingSpeak
}

float round_(float var, int precision)
{
  float value = (int)(var * (10 * precision) + 0.5);
  return (float)value / (10 * precision);
}

void loop(void)
{
  initWifi();

  sensors.requestTemperatures(); // Send the command to get temperatures

  float temp = round_(sensors.getTempCByIndex(0), 1);
  float hum = bme.readHumidity() - CALIBRATION_OFFSET;

  Serial.println("Temperature ist: ");
  Serial.println(temp); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire

  Serial.println("Luftfeuchtigkeit ist: ");
  Serial.println((int)hum);

  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, (int)hum);
  ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);

  delay(30000);
}