#ifndef __LAN8720_H
#define __LAN8720_H
#include "sys.h"
#include "stm32f7xx_hal_conf.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F7¿ª·¢°å
//LAN8720Çı¶¯´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2016/1/13
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define DP83640_ADDR            1           //DP83640µØÖ·Îª1
#define DP83640_TIMEOUT     ((uint32_t)500) //DP83640³¬Ê±Ê±¼ä


//Fixed Register
#define DP83640_BCR      ((uint16_t)0x0000U)//»ù±¾ĞÅÏ¢¼Ä´æÆ÷£º           ÅäÖÃËÙ¶È£¬Ë«¹¤Ä£Ê½£¬LOOPBACK£¬ÈíÖØÖÃµÈ
#define DP83640_BSR      ((uint16_t)0x0001U)//»ù±¾ĞÅÏ¢¼Ä´æÆ÷£¨Ö»¶Á£©
#define DP83640_PHYSCSR	 ((uint16_t)0x0010U)//PHY×´Ì¬¼Ä´æÆ÷£º            ÊÇ·ñ½¨Á¢Á¬½Ó£¬Ë«¹¤×´Ì¬,ËÙ¶È×´Ì¬£¬×ÔĞ­ÉÌÍê³É×´Ì¬£¬LOOPBACK×´Ì¬µÈ
#define DP83640_MICR     ((uint16_t)0x0011U)//×ÜÖĞ¶ÏÊ¹ÄÜ¼Ä´æÆ÷£º         PTPÖĞ¶ÏÊ¹ÄÜ¡¢Ê¹ÄÜMISRÖĞÖ¸¶¨µÄÖĞ¶ÏÔ´£¬Ê¹ÄÜÖĞ¶ÏÊä³öµÈ
#define DP83640_MISR     ((uint16_t)0x0012U)//ÖĞ¶ÏÊÂ¼şÊ¹ÄÜºÍ²é¿´¼Ä´æÆ÷£º ¸÷¸öÖĞ¶ÏÊÂ¼şµÄÊ¹ÄÜºÍËüÃÇµÄÖĞ¶Ï×´Ì¬
#define DP83640_PAGESEL  ((uint16_t)0x0013U)//Ò³ÃæÑ¡Ôñ¼Ä´æÆ÷

//PAGE4 : PTP 1588 base Register
#define DP83640_PTP_CTL    ((uint16_t)0x0014U)//TriggerºÍÊ±¼äÅäÖÃ¼Ä´æÆ÷£º            ¶ÁÈ¡¡¢ÅäÖÃTriggerµÄ´¥·¢Ê±¼ä£»¶ÁÈ¡¡¢ÅäÖÃµ±Ç°µÄ1588Ê±¼ä
#define DP83640_PTP_TDR    ((uint16_t)0x0015U)//Ê±¼ä¼Ä´æÆ÷£º                         ´æ·Å¶ÁÈ¡³öÀ´µÄÊ±¼ä£¬»òÕßÊÇÒªĞ´½øÄ³¸öÅäÖÃµÄÊ±¼ä
#define DP83640_PTP_STS    ((uint16_t)0x0016U)//Ê±¼ä´Á×¼±¸±êÖ¾Î»&PTPÖĞ¶ÏÊ¹ÄÜ¼Ä´æÆ÷£º ·¢ËÍ¡¢½ÓÊÕÊ±¼ä´Á×¼±¸ºÃ±êÖ¾Î»£¬Ê¹ÄÜÊ±¼ä´Á×¼±¸ÖĞ¶Ï
#define DP83640_PTP_TSTS   ((uint16_t)0x0017U)//¸÷¸öTrigger×´Ì¬¼Ä´æÆ÷£º              ¸÷¸öTriggerÊÇ·ñ±»Ê¹ÄÜ¡¢ÊÇ·ñ±»ÅäÖÃ´íÎó
#define DP83640_PTP_RATEL  ((uint16_t)0x0018U)//ËÙÂÊµÍÎ»¼Ä´æÆ÷£º                     ´æ·ÅËÙÂÊµÄµÍÊ®ÁùÎ»Êı¾İ
#define DP83640_PTP_RATEH  ((uint16_t)0x0019U)//ËÙÂÊ¸ßÎ»ºÍÅäÖÃ¼Ä´æÆ÷£º               ËÙÂÊ¸ßÊ®Î»Êı¾İ£¬ÊÇ·ñÎªÁÙÊ±µ÷ÕûËÙÂÊ£¬ËÙÂÊµ÷Õû·½ÏòÉèÖÃ
#define DP83640_PTP_TXTS   ((uint16_t)0x001CU)//·¢ËÍÊ±¼ä´Á´æ·Å¼Ä´æÆ÷£º               Á¬¶ÁËÄÏÂ¶Á³ö¸ÃÊ±¼ä´ÁÖµ
#define DP83640_PTP_RXTS   ((uint16_t)0x001DU)//½ÓÊÕÊ±¼ä´Á´æ·Å¼Ä´æÆ÷£º               Á¬¶ÁËÄÏÂ¶Á³ö¸ÃÊ±¼ä´ÁÖµ
#define DP83640_PTP_ESTS   ((uint16_t)0x001EU)//eventÊäÈëµçÆ½Ê±¼ä´Áµ¥Ôª¼Ä´æÆ÷
#define DP83640_PTP_EDATA  ((uint16_t)0x001FU)//eventÊäÈëµçÆ½×´Ì¬¼Ä´æÆ÷

