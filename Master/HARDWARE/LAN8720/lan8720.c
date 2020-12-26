#include "lan8720.h"
#include "pcf8574.h"
#include "lwip_comm.h"
#include "delay.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LAN8720��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//0-6:ҳ����,��Ҫʹ�ó�Fixed�Ĵ�������������Ĵ���ʱʹ��
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

ETH_HandleTypeDef ETH_Handler;      //��̫�����

ETH_DMADescTypeDef *DMARxDscrTab;	//��̫��DMA�������������ݽṹ��ָ��
ETH_DMADescTypeDef *DMATxDscrTab;	//��̫��DMA�������������ݽṹ��ָ�� 
uint8_t *Rx_Buff; 					//��̫���ײ���������buffersָ�� 
uint8_t *Tx_Buff; 					//��̫���ײ���������buffersָ��
  
//LAN8720��ʼ��
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 LAN8720_Init(void)
{      
    u8 macaddress[6];
    u32 regval=0;
    INTX_DISABLE();                         //�ر������жϣ���λ���̲��ܱ���ϣ�
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_RESET);       //Ӳ����λ
    delay_ms(100);
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,GPIO_PIN_SET);       //��λ����
    delay_ms(100);
    INTX_ENABLE();                          //���������ж�  

    macaddress[0]=lwipdev.mac[0]; 
		macaddress[1]=lwipdev.mac[1]; 
		macaddress[2]=lwipdev.mac[2];
		macaddress[3]=lwipdev.mac[3];   
		macaddress[4]=lwipdev.mac[4];
		macaddress[5]=lwipdev.mac[5];
        
	  ETH_Handler.Instance=ETH;
    ETH_Handler.Init.AutoNegotiation=ETH_AUTONEGOTIATION_ENABLE;//ʹ����Э��ģʽ 
    ETH_Handler.Init.Speed=ETH_SPEED_100M;//�ٶ�100M,�����������Э��ģʽ�������þ���Ч
    ETH_Handler.Init.DuplexMode=ETH_MODE_FULLDUPLEX;//ȫ˫��ģʽ�������������Э��ģʽ�������þ���Ч
    ETH_Handler.Init.PhyAddress=LAN8720_PHY_ADDRESS;//LAN8720��ַ  
    ETH_Handler.Init.MACAddr=macaddress;            //MAC��ַ  
    ETH_Handler.Init.RxMode=ETH_RXINTERRUPT_MODE;   //�жϽ���ģʽ 
    ETH_Handler.Init.ChecksumMode=ETH_CHECKSUM_BY_HARDWARE;//Ӳ��֡У��  
    ETH_Handler.Init.MediaInterface=ETH_MEDIA_INTERFACE_RMII;//RMII�ӿ�  
    if(HAL_ETH_Init(&ETH_Handler)==HAL_OK)
    {
				DP83640_PTP_init(); //��ʼ��PTP��ؼĴ���
        return 0;   //�ɹ�
    }
    else return 1;  //ʧ��
}

//ETH�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_ETH_Init()����
//heth:��̫�����
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_ETH_CLK_ENABLE();             //����ETHʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();			//����GPIOGʱ��
	__HAL_RCC_GPIOI_CLK_ENABLE();			//����GPIOGʱ��
    
    /*������������ RMII�ӿ� 
    ETH_MDIO -------------------------> PA2
    ETH_MDC --------------------------> PC1
    ETH_RMII_REF_CLK------------------> PA1
    ETH_RMII_CRS_DV ------------------> PA7
    ETH_RMII_RXD0 --------------------> PC4
    ETH_RMII_RXD1 --------------------> PC5
    ETH_RMII_TX_EN -------------------> PB11
    ETH_RMII_TXD0 --------------------> PG13
    ETH_RMII_TXD1 --------------------> PG14
    ETH_RESET-------------------------> PCF8574��չIO*/
    
    //PA1,2,7
    GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7; 
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_NOPULL;              //����������
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
    GPIO_Initure.Alternate=GPIO_AF11_ETH;       //����ΪETH����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
    
    //PB11
    GPIO_Initure.Pin=GPIO_PIN_12;               //PB11
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);         //ʼ��
    
    //PC1,4,5
    GPIO_Initure.Pin=GPIO_PIN_1;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //��ʼ��
	
    //PG13,14
    GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;   //PG13,14
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);         //��ʼ��
		
		GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5; //PC1,4,5
		GPIO_Initure.Pull=GPIO_PULLDOWN;              //����������
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //��ʼ��
		
		GPIO_Initure.Pin=GPIO_PIN_10; //PC1,4,5
		GPIO_Initure.Pull=GPIO_PULLUP;              //����������
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);         //��ʼ��
    
    HAL_NVIC_SetPriority(ETH_IRQn,1,0);         //�����ж����ȼ�Ӧ�ø�һ��
    HAL_NVIC_EnableIRQ(ETH_IRQn);
}

