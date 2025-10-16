#include "UserTask.h"
#include "ti_msp_dl_config.h"
/**
 * 引脚连接方式
 * System Configuration: GPIO组名: GPIO_DDS
 * MSPM0 GPIO引脚 -> AD9959模块引脚
 *
 * PA7DDS_SCLK   -> AD9959_SCLK
 * PB2DDS_SDIO0  -> AD9959_SDIO0
 * PB3DDS_CS     -> AD9959_CS
 * PA8DDS_UPDATE -> AD9959_UPDATE
 * PA9DDS_RST    -> AD9959_RST
 * GND        -> GND
 *
 * AD9959模块其余数据引脚均接GND
*/

DDS_SingleToneParam_t SingleTone[4] = {0};
DDS_SweepParam_t Sweep[3] = {0};

volatile uint16_t SweepTick = 0;
volatile uint16_t delay_tick=0;;
volatile float freq_xiezhen;
volatile float freq_xiezhen_cu;
uint8_t xiezhen_flag=0;
float length=0;
float lengthjie=0;
uint16_t adc_min=65535;
uint16_t adc_value;
uint8_t testfirst_flag=0;
float  StepSet=100E3F;
uint16_t wavebuff[200],wavebuff1[200];
uint8_t DDS_SWEEP_TIME=20;
uint8_t flag_FTP=0;
uint8_t flag_maxadc=0;
uint16_t adc_max=0;
uint8_t flag_adcfall=0;
float xishu=1;
float xishu_k=0;
float length_history[WINDOW_SIZE]; // 存储历史lengthjie
uint8_t hist_idx = 0;              // 当前写入位置
uint8_t valid_count = 0;           // 有效数据计数
uint8_t flag_saoduanlu=0;
uint8_t flag_saolength=0;
void delay(uint16_t delay_tick1)
{
    delay_tick=0;
    while(delay_tick<delay_tick1);
    }
float log_base(float x,float base)
{
    return logf(x)/logf(base);}
