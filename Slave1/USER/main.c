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
 ALIENTEK ������STM32F7������ ����ʵ��3
 ����RAW API��UDPʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

extern u8 udp_demo_flag;  //UDP ����ȫ��״̬��Ǳ���

//����UI
//mode:
//bit0:0,������;1,����ǰ�벿��UI
//bit1:0,������;1,���غ�벿��UI

int main(void)
{
    u8 key;  
    Write_Through();                //����ǿ��͸д��
    MPU_Memory_Protection();        //������ش洢����
    Cache_Enable();                 //��L1-Cache
    
    HAL_Init();				        //��ʼ��HAL��
    Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
    delay_init(216);                //��ʱ��ʼ��
		uart_init(115200);		        //���ڳ�ʼ��
		usmart_dev.init(108); 		    //��ʼ��USMART
    LED_Init();                     //��ʼ��LED
    KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //��ʼ��SDRAM
		SPI_Init();
    //LCD_Init();                     //��ʼ��LCD
    //PCF8574_Init();                 //��ʼ��PCF8574
    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMDTCM);		    //��ʼ��DTCM�ڴ��		

    TIM3_Init(1000-1,1080-1);        //��ʱ��3��ʼ������ʱ��ʱ��Ϊ90M����Ƶϵ��Ϊ900-1��
                                    //���Զ�ʱ��3��Ƶ��Ϊ108M/1080=100K���Զ���װ��Ϊ1000-1����ô��ʱ�����ھ���10ms
	while(lwip_comm_init())         //lwip��ʼ��
	{
        delay_ms(500);
	}

    delay_ms(1000);			//��ʱ1s
	udp_demo_test();  		//UDP ģʽ
	while(1)
	{
        lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
        key=KEY_Scan(0);
		if(key==KEY1_PRES)		    //��KEY1����������
		{
			udp_demo_test();		//���Ͽ����Ӻ�,����udp_demo_test()����
		}
		delay_ms(10);
	}								  	       
}
