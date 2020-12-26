#include "lan8720.h"
#include "pcf8574.h"
#include "lwip_comm.h"
#include "delay.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//LAN8720驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//0-6:页面数,需要使用除Fixed寄存器以外的其他寄存器时使用
void DP83640_selectPage(u8 page)
{
	switch(page)
	{
		case 0:
			DP83640_WritePHY(DP83640_PAGESEL,0);
			break;
		case 1:
			DP83640_WritePHY(DP83640_PAGESEL,1);
			break;
		case 2:
			DP83640_WritePHY(DP83640_PAGESEL,2);
			break;
		case 3:
			DP83640_WritePHY(DP83640_PAGESEL,3);
			break;
		case 4:
			DP83640_WritePHY(DP83640_PAGESEL,4);
			break;
		case 5:
			DP83640_WritePHY(DP83640_PAGESEL,5);
			break;
		case 6:
			DP83640_WritePHY(DP83640_PAGESEL,6);
			break;
		default:
			break;
	}
}

ETH_HandleTypeDef ETH_Handler;      //以太网句柄

ETH_DMADescTypeDef *DMARxDscrTab;	//以太网DMA接收描述符数据结构体指针
ETH_DMADescTypeDef *DMATxDscrTab;	//以太网DMA发送描述符数据结构体指针 
uint8_t *Rx_Buff; 					//以太网底层驱动接收buffers指针 
uint8_t *Tx_Buff; 					//以太网底层驱动发送buffers指针
  
//LAN8720初始化
//返回值:0,成功;
//    其他,失败
u8 LAN8720_Init(void)
{      
    u8 macaddress[6];
    u32 regval=0;
    INTX_DISABLE();                         //关闭所有中断，复位过程不能被打断！
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_RESET);       //硬件复位
    delay_ms(100);
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_SET);       //复位结束
    delay_ms(100);
    INTX_ENABLE();                          //开启所有中断  

    macaddress[0]=lwipdev.mac[0]; 
		macaddress[1]=lwipdev.mac[1]; 
		macaddress[2]=lwipdev.mac[2];
		macaddress[3]=lwipdev.mac[3];   
		macaddress[4]=lwipdev.mac[4];
		macaddress[5]=lwipdev.mac[5];
        
	  ETH_Handler.Instance=ETH;
    ETH_Handler.Init.AutoNegotiation=ETH_AUTONEGOTIATION_ENABLE;//使能自协商模式 
    ETH_Handler.Init.Speed=ETH_SPEED_100M;//速度100M,如果开启了自协商模式，此配置就无效
    ETH_Handler.Init.DuplexMode=ETH_MODE_FULLDUPLEX;//全双工模式，如果开启了自协商模式，此配置就无效
    ETH_Handler.Init.PhyAddress=LAN8720_PHY_ADDRESS;//LAN8720地址  
    ETH_Handler.Init.MACAddr=macaddress;            //MAC地址  
    ETH_Handler.Init.RxMode=ETH_RXINTERRUPT_MODE;   //中断接收模式 
    ETH_Handler.Init.ChecksumMode=ETH_CHECKSUM_BY_HARDWARE;//硬件帧校验  
    ETH_Handler.Init.MediaInterface=ETH_MEDIA_INTERFACE_RMII;//RMII接口  
    if(HAL_ETH_Init(&ETH_Handler)==HAL_OK)
    {
				DP83640_PTP_init(); //初始化PTP相关寄存器
        return 0;   //成功
    }
    else return 1;  //失败
}

