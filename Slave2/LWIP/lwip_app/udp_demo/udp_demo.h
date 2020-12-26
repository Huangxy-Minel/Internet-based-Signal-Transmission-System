#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "sys.h"
#include "lwip_comm.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F4&F7开发板
//UDP 测试代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/8/5
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define UDP_DEMO_RX_BUFSIZE		2000	//定义udp最大接收数据长度 
#define UDP_DEMO_PORT			319	//定义udp连接的端口 
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

