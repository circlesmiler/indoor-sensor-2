#include "HTU21D.h"

/*
 * ATTENTION: Use Photon Firmware 2.2.0, not 3.x!!!
 */

#define LED D6

double humidity;
double temp;
double humidityThreshold = 60.0;
int nextRead = 0;

HTU21D htu = HTU21D();

int setTimeZone(String command);
int setHumidityThreshold(String command);

STARTUP(WiFi.selectAntenna(ANT_AUTO));

void setup()
{
  Serial.begin(9600);
  while (!htu.begin())
  {
    Serial.println("HTU21D not found");
    delay(1000);
  }
  Serial.println("HTU21D OK");

  pinMode(LED, OUTPUT);

  Time.zone(2);

  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);
  Particle.function("setHumidityThreshold", setHumidityThreshold);
  Particle.function("setTimeZone", setTimeZone);
}

void loop()
{
  readSensor();
  updateLED();
}

void updateLED()
{
  if (humidity > humidityThreshold)
  {
    digitalWrite(LED, HIGH); // sets the LED on
  }
  else
  {
    digitalWrite(LED, LOW); // sets the LED off
  }
}

void readSensor()
{
  if (nextRead < Time.now())
  {
    Serial.print("\n");
    Serial.print("Retrieving information from sensor: ");
    Serial.print("Read sensor: ");

    double h = htu.readHumidity();
    double t = htu.readTemperature();

    Serial.print("Humidity (%): ");
    Serial.println(h, 2);

    Serial.print("Temperature (oC): ");
    Serial.println(t, 2);

    nextRead = Time.now() + 60;

    humidity = h;
    temp = t;

    Particle.publish("dp_temp", String(temp, 1));
  }
}

// this function automagically gets called upon a matching POST request
int setTimeZone(String timeZone)
{
  int tz = atoi(timeZone);
  Time.zone(tz);
  return tz;
}

int setHumidityThreshold(String command)
{
  int ht = atoi(command);
  humidityThreshold = ht;
  return ht;
}