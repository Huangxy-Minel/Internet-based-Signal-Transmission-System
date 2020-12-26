#ifndef __LAN8720_H
#define __LAN8720_H
#include "sys.h"
#include "stm32f7xx_hal_conf.h"
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

#define DP83640_ADDR            1           //DP83640��ַΪ1
#define DP83640_TIMEOUT     ((uint32_t)500) //DP83640��ʱʱ��


//Fixed Register
#define DP83640_BCR      ((uint16_t)0x0000U)//������Ϣ�Ĵ�����           �����ٶȣ�˫��ģʽ��LOOPBACK�������õ�
#define DP83640_BSR      ((uint16_t)0x0001U)//������Ϣ�Ĵ�����ֻ����
#define DP83640_PHYSCSR	 ((uint16_t)0x0010U)//PHY״̬�Ĵ�����            �Ƿ������ӣ�˫��״̬,�ٶ�״̬����Э�����״̬��LOOPBACK״̬��
#define DP83640_MICR     ((uint16_t)0x0011U)//���ж�ʹ�ܼĴ�����         PTP�ж�ʹ�ܡ�ʹ��MISR��ָ�����ж�Դ��ʹ���ж������
#define DP83640_MISR     ((uint16_t)0x0012U)//�ж��¼�ʹ�ܺͲ鿴�Ĵ����� �����ж��¼���ʹ�ܺ����ǵ��ж�״̬
#define DP83640_PAGESEL  ((uint16_t)0x0013U)//ҳ��ѡ��Ĵ���

//PAGE4 : PTP 1588 base Register
#define DP83640_PTP_CTL    ((uint16_t)0x0014U)//Trigger��ʱ�����üĴ�����            ��ȡ������Trigger�Ĵ���ʱ�䣻��ȡ�����õ�ǰ��1588ʱ��
#define DP83640_PTP_TDR    ((uint16_t)0x0015U)//ʱ��Ĵ�����                         ��Ŷ�ȡ������ʱ�䣬������Ҫд��ĳ�����õ�ʱ��
#define DP83640_PTP_STS    ((uint16_t)0x0016U)//ʱ���׼����־λ&PTP�ж�ʹ�ܼĴ����� ���͡�����ʱ���׼���ñ�־λ��ʹ��ʱ���׼���ж�
#define DP83640_PTP_TSTS   ((uint16_t)0x0017U)//����Trigger״̬�Ĵ�����              ����Trigger�Ƿ�ʹ�ܡ��Ƿ����ô���
#define DP83640_PTP_RATEL  ((uint16_t)0x0018U)//���ʵ�λ�Ĵ�����                     ������ʵĵ�ʮ��λ����
#define DP83640_PTP_RATEH  ((uint16_t)0x0019U)//���ʸ�λ�����üĴ�����               ���ʸ�ʮλ���ݣ��Ƿ�Ϊ��ʱ�������ʣ����ʵ�����������
#define DP83640_PTP_TXTS   ((uint16_t)0x001CU)//����ʱ�����żĴ�����               �������¶�����ʱ���ֵ
#define DP83640_PTP_RXTS   ((uint16_t)0x001DU)//����ʱ�����żĴ�����               �������¶�����ʱ���ֵ
#define DP83640_PTP_ESTS   ((uint16_t)0x001EU)//event�����ƽʱ�����Ԫ�Ĵ���
#define DP83640_PTP_EDATA  ((uint16_t)0x001FU)//event�����ƽ״̬�Ĵ���

//PAGE5 : PTP 1588 Configuration Register
#define DP83640_PTP_TRIG   ((uint16_t)0x0014U)//Trigger���üĴ�����                  ���ÿһ�������Triger�������ã�Ҫ��ʲô�źţ����ĸ��������֮ǰPTP_CTL�Ĵ������ù�����ʱ�䣩
#define DP83640_PTP_EVNT   ((uint16_t)0x0015U)//Event���üĴ�����                    ���ÿһ�������event�������ã��ĸ��ţ�Ҫ��ʲô�ź�
#define DP83640_PTP_TXCFG0 ((uint16_t)0x0016U)//����PTP�������üĴ�����              ʱ�����¼ʹ�ܵ�
#define DP83640_PTP_TXCFG1 ((uint16_t)0x0017U)//δ֪��;
#define DP83640_PTP_RXCFG0 ((uint16_t)0x0019U)//����PTP�������üĴ�����              ʱ�����¼ʹ�ܵ�
#define DP83640_PTP_RXCFG1 ((uint16_t)0x001AU)//δ֪��;
#define DP83640_PTP_RXCFG2 ((uint16_t)0x001BU)//IP�������Ĵ�����                     ���ip��ַ�����ڲ����ip��PTP���Ĳ���¼ʱ�������0x19��ʹ�ܣ�
#define DP83640_PTP_TRDL   ((uint16_t)0x001EU)//��ʱ���ʵ�������ʱ��Ĵ�������λ��
#define DP83640_PTP_TRDH   ((uint16_t)0x001FU)//��ʱ���ʵ�������ʱ��Ĵ�������λ��

