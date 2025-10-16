#include "ti_msp_dl_config.h"
#include "stdio.h"
#include "string.h"
volatile unsigned int uart_data = 0;//接收
volatile unsigned int uart_data1 = 0;
#include "UserTask.h"
//峰峰值
uint16_t adcbuff[50] = {0};
uint16_t adcbuff1[50] = {0};
uint16_t maxval[10] = {0};
uint16_t minval[10] = {0};
uint16_t max = 0;
uint16_t min = 0;
uint16_t vpp = 0;
uint16_t vpp1 = 0;
uint16_t vpp_rem = 0;
uint16_t vpp_output1=0;
uint8_t  flag=1;
uint16_t adc1,adc2,adcmin_save;
uint8_t buf_index = 0;
uint8_t buf_index_flag=0;
uint8_t uart_buffer[32];
uint8_t uart0_start=0;
uint8_t mode=0;
volatile int v=0;
uint16_t button = 0;

uint16_t gAdcResult6[50];
uint16_t gAdcResult7[50];
uint16_t gAdcResult8[50];
uint16_t gAdcResult9[50];
uint16_t i0 = 0;
char output_buff[50] = {0};//串口文本
uint16_t cnt = 0,cnt1 = 0;

uint16_t f= 0;
uint8_t flag_type1,flag_type2;

