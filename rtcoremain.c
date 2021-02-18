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
static const uint8_t uart_port_num = OS_HAL_UART_ISU0;

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

    // Init UART
    mtk_os_hal_uart_ctlr_init(uart_port_num);
    printf("UART Initialized (port_num=%d)\n", uart_port_num);

    main();

    // Avoid warnings that main() has noreturn
    for (;;)
        __asm__("wfi");
}