//PAGE5 : PTP 1588 Configuration Register
#define DP83640_PTP_TRIG   ((uint16_t)0x0014U)//TriggerÅäÖÃ¼Ä´æÆ÷£º                  Õë¶ÔÃ¿Ò»¸ö¾ßÌåµÄTriger½øĞĞÅäÖÃ£¬Òª³öÊ²Ã´ĞÅºÅ£¬ÔÚÄÄ¸ö½ÅÊä³ö£¨Ö®Ç°PTP_CTL¼Ä´æÆ÷ÅäÖÃ¹ı´¥·¢Ê±¼ä£©
#define DP83640_PTP_EVNT   ((uint16_t)0x0015U)//EventÅäÖÃ¼Ä´æÆ÷£º                    Õë¶ÔÃ¿Ò»¸ö¾ßÌåµÄevent½øĞĞÅäÖÃ£¬ÄÄ¸ö½Å£¬Òª²âÊ²Ã´ĞÅºÅ
#define DP83640_PTP_TXCFG0 ((uint16_t)0x0016U)//·¢ËÍPTP±¨ÎÄÅäÖÃ¼Ä´æÆ÷£º              Ê±¼ä´Á¼ÇÂ¼Ê¹ÄÜµÈ
#define DP83640_PTP_TXCFG1 ((uint16_t)0x0017U)//Î´ÖªÓÃÍ¾
#define DP83640_PTP_RXCFG0 ((uint16_t)0x0019U)//½ÓÊÕPTP±¨ÎÄÅäÖÃ¼Ä´æÆ÷£º              Ê±¼ä´Á¼ÇÂ¼Ê¹ÄÜµÈ
#define DP83640_PTP_RXCFG1 ((uint16_t)0x001AU)//Î´ÖªÓÃÍ¾
#define DP83640_PTP_RXCFG2 ((uint16_t)0x001BU)//IP¹ıÂËÆ÷¼Ä´æÆ÷£º                     ´æ·ÅipµØÖ·£¬ÓÃÓÚ²¶»ñ¸ÃipµÄPTP±¨ÎÄ²¢¼ÇÂ¼Ê±¼ä´Á£¨ÔÚ0x19ÖĞÊ¹ÄÜ£©
#define DP83640_PTP_TRDL   ((uint16_t)0x001EU)//ÁÙÊ±ËÙÂÊµ÷Õû³ÖĞøÊ±¼ä¼Ä´æÆ÷£¨µÍÎ»£©
#define DP83640_PTP_TRDH   ((uint16_t)0x001FU)//ÁÙÊ±ËÙÂÊµ÷Õû³ÖĞøÊ±¼ä¼Ä´æÆ÷£¨¸ßÎ»£©

//PAGE6 : PTP 1588 Configuration Register
#define DP83640_PTP_COC    ((uint16_t)0x0014U)//Êä³öÊ±ÖÓÅäÖÃ¼Ä´æÆ÷