uint16_t duan_or_kai=0;
uint8_t ch,fall=0;
uint16_t minus;
uint16_t length_rem;
uint8_t ch_duan=0;
uint8_t ch_kai=1;
uint8_t duanlu1=0,duanlu2=0,duanlu3=0,duanlu4=0;
uint8_t buf_indexx = 0;
uint8_t buf_index_flagx=0;
uint8_t uart_bufferx[32];
uint8_t js;
float loss;
float res;
float length_real;
void adc_getValue(void)
{
    DL_ADC12_startConversion(ADC12_0_INST);
    while (DL_ADC12_getStatus(ADC12_0_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult6[i0] =DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_6);
    while (DL_ADC12_getStatus(ADC12_0_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult7[i0] =DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_7);
    while (DL_ADC12_getStatus(ADC12_0_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult8[i0] =DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_8);
    while (DL_ADC12_getStatus(ADC12_0_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult9[i0] =DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_9);
    i0++;
    if (i0 >= 50) i0 = 0;
    DL_ADC12_enableConversions(ADC12_0_INST);
}
uint16_t gAdcResult0[50];
uint16_t gAdcResult1[50];
uint16_t gAdcResult2[50];
uint16_t gAdcResult3[50];
uint16_t gAdcResult4[50];
uint16_t gAdcResult5[50];
uint16_t i1 = 0;
void adc_getValue1(void)
{
    DL_ADC12_startConversion(ADC12_1_INST);
    while (DL_ADC12_getStatus(ADC12_1_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult0[i1] =DL_ADC12_getMemResult(ADC12_1_INST, DL_ADC12_MEM_IDX_0);
    while (DL_ADC12_getStatus(ADC12_1_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult1[i1] =DL_ADC12_getMemResult(ADC12_1_INST, DL_ADC12_MEM_IDX_1);
    while (DL_ADC12_getStatus(ADC12_1_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult2[i1] =DL_ADC12_getMemResult(ADC12_1_INST, DL_ADC12_MEM_IDX_2);
    while (DL_ADC12_getStatus(ADC12_1_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult3[i1] =DL_ADC12_getMemResult(ADC12_1_INST, DL_ADC12_MEM_IDX_3);
    while (DL_ADC12_getStatus(ADC12_1_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult4[i1] =DL_ADC12_getMemResult(ADC12_1_INST, DL_ADC12_MEM_IDX_4);
    while (DL_ADC12_getStatus(ADC12_1_INST)!= DL_ADC12_STATUS_CONVERSION_IDLE );gAdcResult5[i1] =DL_ADC12_getMemResult(ADC12_1_INST, DL_ADC12_MEM_IDX_5);
    i1++;
    if (i1 >= 50) i1 = 0;
    DL_ADC12_enableConversions(ADC12_1_INST);
}

uint16_t Average(uint16_t *datptr)
{
    uint32_t sum = 0;
    uint8_t cnt = 0;
    for(cnt = 0;cnt < 50;cnt ++)
    {
        sum += *(datptr + cnt);
    }
    return (uint16_t)(sum / 50);
}

//MAX
uint16_t Max(uint16_t *numptr,uint16_t num)
{
    uint16_t cnt = 0;
    uint16_t max = 0;
    for(cnt = 0;cnt < num;cnt ++)
    {
        if(numptr[cnt] > max){
            max = numptr[cnt];
        }
    }
    return max;
}
//MIN
uint16_t Min(uint16_t *numptr,uint16_t num)
{
    uint16_t cnt = 0;
    uint16_t min = 0;
    min = numptr[0];
    for(cnt = 0;cnt < num;cnt ++)
    {
        if(numptr[cnt] < min){
            min = numptr[cnt];
        }
    }
    return min;
}
//串口发送单个字符
void uart0_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_0_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_0_INST, ch);
}
//串口发送字符串
void uart0_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart0_send_char(*str++);
    }
}
//串口发送单个字符
void uart2_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_2_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_2_INST, ch);
}
//串口发送字符串
void uart2_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart2_send_char(*str++);
    }
}
volatile float lx=0;
void leixing(void)
{
    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_H_PIN);//继电器控制屏蔽层不接地
                       delay_cycles(120000000);
                       for(cnt = 0;cnt < 50;cnt++)
                              {
                                 adc_getValue();
                              }
                               vpp=Average(gAdcResult9);

                               if(vpp<2000)
                                   uart2_send_string("page2.t1.txt=\"UTP\"\xff\xff\xff");
                               else
                               {
                                   uart2_send_string("page2.t1.txt=\"SFTP\"\xff\xff\xff");
                               }
                               DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_H_PIN);//继电器控制屏蔽层接地
                               delay_cycles(80000000);//
}
//线序
float r;
void xianxudianzu(void)
{
    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_K_PIN);//直流
                        delay_cycles(20000000);//

                        //测量此时直流值，PA27
                                        for(cnt = 0;cnt < 50;cnt++)
                                        {
                                            adc_getValue();
                                        }
                                vpp = Average(gAdcResult6);;
                        if(vpp<2200)
                            {
                               fall++;
                               js++;//交损通道标志
                            }
                        if(fall==0)
                        {
                            uart2_send_string("page2.t0.txt=\"直连\"\xff\xff\xff");
                        }
                        else
                        {
                            uart2_send_string("page2.t0.txt=\"交叉\"\xff\xff\xff");
                        }
                        fall=0;

                        //电阻
                        vpp = Average(gAdcResult7);
                        r = (4022-vpp)*10.3/vpp;
                        r=r-lx;
                        sprintf(output_buff,"page2.x0.val=%d\xff\xff\xff",(uint16_t)(r*1000));
                        uart2_send_string(output_buff);
                       /* sprintf(output_buff,"1:%d\r\n",max);
                                      uart0_send_string(output_buff);
                                      sprintf(output_buff,"2:%d\r\n",min);
                                                    uart0_send_string(output_buff);
                                                    sprintf(output_buff,"vpp:%d\r\n",vpp);
                                                                  uart0_send_string(output_buff);*/
}

//交损
volatile float jz=0.9;
float loss;
void jiaosun(void)
{
    Set_xiezhen(300);
                        SweepTick = 0;
                        DDS_sweep(AD9959_CH3, &Sweep[0]);
                        DDS_sweep(AD9959_CH2, &Sweep[1]);
                       // DDS_sweep(AD9959_CH3, &Sweep[2]);
                        DDS_update();
                        delay_cycles(80000000);//

                        if(js==0)//直连
                        {//12,12
                            DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_M_PIN);
                            DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_N_PIN);
                            DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_CF_PIN);
                            DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_L_PIN);
                        }
                        else //交叉
                        {//12,36
                            DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_M_PIN);
                            DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_N_PIN);
                            DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_CF_PIN);
                            DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_L_PIN);
                        }
                        js=0;
                        DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_K_PIN);//交流
                        delay_cycles(40000000);//
                        //测量此时8310值
                        for(cnt = 0;cnt < 50;cnt ++)
                        {
                            adc_getValue1();
                        }

                                vpp_output1 = Average(gAdcResult1);
                                vpp = Average(gAdcResult2);
                          loss=(vpp_output1-31-vpp)*3.3/4095/24*1000;
                          loss=loss*jz;

                          sprintf(output_buff,"page2.x1.val=%d\xff\xff\xff",(int16_t)(loss*1000));
                          uart2_send_string(output_buff);
                          DDS_init();
}
void chuanrao(void)
{
    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_K_PIN);//直流
                         delay_cycles(20000000);//

                         //测量此时直流值，PA27
                                         for(cnt = 0;cnt < 50;cnt++)
                                         {
                                             adc_getValue();
                                         }
                                 vpp = Average(gAdcResult6);
                         if(vpp<2200)
                             {
                                fall++;
                                js++;//交损通道标志
                             }
                         Set_xiezhen(300);
                                             SweepTick = 0;
                                             DDS_sweep(AD9959_CH3, &Sweep[0]);
                                             DDS_sweep(AD9959_CH2, &Sweep[1]);
                                            // DDS_sweep(AD9959_CH3, &Sweep[2]);
                                             DDS_update();
                                             delay_cycles(80000000);//

                                             if(js==0)//直连
                                                                       {//12,36
                                                                           DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_M_PIN);
                                                                           DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_N_PIN);
                                                                           DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_CF_PIN);
                                                                           DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_L_PIN);
                                                                       }
                                                                       else //交叉
                                                                       {//12,12
                                                                           DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_M_PIN);
                                                                           DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_N_PIN);
                                                                           DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_CF_PIN);
                                                                           DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_L_PIN);
                                                                       }
                                             js=0;
                                             DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_K_PIN);//交流
                                             delay_cycles(40000000);//
                                             //测量此时8310值
                                             for(cnt = 0;cnt < 50;cnt ++)
                                             {
                                                 adc_getValue1();
                                             }

                                                     vpp_output1 = Average(gAdcResult1);
                                                     vpp = Average(gAdcResult2);
                                               loss=(vpp_output1-31-vpp)*3.3/4095/24*1000;
                                               loss*=jz;

                                               sprintf(output_buff,"page2.x2.val=%d\xff\xff\xff",(int16_t)(loss*1000));
                                               uart2_send_string(output_buff);
                                               DDS_init();
}
void duanluplace(void)
{
    //测输入端2，6，5，7通道4个ADC的值，判断开路断路，记录短路情况，测量短路位置
    //测量此时直流值，PA25
                    for(cnt = 0;cnt < 50;cnt++)
                    {
                        adc_getValue();
                    }
            vpp=Average(gAdcResult8);
    if(vpp>=3000)
          {duanlu4=1;}
    for(cnt = 0;cnt < 50;cnt++)
    {
        adc_getValue1();
    }
vpp = Average(gAdcResult3);
    if(vpp>=3000)
          {
          duanlu1=1;
          }
    vpp=Average(gAdcResult4);//PB19
    if(vpp>=3000)
          {duanlu2=1;}
vpp=Average(gAdcResult5);//PA21
    if(vpp>=3000)
          {duanlu3=1;}
}
void duanlulength(void)
{
    //根据短路情况选择测量通道
 if(duanlu1==1||duanlu2==1||duanlu3==1||duanlu4==1)
 {
     if(duanlu1==1)
     {ch_duan=1;
     ch_kai=2;
     uart2_send_string("page1.t4.txt=\"12短路\"\xff\xff\xff");}

     else if(duanlu2==1)
     {ch_duan=2;
     ch_kai=1;
     uart2_send_string("page1.t4.txt=\"36短路\"\xff\xff\xff");
     }
     else if(duanlu3==1)
     {ch_duan=3;
     ch_kai=1;
     uart2_send_string("page1.t4.txt=\"45短路\"\xff\xff\xff");
     }
     else if(duanlu4==1)
     {uart2_send_string("page1.t4.txt=\"78短路\"\xff\xff\xff");
         ch_duan=4;
     ch_kai=1;
     }
     duanlu1=0;
     duanlu2=0;
     duanlu3=0;
     duanlu4=0;
     uart2_send_string("page1.t3.txt=\"短路\"\xff\xff\xff");
     duan_or_kai=1;
        fourchoose1(ch_duan);
         Set_Saopin_SFTP();
            flag_saoduanlu=1;
          testfirst_flag=3;
       adc_max=0;
       adc_min=65535;
       memset(length_history, 0, sizeof(length_history));
      hist_idx = 0;              // 当前写入位置
       valid_count = 0;           // 有效数据计数
 }
 else
{
     ch_duan=0;
     ch_kai=1;
     uart2_send_string("page1.t3.txt=\"开路\"\xff\xff\xff");
             uint16_t duanlength=0;
               sprintf(output_buff,"page1.x2.val=%d\xff\xff\xff",duanlength);
              uart2_send_string(output_buff);
}
}

