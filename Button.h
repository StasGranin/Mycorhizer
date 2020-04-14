// Mycorhizer Button Handler
// =========================
// by Stas Granin @ April, 2020
//
// This here helper class is used to create an object to handle various button states.

#include "Arduino.h"
#include "Config.h"

#ifndef MBUTTON_h
#define MBUTTON_h

#define BUTTON_NO_PRESS 0
#define BUTTON_PRESSED 1
#define BUTTON_LONG_PRESSED 2

class MycoButton
{
	typedef void (*callback)(); // A pointer to an event object would be nice (to prevent propagation and such)

	public:
		MycoButton(byte buttonPin);
		void init(void);
		byte getState(void);
		void onPress(callback fn); // Accepts callback function with no attributes
		void onLongPress(callback fn);
		void onReleace(callback fn);
		void onPressing(callback fn);

	private:
		byte _pin;
		bool _isPressed;
		bool _isLongPressed;
		unsigned long _startPressingMillis; // When the buttons got pressed. Used to detect long press
};

#endif