void UserTask_init(void) {
    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_H_PIN);
    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_K_PIN);
    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_F_PIN);
    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_CF_PIN);
    initSingleToneParam();
    initSweepParam();

    DDS_init();

    DDS_singleTone(AD9959_CH0, &SingleTone[0]);
    DDS_singleTone(AD9959_CH3, &SingleTone[0]);
    DDS_singleTone(AD9959_CH2, &SingleTone[2]);
 //   DDS_singleTone(AD9959_CH3, &SingleTone[3]);
    DDS_update();

    Tick_delay(5000);

    DDS_initSweep(AD9959_CH3, &Sweep[0]);
    DDS_initSweep(AD9959_CH2, &Sweep[1]);
 //   DDS_initSweep(AD9959_CH3, &Sweep[2]);
    DDS_update();
}
void fourchoose1(uint8_t ch)
{
    switch(ch)
                        {
   case 1:
     DL_GPIO_setPins(GPIO_GRP_PORT,GPIO_GRP_M_PIN);
       DL_GPIO_setPins(GPIO_GRP_PORT,GPIO_GRP_N_PIN);
               break;
         case 2:
          DL_GPIO_setPins(GPIO_GRP_PORT,GPIO_GRP_M_PIN);
         DL_GPIO_clearPins(GPIO_GRP_PORT,GPIO_GRP_N_PIN);
           break;
        case 3:
            DL_GPIO_clearPins(GPIO_GRP_PORT,GPIO_GRP_M_PIN);
           DL_GPIO_setPins(GPIO_GRP_PORT,GPIO_GRP_N_PIN);
           break;
      case 4:
                    DL_GPIO_clearPins(GPIO_GRP_PORT,GPIO_GRP_M_PIN);
               DL_GPIO_clearPins(GPIO_GRP_PORT,GPIO_GRP_N_PIN);
                break;
             default:
                     break;}
}
void UserTask_loop(void) {
    if (SweepTick >= DDS_SWEEP_TIME) {
        SweepTick = 0;
        if(flag_saolength==1&&xiezhen_flag==1)
        {
            Sweep[0].start = freq_xiezhen_cu-50*StepSet;
            Sweep[0].freq = freq_xiezhen_cu-50*StepSet;
            Sweep[0].now = freq_xiezhen_cu-50*StepSet;
            Sweep[0].amp=1023;
            Sweep[0].step=StepSet;
            Sweep[0].end = 50E6F;
            Sweep[0].type = DDS_SWEEP_FREQ;          // 扫频
            Sweep[0].method = DDS_SWEEP_LINEAR; // 对数扫描

            Sweep[1].start = freq_xiezhen_cu-50*StepSet;
            Sweep[1].freq = freq_xiezhen_cu-50*StepSet;
            Sweep[1].now = freq_xiezhen_cu-50*StepSet;
            Sweep[1].amp=1023;
            Sweep[1].step=StepSet;
            Sweep[1].end = 50E6F;
            Sweep[1].type = DDS_SWEEP_FREQ;          // 扫频
            Sweep[1].method = DDS_SWEEP_LINEAR; // 对数扫描
            xiezhen_flag=0;
        }
        if(flag_saoduanlu==1)
        {
            if(xiezhen_flag==1)
            {
            Sweep[0].start = freq_xiezhen_cu-50*StepSet;
            Sweep[0].freq = freq_xiezhen_cu-50*StepSet;
            Sweep[0].now = freq_xiezhen_cu-50*StepSet;
            Sweep[0].amp=1023;
            Sweep[0].step=StepSet;
            Sweep[0].end = 50E6F;
            Sweep[0].type = DDS_SWEEP_FREQ;          // 扫频
            Sweep[0].method = DDS_SWEEP_LINEAR; // 对数扫描

            Sweep[1].start = freq_xiezhen_cu-50*StepSet;
            Sweep[1].freq = freq_xiezhen_cu-50*StepSet;
            Sweep[1].now = freq_xiezhen_cu-50*StepSet;
            Sweep[1].amp=1023;
            Sweep[1].step=StepSet;
            Sweep[1].end = 50E6F;
            Sweep[1].type = DDS_SWEEP_FREQ;          // 扫频
            Sweep[1].method = DDS_SWEEP_LINEAR; // 对数扫描
            xiezhen_flag=0;
            }
        }
        DDS_sweep(AD9959_CH3, &Sweep[0]);
        DDS_sweep(AD9959_CH2, &Sweep[1]);
 //       DDS_sweep(AD9959_CH3, &Sweep[2]);
        DDS_update();
    }
}
void Test_min(uint16_t adc_value,uint16_t duan_or_kai)//短路开路两种方式
{
    if(duan_or_kai==0)//测开路长度
    {

    if(adc_min>adc_value&&testfirst_flag==2)
    {
         freq_xiezhen_cu=Sweep[0].now;
         lengthjie=3E7F*5827/(freq_xiezhen_cu*4);
         if(lengthjie<=60000&&lengthjie>=500)//保证数据有效性
         {
                         adc_min = adc_value;
         }
    }
    if((adc_min<=adc_value-20)&&testfirst_flag==2)
    {
            testfirst_flag=1;
            adc_min=65535;
            xiezhen_flag=1;
            lengthjie=3E7F*5827/(freq_xiezhen_cu*4);
            if(lengthjie>40000)
            {
                StepSet=1E3F;
                xishu=50/48.563;
                xishu_k=0;
            }
            else if(lengthjie>=15000)
    {StepSet=5E3F;
    xishu=20/19.696;
    xishu_k=0;}
            else if(lengthjie>=8000)
            {StepSet=10E3F;
            xishu=10/9.614;
            xishu_k=0;}
            else if(lengthjie>=2800)
            {StepSet=50E3F;
            xishu=0.96268;
            xishu_k=0;}
            else {StepSet=100E3F;
            xishu=1.1/1.335;
            xishu_k=40;}
    }
    if(adc_min>adc_value&&testfirst_flag==1)
    {
         freq_xiezhen=Sweep[0].now;
         lengthjie=3E7F*5827/(freq_xiezhen*4);
         if(lengthjie<=60000&&lengthjie>=500)
         {
             // 存入历史窗口
                         length_history[hist_idx] = lengthjie;
                         hist_idx = (hist_idx + 1) % WINDOW_SIZE;
                         if (valid_count < WINDOW_SIZE) valid_count++;

                         // 计算滑动平均
                         float sum = 0;
                         uint8_t actual_window = (valid_count < WINDOW_SIZE) ? valid_count : WINDOW_SIZE;
                         for (int i = 0; i < actual_window; i++) {
                             sum += length_history[i];
                         }
                         length = sum / actual_window;  // 更新为平均值
                         adc_min = adc_value;
         }
    }
    if(xiezhen_flag==0&&Sweep[0].freq>=(freq_xiezhen_cu+50*StepSet)&&testfirst_flag==1)
    {
            testfirst_flag=0;
    }

    }

    else//短路
    {
        if(adc_max<adc_value&&testfirst_flag==3)
        {
            freq_xiezhen_cu=Sweep[0].now;
            lengthjie=3E7F*5827/(freq_xiezhen_cu*4);
            if(lengthjie<=60000&&lengthjie>=500)//保证数据有效性
            {
                            adc_max = adc_value;
            }
        }
        if(adc_max-20>adc_value&&testfirst_flag==3)
        {
                testfirst_flag=2;
        }
        if(adc_min>adc_value&&testfirst_flag==2)
        {
             freq_xiezhen_cu=Sweep[0].now;
             lengthjie=3E7F*6083/(freq_xiezhen_cu*2);
             if(lengthjie<=60000&&lengthjie>=500)
             {
              adc_min=adc_value;
             }
        }
        if(adc_min<=(adc_value-20)&&testfirst_flag==2)
        {
            testfirst_flag=1;
            adc_min=65535;
            xiezhen_flag=1;
            lengthjie=3E7F*6083/(freq_xiezhen_cu*2);
                    if(lengthjie>40000)
                    {
                        StepSet=1E3F;
                        xishu=50/48.563;
                        xishu_k=0;
                    }
                    else if(lengthjie>=15000)
            {StepSet=5E3F;
            xishu=0.96268;
            xishu_k=0;}
                    else if(lengthjie>=8000)
                    {StepSet=10E3F;
                    xishu=10/9.614;
                    xishu_k=0;}
                    else if(lengthjie>=2800)
                    {StepSet=50E3F;
                    xishu=0.844;
                    xishu_k=0;}
                    else {StepSet=100E3F;
                    xishu=1.1/1.335;
                    xishu_k=40;}

        }
        if(adc_min>adc_value&&testfirst_flag==1)
        {
             freq_xiezhen=Sweep[0].now;
             lengthjie=3E7F*6083/(freq_xiezhen*2);
             if(lengthjie<=60000&&lengthjie>=500)
             {
                 // 存入历史窗口
                             length_history[hist_idx] = lengthjie;
                             hist_idx = (hist_idx + 1) % WINDOW_SIZE;
                             if (valid_count < WINDOW_SIZE) valid_count++;

                             // 计算滑动平均
                             float sum = 0;
                             uint8_t actual_window = (valid_count < WINDOW_SIZE) ? valid_count : WINDOW_SIZE;
                             for (int i = 0; i < actual_window; i++) {
                                 sum += length_history[i];
                             }
                             length = sum / actual_window;  // 更新为平均值
                             adc_min = adc_value;
             }
        }
        if(xiezhen_flag==0&&Sweep[0].freq>=(freq_xiezhen_cu+50*StepSet)&&testfirst_flag==1)
        {
                testfirst_flag=0;
        }

    }
    }
