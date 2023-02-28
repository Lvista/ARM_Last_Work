#include "uc-os3_demo.h"

#include "./MALLOC/malloc.h" /* ʹ��sram����ռ� */

#include "./SYSTEM/usart/usart.h" /* ʹ�ô��� */
#include "./BSP/LED/led.h"        /* �õ�LED�����Լ������ͷ�ļ� */
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/ADC/adc.h"

/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/*uC/OS-III����*/

/* START_TASK ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define START_TASK_PRIO 2              /* �������ȼ� */
#define START_STK_SIZE 512             /* ����ջ��С */
OS_TCB StartTask_TCB;                  /* ������ƿ� */
CPU_STK StartTask_STK[START_STK_SIZE]; /* ����ջ */
void start_task(void *p_arg);          /* ������ */

/* TASK1 ���� ����      HomePage
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK1_TASK_PRIO (OS_CFG_PRIO_MAX - 2) /* �������ȼ� */
#define TASK1_STK_SIZE 256                    /* ����ջ��С */
#define HomePage TASK1_TCB
OS_TCB HomePage;         /* ������ƿ� */
CPU_STK *TASK1Task_STK;  /* ����ջ */
void task1(void *p_arg); /* ������ */

/* TASK2 ���� ����      ����ɨ��
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK2_TASK_PRIO (OS_CFG_PRIO_MAX - 8) /* �������ȼ� */
#define TASK2_STK_SIZE 256                    /* ����ջ��С */
OS_TCB TASK2_TCB;                             /* ������ƿ� */
CPU_STK *TASK2Task_STK;                       /* ����ջ */
void task2(void *p_arg);                      /* ������ */

/* TASK3 ���� ����    ���ڹ���ͻָ���������
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK3_TASK_PRIO (OS_CFG_PRIO_MAX - 9) /* �������ȼ� */
#define TASK3_STK_SIZE 256                    /* ����ջ��С */
OS_TCB TASK3_TCB;                             /* ������ƿ� */
CPU_STK *TASK3Task_STK;                       /* ����ջ */
void task3(void *p_arg);                      /* ������ */

/* TASK4 ���� ����    Menu
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK4_TASK_PRIO (OS_CFG_PRIO_MAX - 3) /* �������ȼ� */
#define TASK4_STK_SIZE 256                    /* ����ջ��С */
#define Menu TASK4_TCB
OS_TCB Menu;             /* ������ƿ� */
CPU_STK *TASK4Task_STK;  /* ����ջ */
void task4(void *p_arg); /* ������ */

/* TASK5 ���� ����    Beep
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK5_TASK_PRIO (OS_CFG_PRIO_MAX - 4) /* �������ȼ� */
#define TASK5_STK_SIZE 256                    /* ����ջ��С */
#define Beep TASK5_TCB
OS_TCB Beep;             /* ������ƿ� */
CPU_STK *TASK5Task_STK;  /* ����ջ */
void task5(void *p_arg); /* ������ */

/* TASK6 ���� ����    Time_slc
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK6_TASK_PRIO (OS_CFG_PRIO_MAX - 4) /* �������ȼ� */
#define TASK6_STK_SIZE 256                    /* ����ջ��С */
#define Time_slc TASK6_TCB
OS_TCB Time_slc;         /* ������ƿ� */
CPU_STK *TASK6Task_STK;  /* ����ջ */
void task6(void *p_arg); /* ������ */
/* TASK7 ���� ����    Time_edt
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK7_TASK_PRIO (OS_CFG_PRIO_MAX - 4) /* �������ȼ� */
#define TASK7_STK_SIZE 256                    /* ����ջ��С */
#define Time_edt TASK7_TCB
OS_TCB Time_edt;         /* ������ƿ� */
CPU_STK *TASK7Task_STK;  /* ����ջ */
void task7(void *p_arg); /* ������ */

