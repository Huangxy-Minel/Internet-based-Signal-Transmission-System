#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "sdram.h"
#include "lan8720.h"
#include "timer.h"
#include "mpu.h"
#include "usmart.h"
#include "malloc.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "udp_demo.h"
#include "myspi.h"
/************************************************
 ALIENTEK 阿波罗STM32F7开发板 网络实验3
 基于RAW API的UDP实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

extern u8 udp_demo_flag;  //UDP 测试全局状态标记变量

//加载UI
//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI

int main(void)
{
    u8 key;  
    Write_Through();                //开启强制透写！
    MPU_Memory_Protection();        //保护相关存储区域
    Cache_Enable();                 //打开L1-Cache
    
    HAL_Init();				        //初始化HAL库
    Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    delay_init(216);                //延时初始化
		uart_init(115200);		        //串口初始化
		usmart_dev.init(108); 		    //初始化USMART
    LED_Init();                     //初始化LED
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
		SPI_Init();
    //LCD_Init();                     //初始化LCD
    //PCF8574_Init();                 //初始化PCF8574
    my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMDTCM);		    //初始化DTCM内存池		

    TIM3_Init(1000-1,1080-1);        //定时器3初始化，定时器时钟为90M，分频系数为900-1，
                                    //所以定时器3的频率为108M/1080=100K，自动重装载为1000-1，那么定时器周期就是10ms
	while(lwip_comm_init())         //lwip初始化
	{
        delay_ms(500);
	}

    delay_ms(1000);			//延时1s
	udp_demo_test();  		//UDP 模式
	while(1)
	{
        lwip_periodic_handle();	//LWIP内核需要定时处理的函数
        key=KEY_Scan(0);
		if(key==KEY1_PRES)		    //按KEY1键建立连接
		{
			udp_demo_test();		//当断开连接后,调用udp_demo_test()函数
		}
		delay_ms(10);
	}								  	       
}
