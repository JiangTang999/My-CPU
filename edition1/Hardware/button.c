#include "stm32f10x.h"
#include "button.h"
#include "sys.h" 
#include "delay.h"

// 按钮初始化函数
void button_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 启用 GPIOA 时钟
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15; // 假设使用 PA15 作为按钮引脚
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// 按钮扫描函数
// 返回按钮状态
// 返回值: 0 - 按钮按下, 1 - 按钮未按下
int button_Scan(void) { 
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == Bit_RESET) { // 低电平表示按钮被按下
        delay_ms(20); // 去抖动延时
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == Bit_RESET) { // 再次检测确认
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == Bit_RESET); // 等待按键释放
            return 0;
        }
    } 
    return 1; // 按钮未按下
}
