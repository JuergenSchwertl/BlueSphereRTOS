#include <stdbool.h>

#include "tx_api.h"
#include "printf.h"
#include "os_hal_gpio.h"
#include "os_hal_gpt.h"

#define STACK_SIZE         1024
#define BYTE_POOL_SIZE     8192

TX_THREAD               threadMain;
TX_BYTE_POOL            byte_pool;
UCHAR                   memory_area[BYTE_POOL_SIZE];



//static const os_hal_gpio_pin gpioPin = OS_HAL_GPIO_8;
//static const os_hal_gpio_pin gpioPin = OS_HAL_GPIO_9;
static const os_hal_gpio_pin gpioLedPin = OS_HAL_GPIO_10; // RGB LED 0, Blue
os_hal_gpio_data gpioLedState = OS_HAL_GPIO_DATA_HIGH; // initial state is off (LED is low active)

/* Define thread prototypes.  */
void    main_thread(ULONG thread_input);
void    blink_timer( void );

/* Define main entry point.  */

int main(void)
{
    int nWait = 0;
    // Simple way to catch the debugger on startup. Just change nWait in the debugger to continue
    while(nWait == 0)
    {
        ;
    }

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

void    tx_application_define(void* first_unused_memory)
{

    CHAR* pointer;

    // initialize hardware
    mtk_os_hal_gpio_set_direction(gpioLedPin, OS_HAL_GPIO_DIR_OUTPUT);
    mtk_os_hal_gpio_set_output(gpioLedPin, OS_HAL_GPIO_DATA_HIGH);

    
    

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
}

void    main_thread(ULONG thread_input)
{
    /* Print results.  */
    printf("**** BlueSphereRTOS \n\n");


    /* This thread simply sits in while-forever-blink loop.  */
    while (1)
    {
        
        /* Sleep for 100 ticks == 1 sec.  */
        tx_thread_sleep(100);
    }
}

void blink_timer(void)
{
    gpioLedState = (gpioLedState==OS_HAL_GPIO_DATA_LOW) ? OS_HAL_GPIO_DATA_HIGH : OS_HAL_GPIO_DATA_LOW;
    mtk_os_hal_gpio_set_output(gpioLedPin, gpioLedState);
}
