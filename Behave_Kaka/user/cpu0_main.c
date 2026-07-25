
#include "zf_common_headfile.h"
#include "dgconfig.h"
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

// **************************** 代码区域 ****************************

int core0_main(void)
{
        clock_init(); // 获取时钟频率<务必保留>
        debug_init(); // 初始化默认调试串口
        // 此处编写用户代码 例如外设初始化代码等
        Base_Peripheral_Init();
        Module_Peripheral_Init();
        cpu_wait_event_ready(); // 等待所有核心初始化完毕
        while (TRUE)
        {
                // 此处编写需要循环执行的代码
                system_delay_ms(1);
        }
}

#pragma section all restore
