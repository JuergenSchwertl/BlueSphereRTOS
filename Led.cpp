#include "os_hal_gpio.h"

#include "Led.h"


os_hal_gpio_data Led::set(os_hal_gpio_data gpioData)
{
	this->pgioState = gpioData;
	mtk_os_hal_gpio_set_output(this->gpioPin, gpioData);
	return gpioData;
}


Led::Led(os_hal_gpio_pin gpioPin)
{
	this->gpioPin = gpioPin;
	/* Init GPIO */
	mtk_os_hal_gpio_set_direction(gpioPin, OS_HAL_GPIO_DIR_OUTPUT);
	set(OS_HAL_GPIO_DATA_HIGH);
}


os_hal_gpio_data Led::Toggle()
{
	return set( (this->pgioState == OS_HAL_GPIO_DATA_LOW) ? OS_HAL_GPIO_DATA_HIGH : OS_HAL_GPIO_DATA_LOW );
}