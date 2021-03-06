/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      ametal.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief book`s matrixkey driver for AM824
 * 
 * \internal
 * \par Modification history
 * - 1.00 18-09-12  adw, first implementation.
 * \endinternal
 */
#include "ametal.h"
#include "matrixkey.h"
#include "am_lpc82x.h"
#include "am_softimer.h"
#include "am_vdebug.h"
#include "am_delay.h"
#include "digitron1.h"
static const int g_matrixkey_row[] = {PIO0_6, PIO0_7};              // 行线对应的管脚
static const int g_matrixkey_col[] = {PIO0_17, PIO0_23};            // 列线对应的管脚
static const uint8_t matrixkey_row_num = sizeof(g_matrixkey_row)/sizeof(g_matrixkey_row[0]);    // 行数
static const uint8_t matrixkey_col_num = sizeof(g_matrixkey_col)/sizeof(g_matrixkey_col[0]);    // 列数
static const uint8_t matrixkey_num = matrixkey_row_num * matrixkey_col_num;                     // 按键个数
static uint32_t matrixkey_last_value;                       // 上次键值变量
static uint32_t matrixkey_final_value;                      // 最终键值变量

void matrixkey_kl_read (uint32_t *matrixkey_value, int col)
{
    int j = 0;
	
    for (j = 0; j < matrixkey_row_num; j++) {
        // 读取各行的值，判断是否有键按下？
        if (am_gpio_get(g_matrixkey_row[j]) == 0) {
            // 该行有键按下
            (*matrixkey_value) &= ~(1 << (j * matrixkey_col_num + col)); // 有键按下，对应位清0
        }
    }
}
// 列扫描 读取 
static uint32_t matrixkey_read (void)                        // 获取键值函数
{                                                           
  uint32_t matrixkey_value = 0xFFFFFFFF;                     // 无键按下，其相应位为1，否则为0
  int  i;                                                
                                                            
  for (i = 0; i < matrixkey_col_num; i++) {                  // 逐列扫描
        am_gpio_set(g_matrixkey_col[i], 0);                  // 第i列输出低电平
        
        matrixkey_kl_read(&matrixkey_value, i);
        am_gpio_set(g_matrixkey_col[i], 1);      // 第i行扫描完毕，恢复高电平
  }
    return matrixkey_value;
}


static uint8_t matrixkey_val_process (uint32_t matrixkey_current_value)
{
	uint8_t   ret_flag = 0xFF;
	uint32_t  change = 0;                				// 按键状态变化标志
	int       i;
	
	if (matrixkey_last_value == matrixkey_current_value) {
		// 由于两次扫描得到的值相等，因此按键行为有效
		if (matrixkey_current_value != matrixkey_final_value) {
			// 按键状态发生改变
          	change = matrixkey_final_value ^ matrixkey_current_value; // 异或，找出变化的位
          	matrixkey_final_value = matrixkey_current_value;     	// 保存两次扫描后相等的键值
          	for (i = 0; i < matrixkey_num; i++) {
              		if (change & (1 << i)) {
					// 仅处理变化的位
                  		if (((matrixkey_current_value & (1 << i)) >> i) == 0) {
                      		ret_flag = i; 					// 有键按下
					}else{
                      		ret_flag = (1 << 7) | i;				// 按键释放，最高位置1
                  		}
              		}
          	}
      	}
  	}
  	matrixkey_last_value = matrixkey_current_value;      		// 保存上次键值
	return ret_flag;
}


void matrixkey_init (void)                      // 板级初始化函数
{
    int i;
    
    for (i = 0; i < matrixkey_row_num; i++) {
      // 将所有行线设置为输入状态
        am_gpio_pin_cfg(g_matrixkey_row[i], AM_GPIO_INPUT | AM_GPIO_PULLUP);
    }
    for (i = 0; i < matrixkey_col_num; i++) {
        // 将所有列线设置为输出状态，并初始化为高电平
         am_gpio_pin_cfg(g_matrixkey_col[i], AM_GPIO_OUTPUT_INIT_HIGH);
    }
    matrixkey_final_value = matrixkey_last_value = matrixkey_read();
}

// 键盘扫描函数
uint8_t matrixkey_scan (void)                         
{

    uint32_t matrixkey_current_value = matrixkey_read();

    return matrixkey_val_process(matrixkey_current_value);
}

static pfn_matrixkey_callback_t   g_matrixkey_callback = NULL;
static void                      *g_matrixkey_arg      = NULL;
static am_softimer_t              g_matrixkey_timer;      // 按键扫描的软件定时器
static void matrixkey_softimer_callback(void *p_arg)
{
    uint8_t matrixkey_return = matrixkey_scan();
    if (matrixkey_return != 0xFF) {
        // 按键状态发生改变
        g_matrixkey_callback(g_matrixkey_arg, matrixkey_return);
    }
}
static void matrixkey_softimer_set (void)
{
    am_softimer_init(&g_matrixkey_timer, matrixkey_softimer_callback, NULL);
    am_softimer_start(&g_matrixkey_timer, 10);
}
void matrixkey_init_with_softimer(pfn_matrixkey_callback_t p_func, void *p_arg)
{
    matrixkey_init();
    g_matrixkey_callback  = p_func;
    g_matrixkey_arg       = p_arg;
    matrixkey_softimer_set();
}

// 数码管按键 一起扫描
uint8_t matrixkey_scan_with_digitron (void (*p_scan_func)(void))
{
    static uint8_t col = 0;
    uint8_t key_return = 0xff;
    static uint32_t matrixkey_value = 0xFFFFFFFF; 
    
    //digitron1_disp_scan();     // 数码管扫描
	
    if (p_scan_func != NULL) {
        p_scan_func();   							// 通过函数指针实现数码管扫描
    }

    matrixkey_kl_read(&matrixkey_value, col++);
    
    if (col == 2) {
        key_return = matrixkey_val_process(matrixkey_value);
        col = 0;
        matrixkey_value = 0xFFFFFFFF; 
    }
    
    return key_return;
}
/* end of file */
