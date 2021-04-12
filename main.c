#include <stdbool.h>

#include "tx_api.h"
#include "printf.h"
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
		printf_(__func__);\
        _putchar(':');\
		printf_(fmt, ##arg);\
        _putchar('\r');\
	} while (0)

#ifndef DISABLE_LOG_DEBUG
#define log_debug(fmt, arg...)\
	do {\
		printf_(__func__);\
        _putchar(':');\
		printf_(fmt, ##arg);\
        _putchar('\r');\
	} while (0)
#else
#define log_debug(fmt, arg...)
#endif


// threadX specific variables
TX_THREAD               threadMain;
TX_BYTE_POOL            byte_pool;
UCHAR                   memory_area[BYTE_POOL_SIZE];

typedef enum _errnum_t {
    STATUS_OK = 0,
    ERR_HARDWARE_INIT = -1,
} errnum_t;

//// declare blockFifoSema if you want to use os_hal_mbox_shared_mem.c
//volatile u8 blockFifoSema;

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

static const pwm_groups uPwmGroup_RgbLed = OS_HAL_PWM_GROUP2;


//static const uint8_t pwm_channel_led_red = PWM_CHANNEL0;
//static const uint8_t pwm_bitmap_led_red = OS_HAL_PWM_0;
//static const uint32_t pwm_frequency_led_red = 20000;
//static uint32_t pwm_duty_led_red = 300;
//
//static const uint8_t pwm_channel_led_green = PWM_CHANNEL1;
//static const uint8_t pwm_bitmap_led_green = OS_HAL_PWM_1;
//static const uint32_t pwm_frequency_led_green = 20000;
//static uint32_t pwm_duty_led_green = 600;

static const pwm_channels pwm_channel_led_green = PWM_CHANNEL1;
static const pwms_bit_map pwm_bitmap_led_green = OS_HAL_PWM_1;
static const uint32_t pwm_frequency_led_green = 2;
static uint32_t pwm_duty_led_green = 600;

static const pwm_channels pwm_channel_led_blue = PWM_CHANNEL2;
static const pwms_bit_map pwm_bitmap_led_blue = OS_HAL_PWM_2;
static const uint32_t pwm_frequency_led_blue = 20000;
static uint32_t pwm_duty_led_blue = 900;




/* Define thread prototypes.  */
void    main_thread(ULONG thread_input);




int init_hardware(void)
{
    log_debug("starts\n");
    // initialize hardware
    int iRet = 0;

    volatile int iWait = 1;
    while (iWait == 0)
    {
        __asm__("nop");
    }

    iRet != mtk_os_hal_pwm_ctlr_init(uPwmGroup_RgbLed, pwm_bitmap_led_red | pwm_bitmap_led_green | pwm_bitmap_led_blue);
    /* Init PWM */
    if(iRet != 0) {
        log_err("mtk_os_hal_pwm_ctlr_init failed\n");
        //return ERR_HARDWARE_INIT;
    }

    /* Configure PWM */
    iRet |= mtk_os_hal_pwm_feature_enable(uPwmGroup_RgbLed, pwm_channel_led_red, GLOBAL_KICK, IO_CTRL, POLARITY_SET);
    iRet |= mtk_os_hal_pwm_feature_enable(uPwmGroup_RgbLed, pwm_channel_led_green, GLOBAL_KICK, IO_CTRL, POLARITY_SET);
    iRet |= mtk_os_hal_pwm_feature_enable(uPwmGroup_RgbLed, pwm_channel_led_blue, GLOBAL_KICK, IO_CTRL, POLARITY_SET);
    if ( iRet != 0)
    {
        log_err("mtk_os_hal_pwm_feature_enable failed\n");
        //return ERR_HARDWARE_INIT;
    }

    iRet |= mtk_os_hal_pwm_config_freq_duty_normal(uPwmGroup_RgbLed, pwm_channel_led_red, pwm_frequency_led_red, pwm_duty_led_red);
    iRet |= mtk_os_hal_pwm_config_freq_duty_normal(uPwmGroup_RgbLed, pwm_channel_led_green, pwm_frequency_led_green, pwm_duty_led_green);
    iRet |= mtk_os_hal_pwm_config_freq_duty_normal(uPwmGroup_RgbLed, pwm_channel_led_blue, pwm_frequency_led_blue, pwm_duty_led_blue);
    if( iRet != 0)
    {
        log_err("mtk_os_hal_pwm_config_freq_duty_normal failed\n");
        //return ERR_HARDWARE_INIT;
    }

    iRet |= mtk_os_hal_pwm_start_normal(uPwmGroup_RgbLed, pwm_channel_led_red);
    iRet |= mtk_os_hal_pwm_start_normal(uPwmGroup_RgbLed, pwm_channel_led_green);
    iRet |= mtk_os_hal_pwm_start_normal(uPwmGroup_RgbLed, pwm_channel_led_blue);
    if (iRet != 0) {
        log_err("mtk_os_hal_pwm_start_normal failed\n");
        //return ERR_HARDWARE_INIT;
    }

    log_debug("ends\n");
    return iRet;
}

void    tx_application_define(void* first_unused_memory)
{
    log_debug("starts\n");

    CHAR* pointer;

    init_hardware();

    log_debug("create threads\n");

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool, (CHAR *) "byte pool", memory_area, BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for main thread.  */
    tx_byte_allocate(&byte_pool, (VOID**)&pointer, STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&threadMain, (CHAR *) "main thread", main_thread, 0,
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
        pwm_duty_led_blue += 100;
		if (pwm_duty_led_blue > 1000)
			pwm_duty_led_blue = 0;

        mtk_os_hal_pwm_config_freq_duty_normal(uPwmGroup_RgbLed,
							pwm_channel_led_blue,
							pwm_frequency_led_blue,
							pwm_duty_led_blue);

        /* Sleep for 25 ticks == 25/100 = 1/4  sec.  */
        tx_thread_sleep(25);
    }

    log_debug("ends\n");
}


int main(void)
{
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    return STATUS_OK;
}