void Set_xiezhen(int DelayCount)
{
    Sweep[0].start = 1E5F;
    Sweep[0].end = 50E6F;
    Sweep[0].freq = 100E3F*DelayCount;
    Sweep[0].now = 100E3F*DelayCount;
    Sweep[0].amp=1023;
    Sweep[0].step=1;
    Sweep[0].phase = 0x0000;

    Sweep[1].start = 1E5F;
    Sweep[1].end = 50E6F;
    Sweep[1].freq = 100E3F*DelayCount;
    Sweep[1].now = 100E3F*DelayCount;
    Sweep[1].amp=1023;
    Sweep[1].step=1;
    Sweep[1].phase = 0x2000;
    }
void Set_Saopin()
{
    Sweep[0].start = 5E5F;
    Sweep[0].freq = 5E5F;
    Sweep[0].now = 5E5F;
    Sweep[0].amp=1023;
    Sweep[0].step=1.027F;
    Sweep[0].end = 50E6F;
    Sweep[0].type = DDS_SWEEP_FREQ;          // 扫频
    Sweep[0].method = DDS_SWEEP_LOGARITHMIC; // 对数扫描


    Sweep[1].start = 5E5F;
    Sweep[1].freq = 5E5F;
    Sweep[1].now = 5E5F;
    Sweep[1].amp=1023;
    Sweep[1].step=1.027F;
    Sweep[1].end = 50E6F;
    Sweep[1].type = DDS_SWEEP_FREQ;          // 扫频
    Sweep[1].method = DDS_SWEEP_LOGARITHMIC; // 对数扫描
    }