/* ��Ļˢ����ɫ˳�� */
uint16_t colorseqence[5] = {WHITE, RED, MAGENTA, YELLOW, GREEN};
/*****************/
/* �����ʱ�� *********/
static uint8_t hour = 23;
static uint8_t min = 58;
static uint8_t sec = 0;
OS_TMR time_tick;
void time_tick_cb(OS_TMR *p_tmr, void *p_arg);
/********************/
/* ��Ϣ���� **************/
/************************/
/* �м��� *******************/
OS_TCB *pre_stu; // ���ڴ��浱ǰ״̬
uint8_t *tim_slc = NULL;
uint8_t is_hour = 1;
uint8_t tem_hour;
uint8_t tem_min;
void check_tim();
/*****************************/

/**
 * @brief       uC/OS-III������ں���
 * @param       ��
 * @retval      ��
 */
void uc_os3_demo(void)
{
    OS_ERR err;

    lcd_draw_line(0, 44, 320, 44, BLACK);

    /* �ر������ж� */
    CPU_IntDis();

    /* ��ʼ��uC/OS-III */
    OSInit(&err);

    /* ����Start Task */
    OSTaskCreate((OS_TCB *)&StartTask_TCB,
                 (CPU_CHAR *)"start_task",
                 (OS_TASK_PTR)start_task,
                 (void *)0,
                 (OS_PRIO)START_TASK_PRIO,
                 (CPU_STK *)StartTask_STK,
                 (CPU_STK_SIZE)START_STK_SIZE / 10,
                 (CPU_STK_SIZE)START_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);

    /* ��ʼ������� */
    OSStart(&err);

    for (;;)
    {
        /* ����������� */
    }
}

