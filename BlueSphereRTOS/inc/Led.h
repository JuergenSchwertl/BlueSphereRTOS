#pragma once

#ifndef __LED_CLASS_H
#define __LED_CLASS_H 

#include <os_hal_gpio.h>


class Led {
	private :
		os_hal_gpio_pin gpioPin;
		os_hal_gpio_data pgioState;

		os_hal_gpio_data set(os_hal_gpio_data gpioData);

	public:
		Led(os_hal_gpio_pin gpioPin);
		// no dtor needed

		os_hal_gpio_data Toggle();
};


#endif // __LED_CLASS_H