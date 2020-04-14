// Mycorhizer Device

#include "Arduino.h"
#include "Config.h"
#include "Button.h"

#ifndef MDEVICE_h
#define MDEVICE_h

// Device states
#define DEVICE_STATE_OFF 0
#define DEVICE_STATE_ON 1
#define DEVICE_STATE_ON_PAUSED 2
#define DEVICE_STATE_MANUAL_OFF 3
#define DEVICE_STATE_MANUAL_ON 4

// Device types
#define UNKNOWN_DEVICE 0 // This device will never update
#define RAISING_DEVICE 1 // i.e. humidifier, heater
#define LOWERING_DEVICE 2 // i.e. chiller, water level alarm
#define TIMED_DEVICE 3 // i.e. vent, light
#define AUX_DEVICE 4 // i.e. anything that can be controlled by a mosfet

class MycoDevice
{
	typedef void (*callback)(byte, byte); // onChangeState callback function(deviceId, deviceState).
	typedef bool (*handler)(byte, byte); // Aux device handler function(deviceId, deviceState). Return true/false to control device;
	
	public:
		MycoDevice(byte deviceId, byte devicePin);
		void init(long targetValue, byte deviceType); // Initialize as value affected device
		void init(unsigned long eventTimes[MAX_DEVICE_EVENTS]); // Initialize as timed device
		void init(handler fn); // Initialize as aux device
		void attachButton(MycoButton *button); // Connect control button
		void onChangeState(callback fn); // ascepts a callback function to be executed every time the device changes state
		void update(long value); // Update the device with new sensor value
		void update(void); // Same as the above but without the value (for timed devices and such)
		void on(void); // Set output pin HIGH, _isOn = true
		void off(void); // Oposite
		void pause(void); // Turns the device off and "freezes" any 
		void unpause(void);
		bool isOn(void);
		byte getDeviceId(void);
		byte getState(void);

	private:
		MycoButton *_button;
		byte _outputPin;
		byte _id;
		byte _deviceState;
		byte _lastControledState;
		bool _isOn;
		bool _isPaused;
		bool _isManual;
		byte _deviceType;
		handler _auxHandler;
		callback _onStateChangeCallback;
		unsigned long _eventTimes[MAX_DEVICE_EVENTS]; // array of seconds [on, off, on, off, ...]
		byte _numOfEvents;
		byte _currentTimingIndex;
		unsigned long _eventStartMillis;	
		unsigned long _lastUpdateMillis; // Used to calsulate time that passed since the last update	
		long _targetValue;

		void _init(void);
		void _update(long value);
		void _handleButton(void);
		void _handleTimeDependant(void);
		void _handleValueDependant(long value);
		void _handleAuxDependant();		
};

#endif