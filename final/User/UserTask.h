#ifndef __USER_TASK_H__
#define __USER_TASK_H__

#include <stdint.h>
#include "ti_msp_dl_config.h"
#include "Tick.h"
#include "DDS.h"

//#define DDS_SWEEP_TIME 100 // DDS鎵弿闂撮殧(ms)
#define WINDOW_SIZE 5  // 滑动窗口大小（奇数，如3,5,7）

extern volatile uint16_t SweepTick;
extern volatile uint16_t delay_tick;
extern float length;
void delay(uint16_t delay_tick1);
void UserTask_init(void);
void UserTask_loop(void);
void Test_min(uint16_t vpp,uint16_t duan_or_kai);
void Set_xiezhen(int DelayCount);
void Set_Saopin(void);
void Set_Saopin_SFTP(void);
void fourchoose1(uint8_t ch);
float log_base(float x,float base);

void initSingleToneParam(void);
void initSweepParam(void);
extern volatile float freq_xiezhen;
extern uint8_t testfirst_flag;
extern float StepSet;
extern float xishu,xishu_k;
extern DDS_SweepParam_t Sweep[3];
extern uint16_t wavebuff[200],wavebuff1[200];
extern uint16_t adc_min,adc_max;
extern uint8_t flag_adcfall;
extern uint8_t DDS_SWEEP_TIME;
extern uint8_t flag_FTP;
extern uint8_t flag_maxadc;

extern float length_history[WINDOW_SIZE]; // 存储历史lengthjie
extern uint8_t hist_idx;              // 当前写入位置
extern uint8_t valid_count;           // 有效数据计数
extern uint8_t flag_saoduanlu;
extern uint8_t flag_saolength;
#endif /* #ifndef __USER_TASK_H__ */
