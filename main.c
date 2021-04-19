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

static const os_hal_gpio_pin gpio_led_red = OS_HAL_GPIO_8;
static bool bLedState = false;


/* Define thread prototypes.  */
void    main_thread(ULONG thread_input);




int init_hardware(void)
{
    log_debug("starts\n");
    // initialize hardware
    int iRet = 0;

    volatile int iWait = 1;
    while (iWait)
    {
        __asm__("nop");
    }

    /* Init GPIO */
    mtk_os_hal_gpio_set_direction(gpio_led_red, OS_HAL_GPIO_DIR_OUTPUT);
    mtk_os_hal_gpio_set_output(gpio_led_red, OS_HAL_GPIO_DATA_HIGH);

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
        mtk_os_hal_gpio_set_output(gpio_led_red, (bLedState) ? OS_HAL_GPIO_DATA_HIGH : OS_HAL_GPIO_DATA_LOW);
        bLedState = !bLedState;


        /* Sleep for 25 ticks == 25/100 = 1/4  sec.  */
        tx_thread_sleep(50);
    }

    log_debug("ends\n");
}


int main(void)
{
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    return STATUS_OK;
}