#pragma once
#ifndef __BLINKING_LED_CLASS_H
#define __BLINKING_LED_CLASS_H

#include <os_hal_gpio.h>
#include <tx_api.h>
#include "Led.h"

#define BLINK_FLAG 0x01

class BlinkingLed : public Led {
private:
	TX_EVENT_FLAGS_GROUP    m_blink_event_flags;

public:
	BlinkingLed(os_hal_gpio_pin gpioPin);
	// no dtor needed

	void RaiseBlinkEvent();
	void BlinkThreadHandler();
};


#endif // __BLINKING_LED_CLASS_H