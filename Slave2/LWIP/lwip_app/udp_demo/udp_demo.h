#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "sys.h"
#include "lwip_comm.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F4&F7������
//UDP ���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/8/5
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define UDP_DEMO_RX_BUFSIZE		2000	//����udp���������ݳ��� 
#define UDP_DEMO_PORT			319	//����udp���ӵĶ˿� 
#define FPGA_RECIEVE_BUFSIZE  10000

//PTP_Header Define version:v2
#define MsgType_Sync 0x00
#define MsgType_Delay_Req 0x01
#define MsgType_Follow_Up 0x08
#define MsgType_Delay_Resp 0x09


 
void udp_demo_test(void);
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
void udp_demo_senddata(struct udp_pcb *upcb);
void udp_demo_connection_close(struct udp_pcb *upcb);
void PTP_Sync(struct udp_pcb *upcb);
void PTP_Delay_Req(struct udp_pcb *upcb);
void PTP_Delay_Resp(struct udp_pcb *upcb);
void PTP_Read_Tranmit_Reg(u8 rate_idx);
void PTP_Time_Correction(void);
void PTP_Rate_Correction(void);
void Get_Adc_Value(void);
void Response_Line_Delay(struct udp_pcb *upcb);
void UDP_SEND_TASK(struct udp_pcb *upcb);

#endif