/**
 * @brief       start_task
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_INT32U cnts;

    /* ��ʼ��CPU�� */
    CPU_Init();

    /* �������õĽ���Ƶ������SysTick */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OSCfg_TickRate_Hz);
    OS_CPU_SysTickInit(cnts);

    /* ����ʱ��Ƭ���ȣ�ʱ��Ƭ��ΪĬ��ֵ */
    OSSchedRoundRobinCfg(OS_TRUE, 0, &err);

    OSTmrCreate((OS_TMR *)&time_tick,              // ָ�������ʱ���ṹ���ָ��
                (CPU_CHAR *)"time_tick",           // ָ����Ϊ�����ʱ������ ASCII �ַ�����ָ��
                (OS_TICK)0,                        // �����ʱ���Ŀ�����ʱʱ��
                (OS_TICK)10,                       // ���ڶ�ʱ���Ķ�ʱ����ʱ��
                (OS_OPT)OS_OPT_TMR_PERIODIC,       // ��������ѡ��������ǵ��ζ�ʱ���������ڶ�ʱ����
                (OS_TMR_CALLBACK_PTR)time_tick_cb, // ָ�������ʱ����ʱ�ص�������ָ��
                (void *)0,                         // ָ�������ʱ����ʱ�ص�����������ָ��
                &err);                             // P405

    OSTmrStart(&time_tick, &err);

    /* �û�������Start */
    TASK1Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK1_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate((OS_TCB *)&TASK1_TCB,
                 (CPU_CHAR *)"TASK1",
                 (OS_TASK_PTR)task1,
                 (void *)0,
                 (OS_PRIO)TASK1_TASK_PRIO,
                 (CPU_STK *)TASK1Task_STK,
                 (CPU_STK_SIZE)TASK1_STK_SIZE / 10,
                 (CPU_STK_SIZE)TASK1_STK_SIZE,
                 (OS_MSG_QTY)1,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    pre_stu = &TASK1_TCB;

    TASK2Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK2_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate((OS_TCB *)&TASK2_TCB,
                 (CPU_CHAR *)"TASK2",
                 (OS_TASK_PTR)task2,
                 (void *)0,
                 (OS_PRIO)TASK2_TASK_PRIO,
                 (CPU_STK *)TASK2Task_STK,
                 (CPU_STK_SIZE)TASK2_STK_SIZE / 10,
                 (CPU_STK_SIZE)TASK2_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);

    TASK3Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK3_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate((OS_TCB *)&TASK3_TCB,
                 (CPU_CHAR *)"TASK3",
                 (OS_TASK_PTR)task3,
                 (void *)0,
                 (OS_PRIO)TASK3_TASK_PRIO,
                 (CPU_STK *)TASK3Task_STK,
                 (CPU_STK_SIZE)TASK3_STK_SIZE / 10,
                 (CPU_STK_SIZE)TASK3_STK_SIZE,
                 (OS_MSG_QTY)1,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);

    TASK4Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK4_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate((OS_TCB *)&TASK4_TCB,
                 (CPU_CHAR *)"TASK4",
                 (OS_TASK_PTR)task4,
                 (void *)0,
                 (OS_PRIO)TASK4_TASK_PRIO,
                 (CPU_STK *)TASK4Task_STK,
                 (CPU_STK_SIZE)TASK4_STK_SIZE / 10,
                 (CPU_STK_SIZE)TASK4_STK_SIZE,
                 (OS_MSG_QTY)1,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    OSTaskSuspend(&TASK4_TCB, &err);

    TASK6Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK6_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate((OS_TCB *)&TASK6_TCB,
                 (CPU_CHAR *)"TASK6",
                 (OS_TASK_PTR)task6,
                 (void *)0,
                 (OS_PRIO)TASK6_TASK_PRIO,
                 (CPU_STK *)TASK6Task_STK,
                 (CPU_STK_SIZE)TASK6_STK_SIZE / 10,
                 (CPU_STK_SIZE)TASK6_STK_SIZE,
                 (OS_MSG_QTY)1,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    OSTaskSuspend(&TASK6_TCB, &err);

    TASK7Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK7_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate((OS_TCB *)&TASK7_TCB,
                 (CPU_CHAR *)"TASK7",
                 (OS_TASK_PTR)task7,
                 (void *)0,
                 (OS_PRIO)TASK7_TASK_PRIO,
                 (CPU_STK *)TASK7Task_STK,
                 (CPU_STK_SIZE)TASK7_STK_SIZE / 10,
                 (CPU_STK_SIZE)TASK7_STK_SIZE,
                 (OS_MSG_QTY)1,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    OSTaskSuspend(&TASK7_TCB, &err);

    /* �û�������End */

    /* ɾ��Start Task */
    OSTaskDel((OS_TCB *)0, &err);
}

/**
 * @brief       HomePage
 *
 */
void task1(void *p_arg)
{
    OS_ERR err;
    uint16_t adcx;
    uint16_t color = GREEN;
    OS_MSG_SIZE msg_size;
    uint8_t *key = NULL;

    while (1)
    {
        adcx = adc_get_result_average(ADC_ADCX_CHY, 10); /* ��ȡͨ��5��ת��ֵ��10��ȡƽ�� */
        if (adcx < 5 || adcx > 3900)
        {
            color = BLACK;
        }
        else if (adcx < 2000 && adcx > 1000)
        {
            color = YELLOW;
        }
        else if (adcx < 3900 && adcx > 2000)
        {
            color = RED;
        }
        else
            color = GREEN;
        lcd_show_xnum(128, 84, adcx, 5, 32, 0, color); /* ��ʾADC�������ԭʼֵ */
        lcd_fill(86, 84, 118, 116, color);

        key = OSTaskQPend(0,
                          OS_OPT_PEND_NON_BLOCKING,
                          &msg_size,
                          0,
                          &err);
        if (key != NULL)
        {
            OSTaskQPost(&TASK3_TCB,
                        &Menu,
                        sizeof(Menu),
                        OS_OPT_POST_FIFO,
                        &err);
        }
        OSTimeDly(50, OS_OPT_TIME_DLY, &err);
    }
}

/**
 * @brief       ɨ�谴��
 */
void task2(void *p_arg)
{
    OS_ERR err;
    uint8_t key;

    while (1)
    {
        key = key_scan(0);
        if (key != 0)
        {
            OSTaskQPost((OS_TCB *)pre_stu,
                        (void *)&key,             // Ҫ���͵���Ϣ���еı���
                        (OS_MSG_SIZE)sizeof(key), // ����Ҫ���͵ı����Ĵ�С
                        (OS_OPT)OS_OPT_POST_FIFO, // �Ƚ��ȳ�ģʽ
                        (OS_ERR *)&err);
        }
        OSTimeDly(10, OS_OPT_TIME_DLY, &err); /* ��ʱ500ticks */
    }
}
/**
 * @brief       �������ͻָ����������л�״̬
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task3(void *p_arg)
{
    OS_ERR err;
    OS_TCB *next_stu = NULL;
    OS_MSG_SIZE msg_size;
    CPU_SR_ALLOC(); // ʹ���ٽ�ģʽ

    while (1)
    {
        next_stu = OSTaskQPend(0,
                               OS_OPT_PEND_BLOCKING,
                               &msg_size,
                               0,
                               &err);
        CPU_CRITICAL_ENTER();
        lcd_fill(0, 45, 320, 240, WHITE);
        CPU_CRITICAL_EXIT();
        OSTaskSuspend(pre_stu, &err);
        OSTaskResume(next_stu, &err);
        pre_stu = next_stu;
    }
}

/**
 * @brief       Menu
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */

void task4(void *p_arg)
{
    OS_ERR err;
    uint8_t *key;
    OS_MSG_SIZE msg_size;
    OS_TCB *substu = NULL;

    while (1)
    {
        lcd_draw_rectangle(20, 60, 300, 110, BLACK);  // ѡ��BEEP���
        lcd_draw_rectangle(20, 130, 300, 180, BLACK); // ѡ��TIME���
        lcd_draw_rectangle(30, 70, 60, 100, BLACK);   // ѡ��BEEPѡ����ʾ���
        lcd_draw_rectangle(30, 140, 60, 170, BLACK);  // ѡ��TIMEѡ����ʾ���
        lcd_show_string(140, 75, 8 * 12, 24, 24, "BEEP_SET", BLACK);
        lcd_show_string(140, 145, 8 * 12, 24, 24, "TIME_SET", BLACK);

        key = OSTaskQPend(0,
                          OS_OPT_PEND_BLOCKING,
                          &msg_size,
                          0,
                          &err);
        if (key != NULL)
        {
            switch (*key)
            {
            case KEY0_PRES:
                OSTaskQPost(&TASK3_TCB,
                            &HomePage,
                            sizeof(HomePage),
                            OS_OPT_POST_FIFO,
                            &err);
                break;

            case KEY1_PRES:
                lcd_fill(31, 141, 59, 169, WHITE);
                lcd_fill(31, 71, 59, 99, BLUE);
                substu = &Beep;
                break;

            case WKUP_PRES:
                lcd_fill(31, 71, 59, 99, WHITE);
                lcd_fill(31, 141, 59, 169, BLUE);
                substu = &Time_slc;
                break;

            case KEY2_PRES:
                if (substu != NULL)
                {
                    OSTaskQPost(&TASK3_TCB,
                                substu,
                                sizeof(Menu),
                                OS_OPT_POST_FIFO,
                                &err);
                    substu = NULL;
                }
                break;

            default:
                break;
            }
        }
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}

/**
 * @brief       Beep
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task5(void *p_arg)
{
}
/**
 * @brief       Time_select_mode
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task6(void *p_arg)
{
    OS_ERR err;
    uint8_t *key;
    OS_MSG_SIZE msg_size;

    while (1)
    {
        tem_min = min;
        tem_hour = hour;
        lcd_show_xnum(94, 104, tem_hour, 2, 32, 0x80, BLACK);
        lcd_show_xnum(194, 104, tem_min, 2, 32, 0x80, BLACK);
        key = OSTaskQPend(0,
                          OS_OPT_PEND_BLOCKING,
                          &msg_size,
                          0,
                          &err);
        if (key != NULL)
        {
            switch (*key)
            {
            case KEY0_PRES:
                OSTaskQPost(&TASK3_TCB,
                            &Menu,
                            sizeof(Menu),
                            OS_OPT_POST_FIFO,
                            &err);
                break;

            case KEY1_PRES:
                lcd_draw_circle(210, 120, 30, WHITE);
                lcd_draw_circle(110, 120, 30, BLACK);
                tim_slc = &tem_hour;
                is_hour = 1;
                break;

            case WKUP_PRES:
                lcd_draw_circle(110, 120, 30, WHITE);
                lcd_draw_circle(210, 120, 30, BLACK);
                tim_slc = &tem_min;
                is_hour = 0;
                break;

            case KEY2_PRES:
                if (tim_slc != NULL)
                {
                    OSTaskQPost(&TASK3_TCB,
                                &Time_edt,
                                sizeof(Time_edt),
                                OS_OPT_POST_FIFO,
                                &err);
                }
                break;

            default:
                break;
            }
        }
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}
/**
 * @brief       Time_edt
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task7(void *p_arg)
{
    OS_ERR err;
    uint8_t *key;
    OS_MSG_SIZE msg_size;

    while (1)
    {
        if (is_hour)
        {
            lcd_fill_circle(110, 120, 30, LGRAY);
        }
        else
        {
            lcd_fill_circle(210, 120, 30, LGRAY);
        }
        lcd_show_xnum(94, 104, tem_hour, 2, 32, 0x81, BLACK);
        lcd_show_xnum(194, 104, tem_min, 2, 32, 0x81, BLACK);
        key = OSTaskQPend(0,
                          OS_OPT_PEND_BLOCKING,
                          &msg_size,
                          0,
                          &err);
        if (key != NULL)
        {
            switch (*key)
            {
            case KEY0_PRES:
                tim_slc = NULL;
                OSTaskQPost(&TASK3_TCB,
                            &Time_slc,
                            sizeof(Time_slc),
                            OS_OPT_POST_FIFO,
                            &err);
                break;

            case KEY1_PRES:
                (*tim_slc)--;
                check_tim();
                break;

            case WKUP_PRES:
                (*tim_slc)++;
                check_tim();
                break;

            case KEY2_PRES:
                if (is_hour)
                {
                    hour = *tim_slc;
                }
                else
                {
                    min = *tim_slc;
                }
                break;

            default:
                break;
            }
        }
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}
void time_tick_cb(OS_TMR *p_tmr, void *p_arg)
{
    static uint8_t dat = ':';

    sec++;
    dat = (sec % 2) ? ':' : ' ';
    if (sec == 60)
    {
        sec = 0;
        min++;
        if (min == 60)
        {
            min = 0;
            hour++;
            if (hour == 24)
            {
                hour = 0;
            }
        }
    }
    lcd_show_xnum(10, 10, hour, 2, 24, 0x80, BLACK);
    lcd_show_char(34, 10, dat, 24, 0x00, BLACK);
    lcd_show_xnum(46, 10, min, 2, 24, 0x80, BLACK);
}

void check_tim()
{
    if (is_hour)
    {
        if (*tim_slc == 255)
        {
            *tim_slc = 23;
        }
        else if (*tim_slc == 24)
        {
            *tim_slc = 0;
        }
    }
    else
    {
        if (*tim_slc == 60)
        {
            *tim_slc = 0;
        }
        else if (*tim_slc == 255)
        {
            *tim_slc = 59;
        }
    }
}
