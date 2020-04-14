// Mycorhizer Configuration

#ifndef MCONFIG_h
#define MCONFIG_h

// Growing conditions
#define TARGET_TEMPERATURE 26 // Degrees celcius
#define TARGET_HUMIDITY 95 // Relative humidity
//#define VENT_TIMING 300, 3600 // 5 minutes on, 1 hour off
#define VENT_TIMING 1, 10 // 5 minutes on, 1 hour off
#define LIGHT_TIMING 43200, 43200 // {on, off} in seconds (12 hours on, 12 hours off)

// App configuration
#define APP_DEBUG false
#define HUMIDITY_ID 1 // This id is used to connect between independent components (i.e. Device to Screen icons) 
#define VENT_ID 2
#define LIGHT_ID 3
#define AUX_ID 4
#define MAX_DEVICES 4 // Maximum number of devices that can be connected
#define SCREEN_REFRESH_INERVAL 100 // Limit the writes to the screen. Does not need to be high
#define GLOBAL_BLINK_INTERVAL 500 // Icon blinking speed
#define LONG_BUTTON_PRESS_DURATION 1000 // How long should the be considered a long button press
#define MAX_DEVICE_EVENTS 2 // Sets the size of the VENT_TIMER_TASKS and LIGHT_TIMER_TASKS size (up to 10)
#define TEMPERATURE_VALUES_ARRAY_SIZE 5 // Saves the last 5 values to get the average over time (~10 seconds)
#define HUMIDITY_VALUES_ARRAY_SIZE 5 

// Sensor
#define DHTTYPE DHT22 // Sensor type
#define DHTPIN 6 // Sensor pin

// Device output pin numbers
#define HUMIDIFIER_PIN 2
#define VENT_PIN 3
#define LIGHT_PIN 4
#define AUX_PIN 5

// Device control buttons pin numbers
#define SCREEN_BUTTON_PIN 8
#define HUMIDIFIER_MANUAL_BUTTON_PIN 9
#define VENT_MANUAL_BUTTON_PIN 10
#define LIGHT_MANUAL_BUTTON_PIN 11
#define AUX_MANUAL_BUTTON_PIN 12

// Screen configuration
#define SCREEN_ADDRESS 0x3f
#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 2

// Device icons
#define HUMIDITY_ICON 0x00,0x0A,0x0A,0x0E,0x0A,0x0A,0x00,0x00
#define LIGHT_ICON 0x00,0x08,0x08,0x08,0x08,0x0E,0x00,0x00
#define VENT_ICON 0x00,0x0A,0x0A,0x0A,0x0A,0x04,0x00,0x00
#define AUX_ICON 0x00,0x04,0x0A,0x0A,0x0E,0x0A,0x00,0x00

#endif