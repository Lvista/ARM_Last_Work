/**
 ****************************************************************************************************
 * @file        delay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���(֧��ucosii)
 *              �ṩdelay_init��ʼ�������� delay_us��delay_ms����ʱ����
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� F407���������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211014
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"


static uint32_t g_fac_us = 0;       /* us��ʱ������ */

/* ���SYS_SUPPORT_OS������,˵��Ҫ֧��OS��(������UCOS) */
#if SYS_SUPPORT_OS

#include "os.h"

/**
 * @brief     systick�жϷ�����,ʹ��OSʱ�õ�
 * @param     ticks : ��ʱ�Ľ�����  
 * @retval    ��
 */  
void SysTick_Handler(void)
{
    if (OSRunning == OS_STATE_OS_RUNNING)   /* OS��ʼ����,��ִ�������ĵ��ȴ��� */
    {
        OS_CPU_SysTickHandler();            /* ����uC/OS-III��SysTick�жϷ������ */
    }
    HAL_IncTick();
}
#endif

/**
 * @brief     ��ʼ���ӳٺ���
 * @param     sysclk: ϵͳʱ��Ƶ��, ��CPUƵ��(rcc_c_ck), 168MHz
 * @retval    ��
 */  
void delay_init(uint16_t sysclk)
{
#if SYS_SUPPORT_OS                          /* �����Ҫ֧��OS. */
    uint32_t reload;
#endif
    
    SysTick->CTRL = 0;                      /* ��Systick״̬���Ա���һ�����裬������￪���жϻ�ر����ж� */
    g_fac_us = sysclk;                      /* �����Ƿ�ʹ��OS,g_fac_us����Ҫʹ��,��Ϊ1us�Ļ���ʱ�� */
    
#if SYS_SUPPORT_OS                          /* �����Ҫ֧��OS. */
    reload = sysclk;                        /* ÿ���ӵļ������� ��λΪM */
    reload *= 1000000 / OSCfg_TickRate_Hz;  /* ����OSCfg_TickRate_Hz�趨���ʱ��
                                             * reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����
                                             */
    SysTick->CTRL |= 1 << 1;                /* ����SYSTICK�ж� */
    SysTick->LOAD = reload;                 /* ÿ1/delay_ostickspersec���ж�һ�� */
    SysTick->CTRL |= 1 << 0;                /* ����SYSTICK */
#endif
}

#if SYS_SUPPORT_OS                                      /* �����Ҫ֧��OS, �����´��� */

/**
 * @brief     ��ʱnus
 * @param     nus: Ҫ��ʱ��us��
 * @note      nusȡֵ��Χ : 0 ~ 190887435us(���ֵ�� 2^32 / fac_us @fac_us = 21)
 * @retval    ��
 */ 
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload;
    OS_ERR err;
    
    reload = SysTick->LOAD;     /* LOAD��ֵ */
    ticks = nus * g_fac_us;     /* ��Ҫ�Ľ����� */
    OSSchedLock(&err);          /* ��ֹOS���ȣ���ֹ���us��ʱ */
    told = SysTick->VAL;        /* �ս���ʱ�ļ�����ֵ */

    while (1)
    {
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;    /* ����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����. */
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if (tcnt >= ticks) break;   /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�. */
        }
    }

    OSSchedUnlock(&err);                /* �ָ�OS���� */
} 

/**
 * @brief     ��ʱnms
 * @param     nms: Ҫ��ʱ��ms�� (0< nms <= 65535) 
 * @retval    ��
 */
void delay_ms(uint16_t nms)
{
    uint32_t i;
    
    for (i=0; i<nms; i++)
    {
        delay_us(1000);
    }                /* ��ͨ��ʽ��ʱ */
}

#else  /* ��ʹ��OSʱ, �����´��� */

/**
 * @brief       ��ʱnus
 * @param       nus: Ҫ��ʱ��us��.
 * @note        nusȡֵ��Χ : 0~190887435(���ֵ�� 2^32 / fac_us @fac_us = 21)
 * @retval      ��
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;        /* LOAD��ֵ */
    ticks = nus * g_fac_us;                 /* ��Ҫ�Ľ����� */
    told = SysTick->VAL;                    /* �ս���ʱ�ļ�����ֵ */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;        /* ����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ����� */
            }
            else 
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;                      /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
            }
        }
    }
}

/**
 * @brief       ��ʱnms
 * @param       nms: Ҫ��ʱ��ms�� (0< nms <= 65535)
 * @retval      ��
 */
void delay_ms(uint16_t nms)
{
    uint32_t repeat = nms / 30;     /*  ������30,�ǿ��ǵ������г�ƵӦ�� */
    uint32_t remain = nms % 30;

    while (repeat)
    {
        delay_us(30 * 1000);        /* ����delay_us ʵ�� 1000ms ��ʱ */
        repeat--;
    }

    if (remain)
    {
        delay_us(remain * 1000);    /* ����delay_us, ��β����ʱ(remain ms)������ */
    }
}

/**
 * @brief       HAL���ڲ������õ�����ʱ
 * @note        HAL�����ʱĬ����Systick���������û�п�Systick���жϻᵼ�µ��������ʱ���޷��˳�
 * @param       Delay : Ҫ��ʱ�ĺ�����
 * @retval      None
 */
void HAL_Delay(uint32_t Delay)
{
     delay_ms(Delay);
}
#endif









