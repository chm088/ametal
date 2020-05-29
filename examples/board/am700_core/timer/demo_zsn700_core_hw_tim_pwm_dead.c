/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief ͨ�ö�ʱ���������Ļ���PWM���̣�ͨ�� HW��ӿ�ʵ��
 *
 * - ʵ������
 *   1.���ڴ�ӡ��ʾ��Ϣ.
 *   2.PIOA_2(TIM0_CHA)��PIOA_1(TIM0_CHB)���ռ�ձ�25%��20Hz��PWM��������ʱ��Ϊ100us��
 *
 * \note
 *    1. ����۲촮�ڴ�ӡ�ĵ�����Ϣ����Ҫ�� PIOA_10 �������� PC ���ڵ� TXD��
 *       PIOA_9 �������� PC ���ڵ� RXD��
 *    2. ʹ��ʾ�������߼������ǲ�������PWM������ʱ�䡣
 *    3. ����ʱ��ӦС��PWM������ʱ�䡣����ʱ����趨��Χ��6����2686us���������Ҫ�趨��������ʱ�䣬����
 *       Ҫ����demo_zsn700_hw_tim_pwm_dead.c�еķ�Ƶϵ��������ʱ������������
 *
 * \par Դ����
 * \snippet demo_zsn700_hw_tim_pwm_dead.c src_zsn700_hw_tim_pwm_dead
 *
 * \internal
 * \par Modification history
 * - 1.00 19-10-10  zp, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_zsn700_hw_tim_pwm_dead
 * \copydoc demo_zsn700_hw_tim_pwm_dead.c
 */

/** [src_zsn700_hw_tim_pwm_dead] */
#include "ametal.h"
#include "am_gpio.h"
#include "am_vdebug.h"
#include "am_zsn700.h"
#include "hw/amhw_zsn700_adtim.h"
#include "am_zsn700_inst_init.h"
#include "demo_zlg_entries.h"
#include "demo_am700_core_entries.h"

/**
 * \brief �������
 */
void demo_zsn700_core_hw_tim_pwm_dead_entry (void)
{

    AM_DBG_INFO("demo am700_core hw tim PWM dead!\r\n");

    /* ��ʼ������ */
    am_gpio_pin_cfg(PIOA_2, PIOA_2_TIM0_CHA | PIOA_2_OUT_PP);
    am_gpio_pin_cfg(PIOA_1, PIOA_1_TIM0_CHB | PIOA_1_OUT_PP);

    /* ��ʱ��ʱ��ʹ�� */
    am_clk_enable(CLK_TIM012);

    demo_zsn700_hw_tim_pwm_dead_entry((void *)ZSN700_TIM0,
                                       AMHW_ZSN700_TIM_TYPE_TIM0,
                                       ZSN700_TIM_CH0A,
                                       ZSN700_TIM_CH0B,
                                       50000 / 4,
                                       50000,
                                       100);
}
/** [src_zsn700_hw_tim_pwm_dead] */

/* end of file */