//DP83640 BCR¼Ä´æÆ÷¸÷Î»ÃèÊö(FIXED)
#define DP83640_BCR_SOFT_RESET         ((uint16_t)0x8000U)
#define DP83640_BCR_LOOPBACK           ((uint16_t)0x4000U)
#define DP83640_BCR_SPEED_SELECT       ((uint16_t)0x2000U)
#define DP83640_BCR_AUTONEGO_EN        ((uint16_t)0x1000U)
#define DP83640_BCR_POWER_DOWN         ((uint16_t)0x0800U)
#define DP83640_BCR_ISOLATE            ((uint16_t)0x0400U)
#define DP83640_BCR_RESTART_AUTONEGO   ((uint16_t)0x0200U)
#define DP83640_BCR_DUPLEX_MODE        ((uint16_t)0x0100U) 

//DP83640µÄBSR¼Ä´æÆ÷¸÷Î»ÃèÊö(FIXED)
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

//DP83640 PHYSCSR¼Ä´æÆ÷¸÷Î»ÃèÊö(FIXED)  
#define DP83640_PHYSCSR_AUTONEGO_DONE  ((uint16_t)0x0010U)
#define DP83640_DUPLEX_STATUS          ((uint16_t)0x0004U)
#define DP83640_SPEED_STATUS           ((uint16_t)0x0002U)

//DP83640 MICR¼Ä´æÆ÷¸÷Î»ÃèÊö(FIXED)
#define DP83640_PTP_INT_SEL          ((uint16_t)0x0008U)//Ê¹ÄÜPTPÖĞ¶Ï(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_INT                  ((uint16_t)0x0004U)//²âÊÔÖĞ¶Ï£¬²úÉúÒ»¸öÖĞ¶ÏĞÅºÅ
#define DP83640_INTEN                ((uint16_t)0x0002U)//Ê¹ÄÜMISRÀï´ò¿ªµÄ¸÷¸öÖĞ¶Ï(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_INT_OE               ((uint16_t)0x0001U)//Ê¹ÄÜÖĞ¶ÏĞÅºÅ´ÓPWRDPWN½ÅÊä³(³õÊ¼»¯Ê¹ÄÜ)ö

//DP83640 MISR¼Ä´æÆ÷¸÷Î»ÃèÊö(FIXED)
#define DP83640_PTP_INT          ((uint16_t)0x0800U)//PTPÖĞ¶Ï±êÖ¾Î»

//DP83640µÄPTP_CTL¼Ä´æÆ÷¸÷Î»ÃèÊö(PAGE4)
#define DP83640_PTP_CTL_RD_CLK       ((uint16_t)0x0020U)
#define DP83640_PTP_CTL_LOAD_CLK     ((uint16_t)0x0010U)
#define DP83640_PTP_CTL_STEP_CLK     ((uint16_t)0x0008U) 
#define DP83640_PTP_CTL_ENABLE       ((uint16_t)0x0004U)//´ò¿ªPTPÊ±ÖÓ(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_PTP_CTL_DISABLE      ((uint16_t)0x0002U)//¹Ø±ÕPTPÊ±ÖÓ
#define DP83640_PTP_CTL_RESET        ((uint16_t)0x0001U)//ÖØÖÃPTPÊ±ÖÓ(Èí¼şÖÃ1ºó±ØĞëÊÖ¶¯ÖÃ0)

//DP83640µÄPTP_STS¼Ä´æÆ÷¸÷Î»ÃèÊö(PAGE4)
#define DP83640_PTP_STS_TXTS_RDY     ((uint16_t)0x0800U)//·¢ËÍ±¨ÎÄÊ±¼ä´Á×¼±¸ºÃ±êÖ¾
#define DP83640_PTP_STS_RXTS_RDY     ((uint16_t)0x0400U)//½ÓÊÕ±¨ÎÄÊ±¼ä´Á×¼±¸ºÃ±êÖ¾
#define DP83640_PTP_STS_TXTS_IE      ((uint16_t)0x0008U)//Ê¹ÄÜ·¢ËÍ±¨ÎÄÊ±¼ä´Á×¼±¸ºÃÖĞ¶Ï(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_PTP_STS_RXTS_IE      ((uint16_t)0x0004U)//Ê¹ÄÜ½ÓÊÕ±¨ÎÄÊ±¼ä´Á×¼±¸ºÃÖĞ¶Ï(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_PTP_STS_EVENT_IE      ((uint16_t)0x0001U)

