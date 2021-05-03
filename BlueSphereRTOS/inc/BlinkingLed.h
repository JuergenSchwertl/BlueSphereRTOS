#pragma once
#ifndef __BLINKING_LED_CLASS_H
#define __BLINKING_LED_CLASS_H

#include <os_hal_gpio.h>
#include <os_hal_gpt.h>

#include <tx_api.h>
#include "Led.h"

#define BLINK_FLAG 0x01

class BlinkingLed : public Led {
private:
	TX_EVENT_FLAGS_GROUP    m_blink_event_flags;
	
	/* OS_HAL_GPT0 clock speed: 1KHz or 32KHz */
	static const gpt_num timerId = gpt_num::GPT0;
	/* 1000ms */
	uint32_t m_TimerInterval = 1000;


public:
	BlinkingLed(os_hal_gpio_pin gpioPin);
	// no dtor needed

	void RaiseBlinkEvent();
	static void BlinkThreadHandler(ULONG thread_data);

private:
	static void timer_isr(void* cb_data);
};


#endif // __BLINKING_LED_CLASS_H