//ETH底层驱动，时钟使能，引脚配置
//此函数会被HAL_ETH_Init()调用
//heth:以太网句柄
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_ETH_CLK_ENABLE();             //开启ETH时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();			//开启GPIOG时钟
	__HAL_RCC_GPIOI_CLK_ENABLE();			//开启GPIOG时钟
    
    /*网络引脚设置 RMII接口 
    ETH_MDIO -------------------------> PA2
    ETH_MDC --------------------------> PC1
    ETH_RMII_REF_CLK------------------> PA1
    ETH_RMII_CRS_DV ------------------> PA7
    ETH_RMII_RXD0 --------------------> PC4
    ETH_RMII_RXD1 --------------------> PC5
    ETH_RMII_TX_EN -------------------> PB11
    ETH_RMII_TXD0 --------------------> PG13
    ETH_RMII_TXD1 --------------------> PG14
    ETH_RESET-------------------------> PCF8574扩展IO*/
    
    //PA1,2,7
    GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7; 
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
    GPIO_Initure.Pull=GPIO_NOPULL;              //不带上下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    GPIO_Initure.Alternate=GPIO_AF11_ETH;       //复用为ETH功能
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化
    
    //PB11
    GPIO_Initure.Pin=GPIO_PIN_12;               //PB11
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);         //始化
    
    //PC1,4,5
    GPIO_Initure.Pin=GPIO_PIN_1;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //初始化
	
    //PG13,14
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;   //PG13,14
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);         //初始化
		
		GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5; //PC1,4,5
		GPIO_Initure.Pull=GPIO_PULLDOWN;              //不带上下拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //初始化
		
		GPIO_Initure.Pin=GPIO_PIN_10; //PC1,4,5
		GPIO_Initure.Pull=GPIO_PULLUP;              //不带上下拉
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);         //初始化
    
    HAL_NVIC_SetPriority(ETH_IRQn,1,0);         //网络中断优先级应该高一点
    HAL_NVIC_EnableIRQ(ETH_IRQn);
}

//读取PHY寄存器值
void DP83640_ReadPHY(u16 reg, u32* regval)
{
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg, regval);
}
//向LAN8720指定寄存器写入值
//reg:要写入的寄存器
//value:要写入的值
void DP83640_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    HAL_ETH_WritePHYRegister(&ETH_Handler,reg,temp);
}

void DP83640_PTP_init(void)
{
	//使能中断，使能中断输出，使能PTP中断
	static u32 readval=0;
	static u32 regval=0;
  DP83640_ReadPHY(DP83640_MICR,&readval);
  readval|=DP83640_PTP_INT_SEL;
	readval|=DP83640_INTEN;
	readval|=DP83640_INT_OE;
  DP83640_WritePHY(DP83640_MICR,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_MICR,&readval);
	
	//打开PTP时钟
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_CTL,&readval);
	DP83640_WritePHY(DP83640_PTP_CTL,DP83640_PTP_CTL_DISABLE);
	DP83640_WritePHY(DP83640_PTP_CTL,DP83640_PTP_CTL_ENABLE);
	delay_us(1);

	DP83640_ReadPHY(DP83640_PTP_CTL,&readval);
	DP83640_WritePHY(DP83640_PTP_CTL,DP83640_PTP_CTL_RD_CLK);
	DP83640_ReadPHY(0x15,&readval);
	
	//使能发送报文，接收报文时间戳准备好中断
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_STS,&readval);
	readval|=DP83640_PTP_STS_TXTS_IE;
	readval|=DP83640_PTP_STS_RXTS_IE;
	readval|=DP83640_PTP_STS_EVENT_IE;
  DP83640_WritePHY(DP83640_PTP_STS,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_PTP_STS,&readval);
	
	//使能发送时对UDP/IPv4 PTP报文的检测
	readval = 0;
	DP83640_selectPage(5);
	DP83640_ReadPHY(DP83640_PTP_TXCFG0,&readval);
	//readval|=DP83640_PTP_TXCFG0_SYNC_1STEP;
	readval|=DP83640_PTP_TXCFG0_TX_IPV4_EN;
	readval|=DP83640_PTP_TXCFG0_TX_TS_EN;
	readval|=DP83640_PTP_TXCFG0_TX_PTP_VER;
  DP83640_WritePHY(DP83640_PTP_TXCFG0,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_PTP_TXCFG0,&readval);
	
	//使能接收时对UDP/IPv4 PTP报文的检测，使能自动打时间戳
	readval = 0;
	DP83640_selectPage(5);
	DP83640_ReadPHY(DP83640_PTP_RXCFG0,&readval);
	readval|=DP83640_PTP_RXCFG0_RX_IPV4_EN;
	readval|=DP83640_PTP_RXCFG0_RX_TS_EN;
	readval|=DP83640_PTP_RXCFG0_RX_PTP_VER;
  DP83640_WritePHY(DP83640_PTP_RXCFG0,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_PTP_RXCFG0,&readval);
	
	//使能PTP divide-by-N(default N equals to 10) 时钟的输出
	readval = 0;
	DP83640_selectPage(6);
	DP83640_ReadPHY(DP83640_PTP_COC,&readval);
	readval|=DP83640_PTP_COC_CLOOUT_EN;
	readval&=0xFFE0;
	readval|=0x14;
  DP83640_WritePHY(DP83640_PTP_COC,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_PTP_COC,&readval);
	
	//检测PTP状态寄存器，检测是否初始化成功
	
}



