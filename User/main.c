#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"

#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/ADC/adc.h"

#include "./MALLOC/malloc.h"
#include "uc-os3_demo.h"

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7); /* 设置时钟,168Mhz */
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化led */
    key_init();                         /* 初始化KEY */
    lcd_init();                         /* 初始化LCD */
    adc_init();                         /* 初始化ADC */

    uc_os3_demo(); /* 运行uC/OS-III例程 */
}