//��ȡPHY�Ĵ���ֵ
void DP83640_ReadPHY(u16 reg, u32* regval)
{
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg, regval);
}
//��LAN8720ָ���Ĵ���д��ֵ
//reg:Ҫд��ļĴ���
//value:Ҫд���ֵ
void DP83640_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    HAL_ETH_WritePHYRegister(&ETH_Handler,reg,temp);
}

void DP83640_PTP_init(void)
{
	//ʹ���жϣ�ʹ���ж������ʹ��PTP�ж�
	static u32 readval=0;
	static u32 regval=0;
  DP83640_ReadPHY(DP83640_MICR,&readval);
  readval|=DP83640_PTP_INT_SEL;
	readval|=DP83640_INTEN;
	readval|=DP83640_INT_OE;
  DP83640_WritePHY(DP83640_MICR,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_MICR,&readval);
	
	//��PTPʱ��
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_CTL,&readval);
	DP83640_WritePHY(DP83640_PTP_CTL,DP83640_PTP_CTL_DISABLE);
	DP83640_WritePHY(DP83640_PTP_CTL,DP83640_PTP_CTL_ENABLE);
	delay_us(1);

	DP83640_ReadPHY(DP83640_PTP_CTL,&readval);
	DP83640_WritePHY(DP83640_PTP_CTL,DP83640_PTP_CTL_RD_CLK);
	DP83640_ReadPHY(0x15,&readval);
	
	//ʹ�ܷ��ͱ��ģ����ձ���ʱ���׼�����ж�
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_STS,&readval);
	readval|=DP83640_PTP_STS_TXTS_IE;
	readval|=DP83640_PTP_STS_RXTS_IE;
	readval|=DP83640_PTP_STS_EVENT_IE;
  DP83640_WritePHY(DP83640_PTP_STS,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_PTP_STS,&readval);
	
	//ʹ�ܷ���ʱ��UDP/IPv4 PTP���ĵļ��
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
	
	//ʹ�ܽ���ʱ��UDP/IPv4 PTP���ĵļ�⣬ʹ���Զ���ʱ���
	readval = 0;
	DP83640_selectPage(5);
	DP83640_ReadPHY(DP83640_PTP_RXCFG0,&readval);
	readval|=DP83640_PTP_RXCFG0_RX_IPV4_EN;
	readval|=DP83640_PTP_RXCFG0_RX_TS_EN;
	readval|=DP83640_PTP_RXCFG0_RX_PTP_VER;
  DP83640_WritePHY(DP83640_PTP_RXCFG0,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_PTP_RXCFG0,&readval);
	
	//ʹ��PTP divide-by-N(default N equals to 10) ʱ�ӵ����
	readval = 0;
	DP83640_selectPage(6);
	DP83640_ReadPHY(DP83640_PTP_COC,&readval);
	readval|=DP83640_PTP_COC_CLOOUT_EN;
	readval&=0xFFE0;
	readval|=0x14;
  DP83640_WritePHY(DP83640_PTP_COC,readval);
	delay_us(1);
	DP83640_ReadPHY(DP83640_PTP_COC,&readval);
	
	//���PTP״̬�Ĵ���������Ƿ��ʼ���ɹ�
	
}



extern void lwip_pkt_handle(void);		//��lwip_comm.c���涨��

//�жϷ�����
void ETH_IRQHandler(void)
{
    while(ETH_GetRxPktSize(ETH_Handler.RxDesc))   
    {
        lwip_pkt_handle();//������̫�����ݣ����������ύ��LWIP
    }
    //����жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS);    //���DMA�жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R);      //���DMA�����жϱ�־λ
}

//��ȡ���յ���֡����
//DMARxDesc:����DMA������
//����ֵ:���յ���֡����
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

