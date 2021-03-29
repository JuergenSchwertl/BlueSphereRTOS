/*
 
 */

#define WAIT_FOR_DEBUGGER

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "printf.h"
#include "mt3620.h"
#include "os_hal_uart.h"

 
#ifdef __cplusplus
extern "C" {
#endif

//forward declarations 
int main(void);
_Noreturn void RTCoreMain(void);
void _putchar(char character);

#ifdef __cplusplus
}
#endif

/******************************************************************************/
 /* DEBUG UART configurations */
 /******************************************************************************/
static const UART_PORT nUartPortNumber = OS_HAL_UART_PORT0; // OS_HAL_UART_ISU0; 
//static const mhal_uart_data_len nUartDataLength = UART_DATA_8_BITS;
//static const mhal_uart_parity nUartParity = UART_NONE_PARITY;
//static const mhal_uart_stop_bit nUartStopBits = UART_STOP_1_BIT;
//static const uint32_t nUartBaudRate = 115200;

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
    volatile int iWait = 0;
    while(iWait == 0)
    {
        ;
    }
#endif

    // Init UART
    mtk_os_hal_uart_ctlr_init(nUartPortNumber);

    // you can omit the initialisation parameters since mtk_os_hal_uart_ctlr_init() uses 115200,8N1 already 
 //   mtk_os_hal_uart_set_format(nUartPortNumber, nUartDataLength, nUartParity, nUartStopBits);
	//mtk_os_hal_uart_set_baudrate(nUartPortNumber, nUartBaudRate);
    
    printf("UART Initialized\n");

    main();

    // Avoid warnings that main() has noreturn
    for (;;)
        __asm__("wfi");
}