void Set_Saopin_SFTP()
{
    Sweep[0].start = 5E5F;
    Sweep[0].freq = 5E5F;
    Sweep[0].now = 5E5F;
    Sweep[0].amp=1023;
    Sweep[0].step=1.027F;
    Sweep[0].end = 100E6F;
    Sweep[0].type = DDS_SWEEP_FREQ;          // 扫频
    Sweep[0].method = DDS_SWEEP_LOGARITHMIC; // 对数扫描


    Sweep[1].start = 5E5F;
    Sweep[1].freq = 5E5F;
    Sweep[1].now = 5E5F;
    Sweep[1].amp=1023;
    Sweep[1].step=1.027F;
    Sweep[1].end = 100E6F;
    Sweep[1].type = DDS_SWEEP_FREQ;          // 扫频
    Sweep[1].method = DDS_SWEEP_LOGARITHMIC; // 对数扫描
    // 1kHz ~ 100kHz 线性扫频
   /* Sweep[0].freq = 1E5F;               // 频率10MHz
    Sweep[0].amp = 1023;                // 幅度最大(1023)
    Sweep[0].phase = 0x0000;                 // 相位0度(0)
    Sweep[0].start = 1E5F;              // 起始频率10MHz
    Sweep[0].step = 200E3F;                // 步进频率1kHz
    Sweep[0].end = 80E6F;              // 终止频率20MHz
    Sweep[0].type = DDS_SWEEP_FREQ;     // 扫频
    Sweep[0].method = DDS_SWEEP_LINEAR; // 线性扫描

    Sweep[1].freq = 1E5F;               // 频率10MHz
    Sweep[1].amp = 1023;                // 幅度最大(1023)
    Sweep[1].phase = 0x2000;                 // 相位0度(0)
    Sweep[1].start = 1E5F;              // 起始频率10MHz
    Sweep[1].step = 200E3F;                // 步进频率1kHz
    Sweep[1].end = 80E6F;              // 终止频率20MHz
    Sweep[1].type = DDS_SWEEP_FREQ;     // 扫频
    Sweep[1].method = DDS_SWEEP_LINEAR; // 线性扫描*/
    }
void initSingleToneParam(void) {
    int i;
    for (i = 0; i < 4; i++) {
        SingleTone[i].freq = 1000; // 频率1kHz
        SingleTone[i].amp = 1023;  // 幅度最大(1023)
    }
    SingleTone[0].phase = 0x0000; // 相位0度(0)
    SingleTone[1].phase = 0x1000; // 相位90度(4096)
    SingleTone[2].phase = 0x2000; // 相位180度(8192)
    SingleTone[3].phase = 0x3000; // 相位270度(12288)
}


void initSweepParam(void) {


    // 1kHz ~ 100kHz 线性扫频
    Sweep[0].freq = 1E5F;               // 频率10MHz
    Sweep[0].amp = 1023;                // 幅度最大(1023)
    Sweep[0].phase = 0x0000;                 // 相位0度(0)
    Sweep[0].start = 1E5F;              // 起始频率10MHz
    Sweep[0].step = 100E3F;                // 步进频率1kHz
    Sweep[0].end = 50E6F;              // 终止频率20MHz
    Sweep[0].type = DDS_SWEEP_FREQ;     // 扫频
    Sweep[0].method = DDS_SWEEP_LINEAR; // 线性扫描*/

    Sweep[1].freq = 1E5F;               // 频率10MHz
    Sweep[1].amp = 1023;                // 幅度最大(1023)
    Sweep[1].phase = 0x2000;                 // 相位0度(0)
    Sweep[1].start = 1E5F;              // 起始频率10MHz
    Sweep[1].step = 100E3F;                // 步进频率1kHz
    Sweep[1].end = 50E6F;              // 终止频率20MHz
    Sweep[1].type = DDS_SWEEP_FREQ;     // 扫频
    Sweep[1].method = DDS_SWEEP_LINEAR; // 线性扫描


    // 1 ~ 1000 线性扫幅
    Sweep[2].freq = 1E3F;               // 频率1kHz
    Sweep[2].amp = 1;                   // 幅度1
    Sweep[2].phase = 0;                 // 相位0度(0)
    Sweep[2].start = 1;                 // 起始幅度1
    Sweep[2].step = 10;                 // 步进幅度10
    Sweep[2].end = 1000;                // 终止幅度1000
    Sweep[2].type = DDS_SWEEP_AMP;      // 扫幅
    Sweep[2].method = DDS_SWEEP_LINEAR; // 线性扫描
}
