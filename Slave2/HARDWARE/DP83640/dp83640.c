#include "dp83640.h"
#include "delay.h"
#include "usart.h"

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

//PTP相关功能使能
void DP83640_PTP_init()
{
	//使能中断，使能中断输出，使能PTP中断
	u32 readval=0;
  DP83640_ReadPHY(DP83640_MICR,&readval);
  readval|=DP83640_PTP_INT_SEL;
	readval|=DP83640_INTEN;
	readval|=DP83640_INT_OE;
  DP83640_WritePHY(DP83640_MICR,readval);
	
	//打开PTP时钟
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_CTL,&readval);
	readval|=DP83640_PTP_CTL_ENABLE;
  DP83640_WritePHY(DP83640_PTP_CTL,readval);
	
	//使能发送报文，接收报文时间戳准备好中断
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_STS,&readval);
	readval|=DP83640_PTP_STS_TXTS_IE;
	readval|=DP83640_PTP_STS_RXTS_IE;
  DP83640_WritePHY(DP83640_PTP_STS,readval);
	
	//使能发送时对UDP/IPv4 PTP报文的检测
	readval = 0;
	DP83640_selectPage(5);
	DP83640_ReadPHY(DP83640_PTP_TXCFG0,&readval);
	readval|=DP83640_PTP_TXCFG0_TX_IPV4_EN;
	readval|=DP83640_PTP_TXCFG0_TX_TS_EN;
  DP83640_WritePHY(DP83640_PTP_TXCFG0,readval);
	
	//使能接收时对UDP/IPv4 PTP报文的检测
	readval = 0;
	DP83640_selectPage(5);
	DP83640_ReadPHY(DP83640_PTP_RXCFG0,&readval);
	readval|=DP83640_PTP_RXCFG0_RX_IPV4_EN;
	readval|=DP83640_PTP_RXCFG0_RX_TS_EN;
  DP83640_WritePHY(DP83640_PTP_RXCFG0,readval);
	
	//使能PTP divide-by-N(default N equals to 10) 时钟的输出
	readval = 0;
	DP83640_selectPage(6);
	DP83640_ReadPHY(DP83640_PTP_COC,&readval);
	readval|=DP83640_PTP_COC_CLOOUT_EN;
  DP83640_WritePHY(DP83640_PTP_COC,readval);
}

//初始化DP83640
int32_t DP83640_Init(void)
{         
    u8 macaddress[6];
    u32 timeout=0;
    u32 regval=0;
    u32 phylink=0;
    int32_t status=DP83640_STATUS_OK;
	
  	//配置硬件重置引脚
		GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOH时钟
		GPIO_Initure.Pin=GPIO_PIN_4;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         			  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH; //高速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);     	   	//初始化GPIOB.0和GPIOB.1
    HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_RESET);//拉低
	
    //硬件复位
    INTX_DISABLE();                         //关闭所有中断，复位过程不能被打断！
    //PCF8574_WriteBit(ETH_RESET_IO,1);       //硬件复位
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_SET);//硬件复位
    delay_ms(100);
    //PCF8574_WriteBit(ETH_RESET_IO,0);       //复位结束
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_RESET);//复位结束
    delay_ms(100);
    
		INTX_ENABLE();                          //开启所有中断 
    
    NETMPU_Config();                        //MPU保护设置
    macaddress[0]=lwipdev.mac[0]; 
		macaddress[1]=lwipdev.mac[1]; 
		macaddress[2]=lwipdev.mac[2];
		macaddress[3]=lwipdev.mac[3];   
		macaddress[4]=lwipdev.mac[4];
		macaddress[5]=lwipdev.mac[5];
    
    DP83640_ETHHandle.Instance=ETH;                             //ETH
    DP83640_ETHHandle.Init.MACAddr=macaddress;                  //mac地址
    DP83640_ETHHandle.Init.MediaInterface=HAL_ETH_MII_MODE;     //MII接口
    DP83640_ETHHandle.Init.RxDesc=DMARxDscrTab;                 //发送描述符
    DP83640_ETHHandle.Init.TxDesc=DMATxDscrTab;                 //接收描述符
    DP83640_ETHHandle.Init.RxBuffLen=ETH_MAX_PACKET_SIZE;       //接收长度
    HAL_ETH_Init(&DP83640_ETHHandle);                           //初始化ETH
    HAL_ETH_SetMDIOClockRange(&DP83640_ETHHandle);
    
		
		
		
		
		
		
    if(DP83640_WritePHY(DP83640_BCR,DP83640_BCR_SOFT_RESET)>=0) //DP83640软件复位
    {
        //等待软件复位完成
        if(DP83640_ReadPHY(DP83640_BCR,&regval)>=0)
        {
            while(regval&DP83640_BCR_SOFT_RESET)
            {
                if(DP83640_ReadPHY(DP83640_BCR,&regval)<0)
                {
                    status=DP83640_STATUS_READ_ERROR;
                    break;
                }
                delay_ms(10);
                timeout++;
                if(timeout>=DP83640_TIMEOUT) break; //超时跳出,5S
            }
    
        }
        else
        {
            status=DP83640_STATUS_READ_ERROR;
        }
    }
    else
    {
        status=DP83640_STATUS_WRITE_ERROR;
    }
		
		DP83640_PTP_init();//PTP相关功能使能
		
    //DP83640_StartAutoNego();                //开启自动协商功能
    
    if(status==DP83640_STATUS_OK)           //如果前面运行正常就延时1s
        delay_ms(1000);                     //等待1s
       
    //等待网络连接成功
    timeout=0;
    while(DP83640_GetLinkState()<=DP83640_STATUS_LINK_DOWN)  
    {
        delay_ms(10);
        timeout++;
        if(timeout>=DP83640_TIMEOUT) 
        {
            status=DP83640_STATUS_LINK_DOWN;
            break; //超时跳出,5S
        }
    }
    phylink=DP83640_GetLinkState();
    if(phylink==DP83640_STATUS_100MBITS_FULLDUPLEX)
        printf("DP83640:100Mb/s FullDuplex\r\n");
    else if(phylink==DP83640_STATUS_100MBITS_HALFDUPLEX)
        printf("DP83640:100Mb/s HalfDuplex\r\n");
    else if(phylink==DP83640_STATUS_10MBITS_FULLDUPLEX)
        printf("DP83640:10Mb/s FullDuplex\r\n");
    else if(phylink==DP83640_STATUS_10MBITS_HALFDUPLEX)
        printf("DP83640:10Mb/s HalfDuplex\r\n");
    return status; 
}


//读取PHY寄存器值
//reg要读取的寄存器地址
//返回值:0 读取成功，-1 读取失败
int32_t DP83640_ReadPHY(u16 reg,u32 *regval)
{
    if(HAL_ETH_ReadPHYRegister(&DP83640_ETHHandle,DP83640_ADDR,reg,regval)!=HAL_OK)
        return -1;
    return 0;
}

//向DP83640指定寄存器写入值
//reg:要写入的寄存器
//value:要写入的值
//返回值:0 写入正常，-1 写入失败
int32_t DP83640_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    if(HAL_ETH_WritePHYRegister(&DP83640_ETHHandle,DP83640_ADDR,reg,temp)!=HAL_OK)
        return -1;
    return 0;
}

//打开DP83640 Power Down模式 
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