extern void lwip_pkt_handle(void);		//在lwip_comm.c里面定义

//中断服务函数
void ETH_IRQHandler(void)
{
    while(ETH_GetRxPktSize(ETH_Handler.RxDesc))   
    {
        lwip_pkt_handle();//处理以太网数据，即将数据提交给LWIP
    }
    //清除中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS);    //清除DMA中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R);      //清除DMA接收中断标志位
}

//获取接收到的帧长度
//DMARxDesc:接收DMA描述符
//返回值:接收到的帧长度
u32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
    u32 frameLength = 0;
    if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_ES)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_LS)!=(uint32_t)RESET)) 
    {
        frameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }
    return frameLength;
}

//为ETH底层驱动申请内存
//返回值:0,正常
//    其他,失败
u8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab=mymalloc(SRAMDTCM,ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));//申请内存
	DMATxDscrTab=mymalloc(SRAMDTCM,ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));//申请内存  
	Rx_Buff=mymalloc(SRAMDTCM,ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//申请内存
	Tx_Buff=mymalloc(SRAMDTCM,ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//申请内存
	if(!(u32)&DMARxDscrTab||!(u32)&DMATxDscrTab||!(u32)&Rx_Buff||!(u32)&Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//申请失败
	}	
	return 0;		//申请成功
}

//释放ETH 底层驱动申请的内存
void ETH_Mem_Free(void)
{ 
	myfree(SRAMDTCM,DMARxDscrTab);//释放内存
	myfree(SRAMDTCM,DMATxDscrTab);//释放内存
	myfree(SRAMDTCM,Rx_Buff);		//释放内存
	myfree(SRAMDTCM,Tx_Buff);		//释放内存  
}

void DP83640_EnablePowerDownMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval|=DP83640_BCR_POWER_DOWN;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//关闭DP83640 Power Down模式
void DP83640_DisablePowerDownMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval&=~DP83640_BCR_POWER_DOWN;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//开启DP83640的自协商功能
void DP83640_StartAutoNego(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval|=DP83640_BCR_AUTONEGO_EN;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//使能回测模式
void DP83640_EnableLoopbackMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval|=DP83640_BCR_LOOPBACK;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//关闭DP83640的回测模式
void DP83640_DisableLoopbackMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval&=~DP83640_BCR_LOOPBACK;
    DP83640_WritePHY(DP83640_BCR,readval);
}


//获取DP83640的连接状态
//返回值：DP83640_STATUS_LINK_DOWN              连接断开
//        DP83640_STATUS_AUTONEGO_NOTDONE       自动协商完成
//        DP83640_STATUS_100MBITS_FULLDUPLEX    100M全双工
//        DP83640_STATUS_100MBITS_HALFDUPLEX    100M半双工
//        DP83640_STATUS_10MBITS_FULLDUPLEX     10M全双工
//        DP83640_STATUS_10MBITS_HALFDUPLEX     10M半双工
u32 DP83640_GetLinkState(void)
{
    u32 readval=0;
    
    //读取两遍，确保读取正确！！！
    DP83640_ReadPHY(DP83640_BSR,&readval);
    DP83640_ReadPHY(DP83640_BSR,&readval);
    
    //获取连接状态(硬件，网线的连接，不是TCP、UDP等软件连接！)
    if((readval&DP83640_BSR_LINK_STATUS)==0)
        return DP83640_STATUS_LINK_DOWN;
    
    //获取自动协商状态
    DP83640_ReadPHY(DP83640_BCR,&readval);
    if((readval&DP83640_BCR_AUTONEGO_EN)!=DP83640_BCR_AUTONEGO_EN)  //未使能自动协商
    {
        if(((readval&DP83640_BCR_SPEED_SELECT)==DP83640_BCR_SPEED_SELECT)&&
                ((readval&DP83640_BCR_DUPLEX_MODE)==DP83640_BCR_DUPLEX_MODE)) 
            return DP83640_STATUS_100MBITS_FULLDUPLEX;
        else if((readval&DP83640_BCR_SPEED_SELECT)==DP83640_BCR_SPEED_SELECT)
            return DP83640_STATUS_100MBITS_HALFDUPLEX;
        else if((readval&DP83640_BCR_DUPLEX_MODE)==DP83640_BCR_DUPLEX_MODE)
            return DP83640_STATUS_10MBITS_FULLDUPLEX;
        else
            return DP83640_STATUS_10MBITS_HALFDUPLEX;
    }
    else                                                            //使能了自动协商    
    {
       DP83640_ReadPHY(DP83640_PHYSCSR,&readval);
        if((readval&DP83640_PHYSCSR_AUTONEGO_DONE)==0)
            return DP83640_STATUS_AUTONEGO_NOTDONE;
        if((readval&DP83640_DUPLEX_STATUS)&&(readval&DP83640_SPEED_STATUS))
            return DP83640_STATUS_10MBITS_FULLDUPLEX;
        else if (readval&DP83640_DUPLEX_STATUS)
            return DP83640_STATUS_100MBITS_FULLDUPLEX;
        else if (readval&DP83640_SPEED_STATUS)
            return DP83640_STATUS_10MBITS_HALFDUPLEX;
        else
            return DP83640_STATUS_100MBITS_HALFDUPLEX;         
    }
}


//设置DP83640的连接状态
//参数linkstate：DP83640_STATUS_100MBITS_FULLDUPLEX 100M全双工
//               DP83640_STATUS_100MBITS_HALFDUPLEX 100M半双工
//               DP83640_STATUS_10MBITS_FULLDUPLEX  10M全双工
//               DP83640_STATUS_10MBITS_HALFDUPLEX  10M半双工
void DP83640_SetLinkState(u32 linkstate)
{
    u32 bcrvalue=0;
    DP83640_ReadPHY(DP83640_BCR,&bcrvalue);
    //关闭连接配置，比如自动协商，速度和双工
    bcrvalue&=~(DP83640_BCR_AUTONEGO_EN|DP83640_BCR_SPEED_SELECT|DP83640_BCR_DUPLEX_MODE);
    if(linkstate==DP83640_STATUS_100MBITS_FULLDUPLEX)       //100M全双工
        bcrvalue|=(DP83640_BCR_SPEED_SELECT|DP83640_BCR_DUPLEX_MODE);
    else if(linkstate==DP83640_STATUS_100MBITS_HALFDUPLEX)  //100M半双工
        bcrvalue|=DP83640_BCR_SPEED_SELECT; 
    else if(linkstate==DP83640_STATUS_10MBITS_FULLDUPLEX)   //10M全双工
        bcrvalue|=DP83640_BCR_DUPLEX_MODE;
    
    DP83640_WritePHY(DP83640_BCR,bcrvalue);
}