//DP83640µÄPTP_RATEH¼Ä´æÆ÷¸÷Î»ÃèÊö(PAGE4)
#define DP83640_PTP_RATEH_RATE_DIR   ((uint16_t)0x8000U)//ËÙÂÊµ÷Õû·½Ïò
#define DP83640_PTP_RATEH_TEM_RATE   ((uint16_t)0x4000U)//ÁÙÊ±ËÙÂÊµ÷ÕûÄ£Ê½

//DP83640µÄPTP_TXCFG0¼Ä´æÆ÷¸÷Î»ÃèÊö(PAGE5)
#define DP83640_PTP_TXCFG0_SYNC_1STEP   ((uint16_t)0x8000U)//Ê¹ÄÜ×Ô¶¯´òÊ±¼ä´Á¹¦ÄÜ
#define DP83640_PTP_TXCFG0_TX_IPV4_EN   ((uint16_t)0x0020U)//Ê¹ÄÜ¶Ô»ùÓÚUDP/IPv4µÄPTP±¨ÎÄµÄ¼ì²â(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_PTP_TXCFG0_TX_TS_EN     ((uint16_t)0x0001U)//Ê¹ÄÜ·¢ËÍ±¨ÎÄµÄÊ±¼ä´Á×¥È¡(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_PTP_TXCFG0_TX_PTP_VER     ((uint16_t)0x0004U)

//DP83640µÄPTP_RXCFG0¼Ä´æÆ÷¸÷Î»ÃèÊö(PAGE5)
#define DP83640_PTP_RXCFG0_RX_IPV4_EN   ((uint16_t)0x0020U)//Ê¹ÄÜ¶Ô»ùÓÚUDP/IPv4µÄPTP±¨ÎÄµÄ¼ì²â(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_PTP_RXCFG0_RX_TS_EN     ((uint16_t)0x0001U)//Ê¹ÄÜ½ÓÊÕ±¨ÎÄµÄÊ±¼ä´Á×¥È¡(³õÊ¼»¯Ê¹ÄÜ)
#define DP83640_PTP_RXCFG0_RX_PTP_VER   ((uint16_t)0x0004U)

//DP83640µÄPTP_COC¼Ä´æÆ÷¸÷Î»ÃèÊö(PAGE6)
#define DP83640_PTP_COC_CLOOUT_EN     ((uint16_t)0x8000U)//Ê¹ÄÜPTP divide-by-N(default N equals to 10) Ê±ÖÓµÄÊä³ö(³õÊ¼»¯Ê¹ÄÜ)



//DP83640×´Ì¬Ïà¹Ø¶¨Òå
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


extern ETH_HandleTypeDef ETH_Handler;               //ÒÔÌ«Íø¾ä±ú
extern ETH_DMADescTypeDef *DMARxDscrTab;			//ÒÔÌ«ÍøDMA½ÓÊÕÃèÊö·ûÊı¾İ½á¹¹ÌåÖ¸Õë
extern ETH_DMADescTypeDef *DMATxDscrTab;			//ÒÔÌ«ÍøDMA·¢ËÍÃèÊö·ûÊı¾İ½á¹¹ÌåÖ¸Õë 
extern uint8_t *Rx_Buff; 							//ÒÔÌ«Íøµ×²ãÇı¶¯½ÓÊÕbuffersÖ¸Õë 
extern uint8_t *Tx_Buff; 							//ÒÔÌ«Íøµ×²ãÇı¶¯·¢ËÍbuffersÖ¸Õë
extern ETH_DMADescTypeDef  *DMATxDescToSet;			//DMA·¢ËÍÃèÊö·û×·×ÙÖ¸Õë
extern ETH_DMADescTypeDef  *DMARxDescToGet; 		//DMA½ÓÊÕÃèÊö·û×·×ÙÖ¸Õë 
 

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
