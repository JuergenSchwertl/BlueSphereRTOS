#include <stdbool.h>

#include <tx_api.h>
#include <printf.h>
#include <os_hal_gpt.h>

#include "globals.h"
#include "Logger.h"
#include "BlinkingLed.h"


#define THREAD_STACK_SIZE   4096
#define BYTE_POOL_SIZE      16383

//#define WAIT_FOR_DEBUGGER


// threadX specific variables
TX_THREAD               threadMain;
TX_THREAD               threadBlinkingLed;
TX_BYTE_POOL            byte_pool;
UCHAR                   memory_area[BYTE_POOL_SIZE];

typedef enum _errnum_t {
    STATUS_OK = 0,
    ERR_HARDWARE_INIT = -1,
} errnum_t;

//// declare blockFifoSema if you want to use os_hal_mbox_shared_mem.c
//volatile u8 blockFifoSema;


static BlinkingLed* pBlinkingLed = nullptr;


/* Define thread prototypes.  */
void    main_thread(ULONG thread_input);



int init_hardware(void)
{
    log_enter();
    // initialize hardware
    int iRet = 0;

    /*volatile int iWait = 1;
    while (iWait)
    {
        __asm__("nop");
    }*/
    
	/* Init GPT */
	mtk_os_hal_gpt_init();
    pBlinkingLed = new BlinkingLed(OS_HAL_GPIO_8);

	log_debug("&pLed (static) : %#010x, pLed : %#010x", (UINT) &pBlinkingLed, (UINT) pBlinkingLed);

    log_exit();
    return iRet;
}

void tx_application_define(void* first_unused_memory)
{
    log_enter();

    void * pBytePool;

    init_hardware();

    log_message("create threads.");

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool, (CHAR *) "ThreadStacks", memory_area, BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    ///* Allocate the stack for main thread.  */
    //tx_byte_allocate(&byte_pool, &pBytePool, THREAD_STACK_SIZE, TX_NO_WAIT);

    ///* Create the main thread.  */
    //tx_thread_create(&threadMain, (CHAR *) "main thread", main_thread, 0,
    //    pBytePool, THREAD_STACK_SIZE,
    //    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for blinking thread.  */
    tx_byte_allocate(&byte_pool, &pBytePool, THREAD_STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&threadBlinkingLed, (CHAR*)"blink thread", BlinkingLed::BlinkThreadHandler, (ULONG)pBlinkingLed,
        pBytePool, THREAD_STACK_SIZE,
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    log_exit();
}

void    main_thread(ULONG thread_input)
{
    /* Print results.  */
    log_enter();

    /* This thread simply sits in while-forever loop.  */
    while (1)
    {
        putchar('.');

        /* Sleep for 25 ticks == 25/100 = 1/4  sec.  */
        tx_thread_sleep(100);
    }

    log_exit();
}


int main(void)
{
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    return STATUS_OK;
}