//ΪETH�ײ����������ڴ�
//����ֵ:0,����
//    ����,ʧ��
u8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab=mymalloc(SRAMDTCM,ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));//�����ڴ�
	DMATxDscrTab=mymalloc(SRAMDTCM,ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));//�����ڴ�  
	Rx_Buff=mymalloc(SRAMDTCM,ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//�����ڴ�
	Tx_Buff=mymalloc(SRAMDTCM,ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//�����ڴ�
	if(!(u32)&DMARxDscrTab||!(u32)&DMATxDscrTab||!(u32)&Rx_Buff||!(u32)&Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//����ʧ��
	}	
	return 0;		//����ɹ�
}

//�ͷ�ETH �ײ�����������ڴ�
void ETH_Mem_Free(void)
{ 
	myfree(SRAMDTCM,DMARxDscrTab);//�ͷ��ڴ�
	myfree(SRAMDTCM,DMATxDscrTab);//�ͷ��ڴ�
	myfree(SRAMDTCM,Rx_Buff);		//�ͷ��ڴ�
	myfree(SRAMDTCM,Tx_Buff);		//�ͷ��ڴ�  
}

void DP83640_EnablePowerDownMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval|=DP83640_BCR_POWER_DOWN;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//�ر�DP83640 Power Downģʽ
void DP83640_DisablePowerDownMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval&=~DP83640_BCR_POWER_DOWN;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//����DP83640����Э�̹���
void DP83640_StartAutoNego(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval|=DP83640_BCR_AUTONEGO_EN;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//ʹ�ܻز�ģʽ
void DP83640_EnableLoopbackMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval|=DP83640_BCR_LOOPBACK;
    DP83640_WritePHY(DP83640_BCR,readval);
}

//�ر�DP83640�Ļز�ģʽ
void DP83640_DisableLoopbackMode(void)
{
    u32 readval=0;
    DP83640_ReadPHY(DP83640_BCR,&readval);
    readval&=~DP83640_BCR_LOOPBACK;
    DP83640_WritePHY(DP83640_BCR,readval);
}


//��ȡDP83640������״̬
//����ֵ��DP83640_STATUS_LINK_DOWN              ���ӶϿ�
//        DP83640_STATUS_AUTONEGO_NOTDONE       �Զ�Э�����
//        DP83640_STATUS_100MBITS_FULLDUPLEX    100Mȫ˫��
//        DP83640_STATUS_100MBITS_HALFDUPLEX    100M��˫��
//        DP83640_STATUS_10MBITS_FULLDUPLEX     10Mȫ˫��
//        DP83640_STATUS_10MBITS_HALFDUPLEX     10M��˫��
u32 DP83640_GetLinkState(void)
{
    u32 readval=0;
    
    //��ȡ���飬ȷ����ȡ��ȷ������
    DP83640_ReadPHY(DP83640_BSR,&readval);
    DP83640_ReadPHY(DP83640_BSR,&readval);
    
    //��ȡ����״̬(Ӳ�������ߵ����ӣ�����TCP��UDP��������ӣ�)
    if((readval&DP83640_BSR_LINK_STATUS)==0)
        return DP83640_STATUS_LINK_DOWN;
    
    //��ȡ�Զ�Э��״̬
    DP83640_ReadPHY(DP83640_BCR,&readval);
    if((readval&DP83640_BCR_AUTONEGO_EN)!=DP83640_BCR_AUTONEGO_EN)  //δʹ���Զ�Э��
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
    else                                                            //ʹ�����Զ�Э��    
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


//����DP83640������״̬
//����linkstate��DP83640_STATUS_100MBITS_FULLDUPLEX 100Mȫ˫��
//               DP83640_STATUS_100MBITS_HALFDUPLEX 100M��˫��
//               DP83640_STATUS_10MBITS_FULLDUPLEX  10Mȫ˫��
//               DP83640_STATUS_10MBITS_HALFDUPLEX  10M��˫��
void DP83640_SetLinkState(u32 linkstate)
{
    u32 bcrvalue=0;
    DP83640_ReadPHY(DP83640_BCR,&bcrvalue);
    //�ر��������ã������Զ�Э�̣��ٶȺ�˫��
    bcrvalue&=~(DP83640_BCR_AUTONEGO_EN|DP83640_BCR_SPEED_SELECT|DP83640_BCR_DUPLEX_MODE);
    if(linkstate==DP83640_STATUS_100MBITS_FULLDUPLEX)       //100Mȫ˫��
        bcrvalue|=(DP83640_BCR_SPEED_SELECT|DP83640_BCR_DUPLEX_MODE);
    else if(linkstate==DP83640_STATUS_100MBITS_HALFDUPLEX)  //100M��˫��
        bcrvalue|=DP83640_BCR_SPEED_SELECT; 
    else if(linkstate==DP83640_STATUS_10MBITS_FULLDUPLEX)   //10Mȫ˫��
        bcrvalue|=DP83640_BCR_DUPLEX_MODE;
    
    DP83640_WritePHY(DP83640_BCR,bcrvalue);
}

