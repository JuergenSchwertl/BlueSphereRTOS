#include "BlinkingLed.h"

#define WAIT_FOR_DEBUGGER

BlinkingLed::BlinkingLed(os_hal_gpio_pin gpioPin)
	: Led(gpioPin)
{
	/* Create the event flags group used by threads 1 and 5.  */
	tx_event_flags_create(&m_blink_event_flags, (CHAR *)"blink event flags");

	struct os_gpt_int gptInt;

	gptInt.gpt_cb_hdl = BlinkingLed::timer_isr;
	gptInt.gpt_cb_data = (void *)this;

	/* Start GPT0 */
	mtk_os_hal_gpt_config(timerId, false, &gptInt);
		// start the interval timer
	mtk_os_hal_gpt_reset_timer(timerId, m_TimerInterval, true);
	mtk_os_hal_gpt_start(timerId);

}

void BlinkingLed::RaiseBlinkEvent()
{
	/* Set event flag to wakeup blink thread */
	tx_event_flags_set(&m_blink_event_flags, BLINK_FLAG, TX_OR);
}

void BlinkingLed::timer_isr(void* cb_data)
{
	BlinkingLed* pLed = (BlinkingLed*)cb_data;
	// just raise the blink event (similar to deferred procedure call)
	if (pLed) {
		pLed->RaiseBlinkEvent();
	}
}

void BlinkingLed::BlinkThreadHandler(ULONG thread_data)
{

	#ifdef WAIT_FOR_DEBUGGER
    // Simple way to catch the debugger on startup. Just change nWait in the debugger to continue
    volatile int iWait = 1;
    while (iWait)
    {
        ;
    }
#endif

	ULONG   actual_flags;
	BlinkingLed* pLed = new BlinkingLed(OS_HAL_GPIO_8);

	while (1)
	{
		/* Wait for blink event flag.  */
		if (tx_event_flags_get(&pLed->m_blink_event_flags, BLINK_FLAG, TX_OR_CLEAR,
				&actual_flags, TX_WAIT_FOREVER)
			!= TX_SUCCESS) 
		{
			break;
		}

		pLed->Toggle();
	}
}