//PAGE6 : PTP 1588 Configuration Register
#define DP83640_PTP_COC    ((uint16_t)0x0014U)//���ʱ�����üĴ���

//DP83640 BCR�Ĵ�����λ����(FIXED)
#define DP83640_BCR_SOFT_RESET         ((uint16_t)0x8000U)
#define DP83640_BCR_LOOPBACK           ((uint16_t)0x4000U)
#define DP83640_BCR_SPEED_SELECT       ((uint16_t)0x2000U)
#define DP83640_BCR_AUTONEGO_EN        ((uint16_t)0x1000U)
#define DP83640_BCR_POWER_DOWN         ((uint16_t)0x0800U)
#define DP83640_BCR_ISOLATE            ((uint16_t)0x0400U)
#define DP83640_BCR_RESTART_AUTONEGO   ((uint16_t)0x0200U)
#define DP83640_BCR_DUPLEX_MODE        ((uint16_t)0x0100U) 

//DP83640��BSR�Ĵ�����λ����(FIXED)
#define DP83640_BSR_100BASE_T4       ((uint16_t)0x8000U)
#define DP83640_BSR_100BASE_TX_FD    ((uint16_t)0x4000U)
#define DP83640_BSR_100BASE_TX_HD    ((uint16_t)0x2000U)
#define DP83640_BSR_10BASE_T_FD      ((uint16_t)0x1000U)
#define DP83640_BSR_10BASE_T_HD      ((uint16_t)0x0800U)
#define DP83640_BSR_100BASE_T2_FD    ((uint16_t)0x0400U)
#define DP83640_BSR_100BASE_T2_HD    ((uint16_t)0x0200U)
#define DP83640_BSR_EXTENDED_STATUS  ((uint16_t)0x0100U)
#define DP83640_BSR_AUTONEGO_CPLT    ((uint16_t)0x0020U)
#define DP83640_BSR_REMOTE_FAULT     ((uint16_t)0x0010U)
#define DP83640_BSR_AUTONEGO_ABILITY ((uint16_t)0x0008U)
#define DP83640_BSR_LINK_STATUS      ((uint16_t)0x0004U)
#define DP83640_BSR_JABBER_DETECT    ((uint16_t)0x0002U)
#define DP83640_BSR_EXTENDED_CAP     ((uint16_t)0x0001U)

//DP83640 PHYSCSR�Ĵ�����λ����(FIXED)  
#define DP83640_PHYSCSR_AUTONEGO_DONE  ((uint16_t)0x0010U)
#define DP83640_DUPLEX_STATUS          ((uint16_t)0x0004U)
#define DP83640_SPEED_STATUS           ((uint16_t)0x0002U)

//DP83640 MICR�Ĵ�����λ����(FIXED)
#define DP83640_PTP_INT_SEL          ((uint16_t)0x0008U)//ʹ��PTP�ж�(��ʼ��ʹ��)
#define DP83640_INT                  ((uint16_t)0x0004U)//�����жϣ�����һ���ж��ź�
#define DP83640_INTEN                ((uint16_t)0x0002U)//ʹ��MISR��򿪵ĸ����ж�(��ʼ��ʹ��)
#define DP83640_INT_OE               ((uint16_t)0x0001U)//ʹ���ж��źŴ�PWRDPWN����(��ʼ��ʹ��)�

//DP83640 MISR�Ĵ�����λ����(FIXED)
#define DP83640_PTP_INT          ((uint16_t)0x0800U)//PTP�жϱ�־λ

//DP83640��PTP_CTL�Ĵ�����λ����(PAGE4)
#define DP83640_PTP_CTL_RD_CLK       ((uint16_t)0x0020U)
#define DP83640_PTP_CTL_LOAD_CLK     ((uint16_t)0x0010U)
#define DP83640_PTP_CTL_STEP_CLK     ((uint16_t)0x0008U) 
#define DP83640_PTP_CTL_ENABLE       ((uint16_t)0x0004U)//��PTPʱ��(��ʼ��ʹ��)
#define DP83640_PTP_CTL_DISABLE      ((uint16_t)0x0002U)//�ر�PTPʱ��
#define DP83640_PTP_CTL_RESET        ((uint16_t)0x0001U)//����PTPʱ��(�����1������ֶ���0)

