#include "ametal.h"
#include "am_zlg237.h"
#include "zlg237_pin.h"
#include "zlg237_clk.h"
#include "am_clk.h"
#include "am_zlg237_inst_init.h"
#include "am_baudrate_detect.h"
/**
 * \name 自动波特率检测宏定义
 * @{
 */

/** \brief 需要用到的定时器位数 */
#define     TIMER_WIDTH           16
static am_cap_handle_t            cap_handle;
/** @} */

/*******************************************************************************
 * 自动波特率检测配置
 ******************************************************************************/


/** \brief 自动波特率的平台初始化 */
void __zlg237_plfm_autobaud_init (void)
{
    amhw_zlg_tim_prescale_set((amhw_zlg_tim_t *)ZLG237_TIM2_BASE, (uint16_t)8);
}

/** \brief 自动波特率的平台解初始化  */
void __zlg237_plfm_autobaud_deinit (void)
{
    am_zlg237_tim1_cap_inst_deinit (cap_handle);
}

/** \brief 自动波特率的设备信息实例 */
static am_baudrate_detect_devinfo_t __g_zlg237_baudrate_detect_devinfo = {
    PIOA_10,                           /**< \brief 被捕获引脚 */
    2,                                 /**< \brief CAP捕获通道号 */
    TIMER_WIDTH,                       /**< \brief TIMER定时器位数 */
    10,                                /**< \brief 接收一次数据的超时时间(ms)*/
    AM_CAP_TRIGGER_BOTH_EDGES,         /**< \brief CAP捕获触发方式 */
    __zlg237_plfm_autobaud_init,  /**< \brief 平台初始化函数 */
    __zlg237_plfm_autobaud_deinit,/**< \brief 平台解初始化函数 */
};

/** \brief 自动波特率功能的设备实例 */
am_baudrate_detect_dev_t  __g_zlg237_baudrate_detect_dev;

/** \brief 实例初始化，获得自动波特率服务句柄 */
am_baudrate_detect_handle_t am_zlg237_baudrate_detect_inst_init (void)
{
    cap_handle = am_zlg237_tim1_cap_inst_init();

    return am_baudrate_detect_init(&__g_zlg237_baudrate_detect_dev,
                                   &__g_zlg237_baudrate_detect_devinfo,
                                    cap_handle);
}

/** \brief 实例解初始化*/
void am_zlg237_baudrate_detect_inst_deinit(am_baudrate_detect_handle_t handle)
{
    am_baudrate_detect_deinit(handle);
}

/* end of file */
