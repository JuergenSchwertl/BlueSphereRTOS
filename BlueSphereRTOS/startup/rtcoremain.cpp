/*
 
 */

//#define WAIT_FOR_DEBUGGER

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "printf.h"
#include "mt3620.h"
#include "tx_api.h"
#include "os_hal_uart.h"

 
#ifdef __cplusplus
extern "C" {
#endif

//forward declarations 
int main(void);
_Noreturn void RTCoreMain(void);
void _putchar(char character);

// Initialization function for C++ static constructors
// normally those would be called by C++ startup code
typedef void (*InitFunc)(void);
extern InitFunc __init_array_start;
extern InitFunc __init_array_end;
extern void * __bss_start__;
extern void * __bss_end__;
extern void * StackTop;
#ifdef __cplusplus
}
#endif

/******************************************************************************/
 /* DEBUG UART configurations */
 /******************************************************************************/
static const UART_PORT nUartPortNumber = OS_HAL_UART_PORT0; // OS_HAL_UART_ISU0; 

/******************************************************************************/
/* Application Hooks */
/******************************************************************************/
// Hook for "printf".
void _putchar(char character)
{
    mtk_os_hal_uart_put_char(nUartPortNumber, character);
    if (character == '\n')
        mtk_os_hal_uart_put_char(nUartPortNumber, '\r');
}

_Noreturn void RTCoreMain(void)
{
    // Init Vector Table
    NVIC_SetupVectorTable();

#ifdef WAIT_FOR_DEBUGGER
    // Simple way to catch the debugger on startup. Just change nWait in the debugger to continue
    volatile int iWait = 1;
    while(iWait)
    {
        __asm__("nop");
    }
#endif

    // Init UART: defaults to uses 115200,8N1
    mtk_os_hal_uart_ctlr_init(nUartPortNumber);
    
    printf("UART Initialized\n");

    printf( "__bss_start__ : %#010x, __bss_end__ : %#010x, size = %d\n"
            "__init_array_start : %#010x, __init_array_end : %#010x, size = %d\n"
            "StackTop : %#010x\n", 
            (UINT) &__bss_start__, (UINT) &__bss_end__, (UINT)&__bss_end__ - (UINT)&__bss_start__,
            (UINT) &__init_array_start, (UINT) &__init_array_end, (UINT)&__init_array_end - (UINT)&__init_array_start,
            (UINT) &StackTop);

#ifdef __cplusplus
    // Call global constructors
    for (InitFunc* func = &__init_array_start; func < &__init_array_end; ++func) (*func)();
#endif

    main();

    // Avoid warnings that main() has noreturn
    for (;;)
        __asm__("wfi");
}