int main(void)
{
    SYSCFG_DL_init();


    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);//清除串口中断标志
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);//使能串口中断
    NVIC_ClearPendingIRQ(UART_2_INST_INT_IRQN);//清除串口中断标志
        NVIC_EnableIRQ(UART_2_INST_INT_IRQN);//使能串口中断
        DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_M_PIN);
        DL_GPIO_setPins(GPIO_GRP_PORT, GPIO_GRP_N_PIN);
        DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_CF_PIN);
        DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_L_PIN);
        DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_G_PIN);

    UserTask_init();


    /* Initialize capture global states */


    while (1)
    {
        UserTask_loop();

                for(cnt = 0;cnt < 50;cnt++)//PA17
                {
                       adc_getValue1();
                   }
          vpp=Average(gAdcResult2);
        Test_min(vpp,duan_or_kai);//找到最小点

        if(testfirst_flag==0&&flag_saolength==1)
        {
            length_real=length*xishu-xishu_k;
            sprintf(output_buff,"page1.x1.val=%d\xff\xff\xff",(uint16_t)(length_real));
          uart2_send_string(output_buff);
          flag_saolength=0;
          ch_kai=1;
        }
        if(testfirst_flag==0&&flag_saoduanlu==1)
        {
            length_real=length*xishu;
            sprintf(output_buff,"page1.x2.val=%d\xff\xff\xff",(uint16_t)(length_real));
          uart2_send_string(output_buff);
          flag_saoduanlu=0;
          ch_duan=0;
          duan_or_kai=0;
        }

                switch(button)
                {
                case 0://默认情况
                    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_H_PIN);//继电器控制屏蔽层接地
                    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_F_PIN);//给屏蔽层输信号
                    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_K_PIN);//通交流
                    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_A12_PIN);//通3V3
                    break;

                case 3://测长度
                    fourchoose1(ch_kai);
                    Set_Saopin();
                    testfirst_flag=2;
                    flag_saolength=1;
                    adc_min=65535;
                    adc_max=0;
                    memset(length_history, 0, sizeof(length_history));
                   hist_idx = 0;              // 当前写入位置
                    valid_count = 0;           // 有效数据计数
                    DDS_init();
                 //uart2_send_string("page.t3.txt=\"正在测量\"\xff\xff\xff");
                    button=0;
                    break;
                case 4:
                    //直流方案，单端测量，输入直流端1，3，4，8接直流

                    duanlulength();

                            button=0;
                       break;
                case 2:
                    leixing();
                    xianxudianzu();
                    jiaosun();
                    button=0;
                    break;
                case 5:
                    chuanrao();
                    button=0;
                    break;
                case 1:
                    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_K_PIN);//控制通直流
                    delay_cycles(160000000);
                    duanluplace();
                    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_K_PIN);//控制通交流
                    button=0;
                    break;
                }
}

}
//串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_0_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //将发送过来的数据保存在变量中
            uart_data = DL_UART_Main_receiveData(UART_0_INST);
            break;

        default://其他的串口中断
            break;
    }
}
void UART_2_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_2_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //将发送过来的数据保存在变量中
            uart_data1 = DL_UART_Main_receiveData(UART_2_INST);
            //将保存的数据再发送出去
            if(uart_data1==0x55)//帧头
            {
                buf_index_flag=1;
            }
            else if(uart_data1==0xff)//帧尾
            {
                buf_index_flag=0;
                buf_index=0;
                if(uart_buffer[1]==0x01)button=1;//单端
                else if(uart_buffer[1]==0x02)button=2;//双端
                else if(uart_buffer[1]==0x10)button=3;//长度
                else if(uart_buffer[1]==0x11)button=4;//短路
                else if(uart_buffer[1]==0x12)button=5;//串扰
                else button=0;
            }
            if(uart_data1==0xfe)//帧头
                                    {
                                        buf_index_flagx=1;
                                    }
                                    else if(uart_data1==0xfd)//帧尾
                                    {
                                        buf_index_flagx=0;
                                        buf_indexx=0;
                                        lx=(uint8_t)uart_bufferx[1]/100.00;

                                    }
            if(uart_data1==0xfb)//帧头
                                    {
                                        buf_index_flagx=1;
                                    }
                                    else if(uart_data1==0xfc)//帧尾
                                    {
                                        buf_index_flagx=0;
                                        buf_indexx=0;
                                        jz=(uint8_t)uart_bufferx[1]/100.00;

                                    }

            //储存
            if(buf_index_flag==1)uart_buffer[buf_index++] = uart_data1;
            if(buf_index_flagx==1)uart_bufferx[buf_indexx++] = uart_data1;
            break;
        default://其他的串口中断
            break;
    }
}

