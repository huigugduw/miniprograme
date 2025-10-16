#include "Tick.h"

volatile uint32_t Tick = 0;

/**
 * @brief 延时函数（使用SysTick中断计时）
 * @param t 延时时间(ms)
*/
void Tick_delay(uint32_t t) {
    uint32_t tEnd = Tick + t;
    while (Tick < tEnd);
}

// SysTick中断处理函数（1ms触发一次）
void SysTick_Handler(void) {
    Tick++;
    SweepTick++;
        delay_tick++;
}
