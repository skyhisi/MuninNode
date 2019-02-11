# MuninNode
Munin Node for Arduino with ESP8266 and similar.

Allows an ESP8266 to be queried by Munin.

## Example

```c++
#include <MuninNode.h>

// ...

MySensor sensor;

void temperatureConfig(const char* name, Stream& stream)
{
  MuninNode::println(stream, F("graph_title Temperature"));
  MuninNode::println(stream, F("graph_vlabel Celcius"));
  MuninNode::println(stream, F("temperature.label Temperature"));
}

void temperatureFetch(const char* name, Stream& stream)
{
  MuninNode::println(stream, F("temperature.value"), sensor.readTemperature());
}

void humidityConfig(const char* name, Stream& stream)
{
  MuninNode::println(stream, F("graph_title Humidity"));
  MuninNode::println(stream, F("graph_vlabel %"));
  MuninNode::println(stream, F("humidity.label Humidity"));
}

void humidityFetch(const char* name, Stream& stream)
{
  MuninNode::println(stream, F("humidity.value"), sensor.readHumidity());
}

const static MuninNode::Plugin muninPlugins[] =
{
  {"temp", temperatureConfig, temperatureFetch},
  {"humidity", humidityConfig, humidityFetch},
  {nullptr, nullptr, nullptr}
};

static MuninNode muninNode(muninPlugins);

// ...

void setup()
{
  // Configure sensor
  // ...

  WiFi.hostname("my.hostname");
  // Connect to WiFi
  
  muninNode.begin();
}

void loop()
{
  muninNode.process();
  delay(10);
}

```