//DP83640��PTP_STS�Ĵ�����λ����(PAGE4)
#define DP83640_PTP_STS_TXTS_RDY     ((uint16_t)0x0800U)//���ͱ���ʱ���׼���ñ�־
#define DP83640_PTP_STS_RXTS_RDY     ((uint16_t)0x0400U)//���ձ���ʱ���׼���ñ�־
#define DP83640_PTP_STS_TXTS_IE      ((uint16_t)0x0008U)//ʹ�ܷ��ͱ���ʱ���׼�����ж�(��ʼ��ʹ��)
#define DP83640_PTP_STS_RXTS_IE      ((uint16_t)0x0004U)//ʹ�ܽ��ձ���ʱ���׼�����ж�(��ʼ��ʹ��)
#define DP83640_PTP_STS_EVENT_IE      ((uint16_t)0x0001U)

//DP83640��PTP_RATEH�Ĵ�����λ����(PAGE4)
#define DP83640_PTP_RATEH_RATE_DIR   ((uint16_t)0x8000U)//���ʵ�������
#define DP83640_PTP_RATEH_TEM_RATE   ((uint16_t)0x4000U)//��ʱ���ʵ���ģʽ

//DP83640��PTP_TXCFG0�Ĵ�����λ����(PAGE5)
#define DP83640_PTP_TXCFG0_SYNC_1STEP   ((uint16_t)0x8000U)//ʹ���Զ���ʱ�������
#define DP83640_PTP_TXCFG0_TX_IPV4_EN   ((uint16_t)0x0020U)//ʹ�ܶԻ���UDP/IPv4��PTP���ĵļ��(��ʼ��ʹ��)
#define DP83640_PTP_TXCFG0_TX_TS_EN     ((uint16_t)0x0001U)//ʹ�ܷ��ͱ��ĵ�ʱ���ץȡ(��ʼ��ʹ��)
#define DP83640_PTP_TXCFG0_TX_PTP_VER     ((uint16_t)0x0004U)

//DP83640��PTP_RXCFG0�Ĵ�����λ����(PAGE5)
#define DP83640_PTP_RXCFG0_RX_IPV4_EN   ((uint16_t)0x0020U)//ʹ�ܶԻ���UDP/IPv4��PTP���ĵļ��(��ʼ��ʹ��)
#define DP83640_PTP_RXCFG0_RX_TS_EN     ((uint16_t)0x0001U)//ʹ�ܽ��ձ��ĵ�ʱ���ץȡ(��ʼ��ʹ��)
#define DP83640_PTP_RXCFG0_RX_PTP_VER   ((uint16_t)0x0004U)

//DP83640��PTP_COC�Ĵ�����λ����(PAGE6)
#define DP83640_PTP_COC_CLOOUT_EN     ((uint16_t)0x8000U)//ʹ��PTP divide-by-N(default N equals to 10) ʱ�ӵ����(��ʼ��ʹ��)



//DP83640״̬��ض���
#define  DP83640_STATUS_READ_ERROR            ((int32_t)-5)
#define  DP83640_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  DP83640_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  DP83640_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  DP83640_STATUS_ERROR                 ((int32_t)-1)
#define  DP83640_STATUS_OK                    ((int32_t) 0)
#define  DP83640_STATUS_LINK_DOWN             ((int32_t) 1)
#define  DP83640_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  DP83640_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  DP83640_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  DP83640_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  DP83640_STATUS_AUTONEGO_NOTDONE      ((int32_t) 6)


extern ETH_HandleTypeDef ETH_Handler;               //��̫�����
extern ETH_DMADescTypeDef *DMARxDscrTab;			//��̫��DMA�������������ݽṹ��ָ��
extern ETH_DMADescTypeDef *DMATxDscrTab;			//��̫��DMA�������������ݽṹ��ָ�� 
extern uint8_t *Rx_Buff; 							//��̫���ײ���������buffersָ�� 
extern uint8_t *Tx_Buff; 							//��̫���ײ���������buffersָ��
extern ETH_DMADescTypeDef  *DMATxDescToSet;			//DMA����������׷��ָ��
extern ETH_DMADescTypeDef  *DMARxDescToGet; 		//DMA����������׷��ָ�� 
 

u8 LAN8720_Init(void);
void DP83640_selectPage(u8 page);
void DP83640_ReadPHY(u16 reg, u32* regval);
void DP83640_WritePHY(u16 reg,u16 value);
void DP83640_PTP_init(void);
u8 ETH_MACDMA_Config(void);
u8 ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
u32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc);
#endif
