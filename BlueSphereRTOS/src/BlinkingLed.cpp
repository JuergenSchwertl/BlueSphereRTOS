#include "BlinkingLed.h"

BlinkingLed::BlinkingLed(os_hal_gpio_pin gpioPin)
	: Led(gpioPin)
{
	/* Create the event flags group used by threads 1 and 5.  */
	tx_event_flags_create(&m_blink_event_flags, "blink event flags");
}

void BlinkingLed::RaiseBlinkEvent()
{
	/* Set event flag 0 to wakeup thread 5.  */
	tx_event_flags_set(&m_blink_event_flags, BLINK_FLAG, TX_OR);
}

void BlinkingLed::BlinkThreadHandler()
{
	ULONG   actual_flags;

	while (1)
	{
		/* Wait for blink event flag.  */
		if (tx_event_flags_get(&m_blink_event_flags, BLINK_FLAG, TX_OR_CLEAR,
				&actual_flags, TX_WAIT_FOREVER)
			!= TX_SUCCESS) 
		{
			break;
		}

		Toggle();
	}
}