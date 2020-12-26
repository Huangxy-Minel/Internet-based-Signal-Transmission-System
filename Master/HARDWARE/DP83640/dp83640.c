#include "dp83640.h"
#include "delay.h"
#include "usart.h"

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

//PTP��ع���ʹ��
void DP83640_PTP_init()
{
	//ʹ���жϣ�ʹ���ж������ʹ��PTP�ж�
	u32 readval=0;
  DP83640_ReadPHY(DP83640_MICR,&readval);
  readval|=DP83640_PTP_INT_SEL;
	readval|=DP83640_INTEN;
	readval|=DP83640_INT_OE;
  DP83640_WritePHY(DP83640_MICR,readval);
	
	//��PTPʱ��
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_CTL,&readval);
	readval|=DP83640_PTP_CTL_ENABLE;
  DP83640_WritePHY(DP83640_PTP_CTL,readval);
	
	//ʹ�ܷ��ͱ��ģ����ձ���ʱ���׼�����ж�
	readval = 0;
	DP83640_selectPage(4);
	DP83640_ReadPHY(DP83640_PTP_STS,&readval);
	readval|=DP83640_PTP_STS_TXTS_IE;
	readval|=DP83640_PTP_STS_RXTS_IE;
  DP83640_WritePHY(DP83640_PTP_STS,readval);
	
	//ʹ�ܷ���ʱ��UDP/IPv4 PTP���ĵļ��
	readval = 0;
	DP83640_selectPage(5);
	DP83640_ReadPHY(DP83640_PTP_TXCFG0,&readval);
	readval|=DP83640_PTP_TXCFG0_TX_IPV4_EN;
	readval|=DP83640_PTP_TXCFG0_TX_TS_EN;
  DP83640_WritePHY(DP83640_PTP_TXCFG0,readval);
	
	//ʹ�ܽ���ʱ��UDP/IPv4 PTP���ĵļ��
	readval = 0;
	DP83640_selectPage(5);
	DP83640_ReadPHY(DP83640_PTP_RXCFG0,&readval);
	readval|=DP83640_PTP_RXCFG0_RX_IPV4_EN;
	readval|=DP83640_PTP_RXCFG0_RX_TS_EN;
  DP83640_WritePHY(DP83640_PTP_RXCFG0,readval);
	
	//ʹ��PTP divide-by-N(default N equals to 10) ʱ�ӵ����
	readval = 0;
	DP83640_selectPage(6);
	DP83640_ReadPHY(DP83640_PTP_COC,&readval);
	readval|=DP83640_PTP_COC_CLOOUT_EN;
  DP83640_WritePHY(DP83640_PTP_COC,readval);
}

//��ʼ��DP83640
int32_t DP83640_Init(void)
{         
    u8 macaddress[6];
    u32 timeout=0;
    u32 regval=0;
    u32 phylink=0;
    int32_t status=DP83640_STATUS_OK;
	
  	//����Ӳ����������
		GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_GPIOH_CLK_ENABLE();			//����GPIOHʱ��
		GPIO_Initure.Pin=GPIO_PIN_4;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;         			  //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH; //����
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);     	   	//��ʼ��GPIOB.0��GPIOB.1
    HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_RESET);//����
	
    //Ӳ����λ
    INTX_DISABLE();                         //�ر������жϣ���λ���̲��ܱ���ϣ�
    //PCF8574_WriteBit(ETH_RESET_IO,1);       //Ӳ����λ
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_SET);//Ӳ����λ
    delay_ms(100);
    //PCF8574_WriteBit(ETH_RESET_IO,0);       //��λ����
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_RESET);//��λ����
    delay_ms(100);
    
		INTX_ENABLE();                          //���������ж� 
    
    NETMPU_Config();                        //MPU��������
    macaddress[0]=lwipdev.mac[0]; 
		macaddress[1]=lwipdev.mac[1]; 
		macaddress[2]=lwipdev.mac[2];
		macaddress[3]=lwipdev.mac[3];   
		macaddress[4]=lwipdev.mac[4];
		macaddress[5]=lwipdev.mac[5];
    
    DP83640_ETHHandle.Instance=ETH;                             //ETH
    DP83640_ETHHandle.Init.MACAddr=macaddress;                  //mac��ַ
    DP83640_ETHHandle.Init.MediaInterface=HAL_ETH_MII_MODE;     //MII�ӿ�
    DP83640_ETHHandle.Init.RxDesc=DMARxDscrTab;                 //����������
    DP83640_ETHHandle.Init.TxDesc=DMATxDscrTab;                 //����������
    DP83640_ETHHandle.Init.RxBuffLen=ETH_MAX_PACKET_SIZE;       //���ճ���
    HAL_ETH_Init(&DP83640_ETHHandle);                           //��ʼ��ETH
    HAL_ETH_SetMDIOClockRange(&DP83640_ETHHandle);
    
		
		
		
		
		
		
    if(DP83640_WritePHY(DP83640_BCR,DP83640_BCR_SOFT_RESET)>=0) //DP83640�����λ
    {
        //�ȴ������λ���
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
                if(timeout>=DP83640_TIMEOUT) break; //��ʱ����,5S
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
		
		DP83640_PTP_init();//PTP��ع���ʹ��
		
    //DP83640_StartAutoNego();                //�����Զ�Э�̹���
    
    if(status==DP83640_STATUS_OK)           //���ǰ��������������ʱ1s
        delay_ms(1000);                     //�ȴ�1s
       
    //�ȴ��������ӳɹ�
    timeout=0;
    while(DP83640_GetLinkState()<=DP83640_STATUS_LINK_DOWN)  
    {
        delay_ms(10);
        timeout++;
        if(timeout>=DP83640_TIMEOUT) 
        {
            status=DP83640_STATUS_LINK_DOWN;
            break; //��ʱ����,5S
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


//��ȡPHY�Ĵ���ֵ
//regҪ��ȡ�ļĴ�����ַ
//����ֵ:0 ��ȡ�ɹ���-1 ��ȡʧ��
int32_t DP83640_ReadPHY(u16 reg,u32 *regval)
{
    if(HAL_ETH_ReadPHYRegister(&DP83640_ETHHandle,DP83640_ADDR,reg,regval)!=HAL_OK)
        return -1;
    return 0;
}

//��DP83640ָ���Ĵ���д��ֵ
//reg:Ҫд��ļĴ���
//value:Ҫд���ֵ
//����ֵ:0 д��������-1 д��ʧ��
int32_t DP83640_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    if(HAL_ETH_WritePHYRegister(&DP83640_ETHHandle,DP83640_ADDR,reg,temp)!=HAL_OK)
        return -1;
    return 0;
}

//��DP83640 Power Downģʽ 
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
