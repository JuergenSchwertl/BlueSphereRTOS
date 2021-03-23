/*
 
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "printf.h"
#include "mt3620.h"
#include "os_hal_uart.h"

 /******************************************************************************/
 /* Configurations */
 /******************************************************************************/
static const uint8_t uart_port_num = OS_HAL_UART_PORT0; // OS_HAL_UART_ISU0; 
static const mhal_uart_data_len uart_dat_len = UART_DATA_8_BITS;
static const mhal_uart_parity uart_parity = UART_NONE_PARITY;
static const mhal_uart_stop_bit uart_stop_bit = UART_STOP_1_BIT;
static const uint32_t uart_baudrate = 115200;

//forward declaration for main() function in main.c 
int main(void);

/******************************************************************************/
/* Application Hooks */
/******************************************************************************/
// Hook for "printf".
void _putchar(char character)
{
    mtk_os_hal_uart_put_char(uart_port_num, character);
    if (character == '\n')
        mtk_os_hal_uart_put_char(uart_port_num, '\r');
}



_Noreturn void RTCoreMain(void)
{

    // Init Vector Table
    NVIC_SetupVectorTable();

    int nWait = 0;

    // Simple way to catch the debugger on startup. Just change nWait in the debugger to continue
    while(nWait == 0)
    {
        ;
    }

    int ret = 0;
    // Init UART
    mtk_os_hal_uart_ctlr_init(uart_port_num);
    mtk_os_hal_uart_set_format(uart_port_num, uart_dat_len, uart_parity, uart_stop_bit);
	mtk_os_hal_uart_set_baudrate(uart_port_num, uart_baudrate);
    printf("UART Initialized (port_num=%d)\n", uart_port_num);

    main();

    // Avoid warnings that main() has noreturn
    for (;;)
        __asm__("wfi");
}
