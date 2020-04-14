// ---++ Mycorhizer - Simple Homegrowing Controller ++--- 
// 
// (by Stas Granin, since April 2020 Global Corona Party)  

#include "DHT.h"
#include "Config.h"
#include "Screen.h"
#include "Device.h"
#include "Button.h"

// Sensor
DHT dht(DHTPIN, DHTTYPE);

// Connected devices
MycoDevice humidifier(HUMIDITY_ID, HUMIDIFIER_PIN);
MycoDevice vent(VENT_ID, VENT_PIN);
MycoDevice light(LIGHT_ID, LIGHT_PIN);
MycoDevice aux(AUX_ID, AUX_PIN);
MycoDevice test(7, AUX_PIN);

// Control buttons
MycoButton screenButton(SCREEN_BUTTON_PIN);
MycoButton humidifierButton(HUMIDIFIER_MANUAL_BUTTON_PIN);
MycoButton ventButton(VENT_MANUAL_BUTTON_PIN);
MycoButton lightButton(LIGHT_MANUAL_BUTTON_PIN);
MycoButton auxButton(AUX_MANUAL_BUTTON_PIN);

// Screen
MycoScreen screen(SCREEN_ADDRESS);


void setup(void) 
{
  byte targetHumidity = TARGET_HUMIDITY;
  unsigned long ventEventTimes[2] = {VENT_TIMING};
  unsigned long lightEventTimes[2] = {LIGHT_TIMING};
  
  byte humidityIcon[8] = {HUMIDITY_ICON};
  byte ventIcon[8] = {LIGHT_ICON};
  byte lightIcon[8] = {VENT_ICON};
  byte auxIcon[8] = {AUX_ICON};
  
  Serial.begin(9600);

  // Initialize the sensor
  dht.begin();

  // Initialize the devices and attach control buttons
  humidifier.init(targetHumidity, RAISING_DEVICE);
  humidifier.attachButton(&humidifierButton); // Accepts pointer to MycoButton object

  vent.init(ventEventTimes);
  vent.attachButton(&ventButton);
  vent.onChangeState(onChangeDeviceState); // No need to run the humidifier while the vent is on. onChangeDeviceState function will handle it

  light.init(lightEventTimes);
  light.attachButton(&lightButton);

  aux.init(auxHandler); // auxHandler function is returning false;
  aux.attachButton(&auxButton);

  test.init(auxHandler);
  aux.attachButton(&auxButton);

  // Initialize the screen
  screen.init();
  screen.attachButton(&screenButton); // Attach a screen control button. Accepts pointer to MycoButton object
  screen.attachDevice(&humidifier, humidityIcon); // Attach a device to screen to show its state. Accepts pointer to MycoDevice and an optional iconBitmap array
  screen.attachDevice(&vent, ventIcon);
  screen.attachDevice(&light, lightIcon);
  screen.attachDevice(&aux, auxIcon);
  //screen.attachDevice(&test);
}

void loop(void) 
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Update all devices states
  humidifier.update(humidity);
  vent.update();
  light.update();
  aux.update();
  test.update();
  
  // Update the screen
  screen.update(temperature, humidity);
}

void onChangeDeviceState(byte deviceId, byte deviceState)
{
  // Serial.println("----------------------------------------");
  // Serial.println("Device state changed to: " + (String)deviceState);
  // Serial.println("----------------------------------------");
  
  (deviceId == VENT_ID && vent.isOn()) ? humidifier.pause() : humidifier.unpause();
}

bool auxHandler(byte deviceId, byte deviceState)
{
  return false;
}
