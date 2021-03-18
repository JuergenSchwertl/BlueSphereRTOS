#include <stdbool.h>

#include "tx_api.h"
#include "printf.h"
//#include "mt3620.h"


#include "os_hal_gpio.h"
#include "os_hal_pwm.h"
#include "os_hal_gpt.h"

#define GLOBAL_KICK 0
#define IO_CTRL 0
#define POLARITY_SET 1

#define STACK_SIZE         1024
#define BYTE_POOL_SIZE     8192

//#define DISABLE_LOG_DEBUG

#define log_err(fmt, arg...)\
	do {\
		osai_print("[%s]: ", __func__);\
		osai_print(fmt, ##arg);\
	} while (0)

#ifndef DISABLE_LOG_DEBUG
#define log_debug(fmt, arg...)\
	do {\
		osai_print("[%s]: ", __func__);\
		osai_print(fmt, ##arg);\
	} while (0)
#else
#define log_debug(fmt, arg...)
#endif


// threadX specific variables
TX_THREAD               threadMain;
TX_BYTE_POOL            byte_pool;
UCHAR                   memory_area[BYTE_POOL_SIZE];

/** -------------------------------------------------------
 * PWM Group        , PWM Channel , PWM Bitmap  : GPIO
 * -------------------------------------------------------
 *  OS_HAL_PWM_GROUP0, PWM_CHANNEL0, OS_HAL_PWM_0: GPIO0
 *  OS_HAL_PWM_GROUP0, PWM_CHANNEL1, OS_HAL_PWM_1: GPIO1
 *  OS_HAL_PWM_GROUP0, PWM_CHANNEL2, OS_HAL_PWM_2: GPIO2
 *  OS_HAL_PWM_GROUP0, PWM_CHANNEL3, OS_HAL_PWM_3: GPIO3
 *  OS_HAL_PWM_GROUP1, PWM_CHANNEL0, OS_HAL_PWM_0: GPIO4
 *  OS_HAL_PWM_GROUP1, PWM_CHANNEL1, OS_HAL_PWM_1: GPIO5
 *  OS_HAL_PWM_GROUP1, PWM_CHANNEL2, OS_HAL_PWM_2: GPIO6
 *  OS_HAL_PWM_GROUP1, PWM_CHANNEL3, OS_HAL_PWM_3: GPIO7
 *  OS_HAL_PWM_GROUP2, PWM_CHANNEL0, OS_HAL_PWM_0: GPIO8 (LED Red)
 *  OS_HAL_PWM_GROUP2, PWM_CHANNEL1, OS_HAL_PWM_1: GPIO9 (LED Green)
 *  OS_HAL_PWM_GROUP2, PWM_CHANNEL2, OS_HAL_PWM_2: GPIO10 (LED Blue)
 *  OS_HAL_PWM_GROUP2, PWM_CHANNEL3, OS_HAL_PWM_3: GPIO11
 * -------------------------------------------------------
 */

static const uint8_t uPwmGroup_RgbLed = OS_HAL_PWM_GROUP2;


static const uint8_t pwm_channel_led_red = PWM_CHANNEL0;
static const uint8_t pwm_bitmap_led_red = OS_HAL_PWM_0;
static const uint32_t pwm_frequency_led_red = 20000;
static uint32_t pwm_duty_led_red = 300;

static const uint8_t pwm_channel_led_green = PWM_CHANNEL1;
static const uint8_t pwm_bitmap_led_green = OS_HAL_PWM_1;
static const uint32_t pwm_frequency_led_green = 20000;
static uint32_t pwm_duty_led_green = 600;

static const uint8_t pwm_channel_led_blue = PWM_CHANNEL2;
static const uint8_t pwm_bitmap_led_blue = OS_HAL_PWM_2;
static const uint32_t pwm_frequency_led_blue = 20000;
static uint32_t pwm_duty_led_blue = 900;




/* Define thread prototypes.  */
void    main_thread(ULONG thread_input);

/* Define main entry point.  */

int main(void)
{
    printf("BlueSphereRTOS app is starting\n");

     /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

void    tx_application_define(void* first_unused_memory)
{

    CHAR* pointer;

    log_debug("initialize hardware\n");
    // initialize hardware
    
/* Init PWM */
	if( mtk_os_hal_pwm_ctlr_init(uPwmGroup_RgbLed, pwm_bitmap_led_blue) != 0 ){
		log_err("mtk_os_hal_pwm_ctlr_init failed\n");
		return;
	}
    
	/* Configure PWM */
	
	if( mtk_os_hal_pwm_feature_enable(uPwmGroup_RgbLed,
            pwm_channel_led_blue,
            GLOBAL_KICK,
            IO_CTRL,
            POLARITY_SET) != 0 )
    {
        log_err("mtk_os_hal_pwm_feature_enable failed\n");
        return;
    }

    if( mtk_os_hal_pwm_config_freq_duty_normal(uPwmGroup_RgbLed,
            pwm_channel_led_blue,
            pwm_frequency_led_blue,
            pwm_duty_led_blue) != 0)
    {
        log_err("mtk_os_hal_pwm_config_freq_duty_normal failed\n");
        return;
    }

    if( mtk_os_hal_pwm_start_normal(uPwmGroup_RgbLed,
					   pwm_channel_led_blue) != 0){
        log_err("mtk_os_hal_pwm_start_normal failed\n");
        return;
    }

    log_debug("create threads\n");

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool, "byte pool", memory_area, BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for main thread.  */
    tx_byte_allocate(&byte_pool, (VOID**)&pointer, STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&threadMain, "main thread", main_thread, 0,
        pointer, STACK_SIZE,
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    log_debug("ends\n");
}

void    main_thread(ULONG thread_input)
{
    /* Print results.  */
    log_debug("starts\n");

    /* This thread simply sits in while-forever-blink loop.  */
    while (1)
    {
        pwm_duty_led_blue += 1;
		if (pwm_duty_led_blue > 1000)
			pwm_duty_led_blue = 0;

        mtk_os_hal_pwm_config_freq_duty_normal(uPwmGroup_RgbLed,
							pwm_channel_led_blue,
							pwm_frequency_led_blue,
							pwm_duty_led_blue);

        /* Sleep for 10 ticks == 1/10 sec.  */
        tx_thread_sleep(10);
    }
}


