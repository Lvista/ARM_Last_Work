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
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(336, 8, 2, 7); /* ����ʱ��,168Mhz */
    delay_init(168);                    /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                         /* ��ʼ��led */
    key_init();                         /* ��ʼ��KEY */
    lcd_init();                         /* ��ʼ��LCD */
    adc_init();                         /* ��ʼ��ADC */

    uc_os3_demo(); /* ����uC/OS-III���� */
}
