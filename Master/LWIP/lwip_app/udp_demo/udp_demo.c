#include "udp_demo.h" 
#include "lan8720.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "myspi.h"
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
 
//UDP�������ݻ�����
static u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ����� 
static u8 ad_recvbuf_1[12000];	//UDP�������ݻ����� 
static u8 ad_recvbuf_2[12000];	//UDP�������ݻ����� 
static u8* p1 = ad_recvbuf_1;
static u8* p2 = ad_recvbuf_2;
u8 *p = udp_demo_recvbuf;
u8 flag_1 = 0, flag_2 = 0; //��¼�������ݴ���
uint64_t line_delay_1 = 0, line_delay_2 = 0;
u8 line_delay_1_flag = 0, line_delay_2_flag = 0;
//UDP������������
u8 *tcp_demo_sendbuf="Apollo STM32F4/F7 UDP demo send data\r\n";
//1:MsgType_TranSpec  VerPTP_Reserved  MsgLength
//2:DomainNumber  Reserved  FlagField
//3:CorrectionField
//4:Reserved
//5:SourcePortIdentity
u8 PTP_Header[44] = {0x00,0x02,0x2C,0x00
										,0x00,0x00,0x05,0x00
										,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
										,0x00,0x00,0x00,0x00
										,0x01,0x00,0x00,0x00,0x10,0x9e,0x25,0x00,0x99,0x1F
										,0xB8,0x04
										,0x00,0x00
										,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11};

//UDP ����ȫ��״̬��Ǳ���
//bit7:û���õ�
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û��������;1,��������.
//bit4~0:����
u8 udp_demo_flag;

//����Զ��IP��ַ
void udp_demo_set_remoteip(void)
{
	//ǰ����IP���ֺ�DHCP�õ���IPһ��
	lwipdev.remoteip[0]=lwipdev.ip[0];
	lwipdev.remoteip[1]=lwipdev.ip[1];
	lwipdev.remoteip[2]=lwipdev.ip[2]; 
} 

//UDP����
void udp_demo_test(void)
{
 	err_t err;
	struct udp_pcb *udppcb;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
 	
	u8 *tbuf;
 	u8 key;
	u8 res=0;		 
 	
	udp_demo_set_remoteip();//��ѡ��IP
	udppcb=udp_new();
	if(udppcb)//�����ɹ�
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
		err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{
				udp_recv(udppcb,udp_demo_recv,NULL);//ע����ջص����� 
				udp_demo_flag |= 1<<5;			//����Ѿ�������
				POINT_COLOR=RED;
			}else res=1;
		}else res=1;		
	}else res=1;
	//�״�PTP_Correct
		//PTP_Correct(udppcb);
	while(res==0)
	{
		if(udp_demo_flag&1<<6)//�Ƿ��յ�����?
		{
			if(udp_demo_recvbuf[Rx_size] == 0x10 && udp_demo_recvbuf[Rx_size+1] == 0x02) //���ܵ��˴ӻ�2��ad���ģ���Ϊ��ͷ
			{
				Send_FPGA(0);
				flag_1 = 0;
			}
			else if(udp_demo_recvbuf[Rx_size] == 0x10 && udp_demo_recvbuf[Rx_size+1] == 0x01) //���ܵ��˴ӻ�2��ad���ģ���Ϊ��β
			{
				Send_FPGA(0);
				flag_1 = 1;
			}
			
			else if(udp_demo_recvbuf[Rx_size] == 0x11 && udp_demo_recvbuf[Rx_size+1] == 0x02) //���ܵ��˴ӻ�2��ad���ģ���Ϊ��ͷ
			{
				Send_FPGA(1);
				flag_2 = 0;
			}
			else if(udp_demo_recvbuf[Rx_size] == 0x11 && udp_demo_recvbuf[Rx_size+1] == 0x01) //���ܵ��˴ӻ�2��ad���ģ���Ϊ��β
			{
				Send_FPGA(1);
				flag_2 = 1;
			}
			udp_demo_flag&=~(1<<6);//��������Ѿ���������.
		}
		if(flag_1 && flag_2)
		{
			FIFO(1);
			EN_DA0(1);
			//PTP_Correct(udppcb);
			flag_1 = 0;
			flag_2 = 0;
			delay_ms(10);
			FIFO(0);
			EN_DA0(0);
		}
		lwip_periodic_handle();
	}
	udp_demo_connection_close(udppcb); 
	myfree(SRAMIN,tbuf);
} 

//UDP�ص�����
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		udp_demo_flag|=1<<6;	//��ǽ��յ�������
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
		udp_demo_flag &= ~(1<<5);	//������ӶϿ�
	} 
} 

//UDP��������������
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_demo_sendbuf),PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr,(char*)tcp_demo_sendbuf,strlen((char*)tcp_demo_sendbuf)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	} 
} 
//�ر�UDP����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//�Ͽ�UDP���� 
	udp_demo_flag &= ~(1<<5);	//������ӶϿ�
}
//����PTPͬ���ź�
void PTP_Sync(struct udp_pcb *upcb, u8 num, u8 type) //type :0x00-Rate Correction;0x01-Time Correction 
{																									//num  :0x00-�ӻ�1;0x01-�ӻ�2
	struct pbuf *ptr;																//����0������
	u8 t=0;
	u32 regval =0, temp;
	u32 Timestamp_ns_low=0, Timestamp_ns_high=0, Timestamp_sec_low=0, Timestamp_sec_high=0;
	u8* ptp_sync = PTP_Header;
	ptp_sync[0] = MsgType_Sync;
	switch(num) //ѡ��ӻ����
	{
		case 0:ptp_sync[8] = 0x10;break;
		case 1:ptp_sync[8] = 0x11;break;
	}
	switch(type)
	{
		case 0:ptp_sync[9] = 0x00;break;
		case 1:ptp_sync[9] = 0x01;break;
	}
	ptr=pbuf_alloc(PBUF_TRANSPORT,44,PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_sync,44); //��ptpsync�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	}
	DP83640_selectPage(4);
	while(1)
	{
		DP83640_ReadPHY(0x16,&regval);
		if(regval & 0x0800) //׼���÷���ʱ�����
		{
			DP83640_ReadPHY(0x1C,&Timestamp_ns_low); //��ȡ����ʱ����Ĵ���
			DP83640_ReadPHY(0x1C,&Timestamp_ns_high);
			DP83640_ReadPHY(0x1C,&Timestamp_sec_low);
			DP83640_ReadPHY(0x1C,&Timestamp_sec_high);

			break;
		}
		if(t>5)
			return;
		t++;
	}
	//����Follow_up����
	ptp_sync[0] = MsgType_Follow_Up; //�޸�PTP��ͷ
	ptp_sync[33] = 0x02;
	//���ʱ���
	ptp_sync[34] = Timestamp_ns_low&0x00FF;
	ptp_sync[35] = (Timestamp_ns_low&0xFF00)>>8;
	ptp_sync[36] = Timestamp_ns_high&0x00FF;
	ptp_sync[37] = (Timestamp_ns_high&0xFF00)>>8;
	ptp_sync[38] = Timestamp_sec_low&0x00FF;
	ptp_sync[39] = (Timestamp_sec_low&0xFF00)>>8;
	ptp_sync[40] = Timestamp_sec_high&0x00FF;
	ptp_sync[41] = (Timestamp_sec_high&0xFF00)>>8;
	ptr=pbuf_alloc(PBUF_TRANSPORT,44,PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_sync,44); //��ptpsync�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	}
	DP83640_ReadPHY(0x16,&regval);
	while(regval & 0x0800)								//��շ���ʱ����Ĵ���
	{
		DP83640_ReadPHY(0x1C,&temp);
		DP83640_ReadPHY(0x16,&regval);
	}
}
void PTP_Delay_Resp(struct udp_pcb *upcb, u8 num)
{
	struct pbuf *ptr;
	u8* ptp_delay_resp = PTP_Header;
	u32 regval =0, temp;
	u32 Timestamp_ns_low=0, Timestamp_ns_high=0, Timestamp_sec_low=0, Timestamp_sec_high=0;
	//��ȡ����ʱ���
	while(1)
	{
		DP83640_ReadPHY(0x16,&regval);
		if(regval & 0x0400) //׼���ý���ʱ�����
		{
			DP83640_ReadPHY(0x1D,&Timestamp_ns_low); 
			DP83640_ReadPHY(0x1D,&Timestamp_ns_high);
			DP83640_ReadPHY(0x1D,&Timestamp_sec_low);
			DP83640_ReadPHY(0x1D,&Timestamp_sec_high);
			break;
		}
	}
	ptp_delay_resp[0] = MsgType_Delay_Resp;
	ptp_delay_resp[33] = 0x03;
	switch(num) //ѡ��ӻ����
	{
		case 0:ptp_delay_resp[8] = 0x10;break;
		case 1:ptp_delay_resp[8] = 0x11;break;
	}
	//���ʱ���
	ptp_delay_resp[34] = Timestamp_ns_low&0x00FF;
	ptp_delay_resp[35] = (Timestamp_ns_low&0xFF00)>>8;
	ptp_delay_resp[36] = Timestamp_ns_high&0x00FF;
	ptp_delay_resp[37] = (Timestamp_ns_high&0xFF00)>>8;
	ptp_delay_resp[38] = Timestamp_sec_low&0x00FF;
	ptp_delay_resp[39] = (Timestamp_sec_low&0xFF00)>>8;
	ptp_delay_resp[40] = Timestamp_sec_high&0x00FF;
	ptp_delay_resp[41] = (Timestamp_sec_high&0xFF00)>>8;
	ptr=pbuf_alloc(PBUF_TRANSPORT,44,PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_delay_resp,44); //��ptpsync�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	}
	
	DP83640_ReadPHY(0x16,&regval);
	while(regval & 0x0800)								//��շ���ʱ����Ĵ���
	{
		DP83640_ReadPHY(0x1C,&temp);
		DP83640_ReadPHY(0x16,&regval);
	}
	DP83640_ReadPHY(0x16,&regval);
	while(regval & 0x0400)								//��շ���ʱ����Ĵ���
	{
		DP83640_ReadPHY(0x1D,&temp);
		DP83640_ReadPHY(0x16,&regval);
	}
}

void PTP_Correct(struct udp_pcb *udppcb)
{
	u8 t=0;
	u8 i;
	u32 regval;
	uint64_t temp;
	//ֹͣ���ͱ���,��ʼPTPУ��
	PTP_Sync(udppcb,0x00,0x00);
	PTP_Sync(udppcb,0x01,0x00);
	for(i=0;i<10;i++)
	{
		delay_ms(10);
		PTP_Sync(udppcb,0x00,0x00);
		PTP_Sync(udppcb,0x01,0x00);
	}
	PTP_Sync(udppcb,0x00,0x01);
	while(1)
	{
		if(udp_demo_flag&1<<6)//�Ƿ��յ�����?
		{
			if(udp_demo_recvbuf[0] == 0x01 && udp_demo_recvbuf[8] == 0x10 && udp_demo_recvbuf[Rx_size] == 0x00) //���յ�Delay_Request
				PTP_Delay_Resp(udppcb, 0x00);
			else if(udp_demo_recvbuf[0] == 0x02 && udp_demo_recvbuf[10] == 0x10 && udp_demo_recvbuf[Rx_size] == 0x00) //���յ�line_delay_1
			{
				line_delay_1_flag = udp_demo_recvbuf[1];
				line_delay_1 = udp_demo_recvbuf[2];
				temp = udp_demo_recvbuf[3];
				line_delay_1 |= temp<<8;
				temp = udp_demo_recvbuf[4];
				line_delay_1 |= temp<<16;
				temp = udp_demo_recvbuf[5];
				line_delay_1 |= temp<<24;
				temp = udp_demo_recvbuf[6];
				line_delay_1 |= temp<<32;
				temp = udp_demo_recvbuf[7];
				line_delay_1 |= temp<<40;
				temp = udp_demo_recvbuf[8];
				line_delay_1 |= temp<<48;
				temp = udp_demo_recvbuf[9];
				line_delay_1 |= temp<<56;
				udp_demo_flag&=~(1<<6);
				break;
			}
			udp_demo_flag&=~(1<<6);//��������Ѿ���������.
		}
		t++;
		if(t>11)
			break; //�ӻ�1У�����
	}
	t=0;
	PTP_Sync(udppcb,0x00,0x00);
	PTP_Sync(udppcb,0x01,0x00);
	while(1)
	{
		if(udp_demo_flag&1<<6)//�Ƿ��յ�����?
		{
			if(udp_demo_recvbuf[0] == 0x05 && udp_demo_recvbuf[8] == 0x11) //���ܵ�Ack
				break;
		}
		delay_ms(10);
		PTP_Sync(udppcb,0x01,0x00);
	}
	udp_demo_flag&=~(1<<6);
	PTP_Sync(udppcb,0x01,0x01);
	while(1)
	{
		if(udp_demo_flag&1<<6)//�Ƿ��յ�����?
		{
			if(udp_demo_recvbuf[0] == 0x01 && udp_demo_recvbuf[8] == 0x11 && udp_demo_recvbuf[Rx_size] == 0x00) //���յ�Delay_Request
				PTP_Delay_Resp(udppcb, 0x01);
			else if(udp_demo_recvbuf[0] == 0x02 && udp_demo_recvbuf[10] == 0x11 && udp_demo_recvbuf[Rx_size] == 0x00) //���յ�line_delay_1
			{
				line_delay_2_flag = udp_demo_recvbuf[1];
				line_delay_2 = udp_demo_recvbuf[2];
				temp = udp_demo_recvbuf[3];
				line_delay_2 |= temp<<8;
				temp = udp_demo_recvbuf[4];
				line_delay_2 |= temp<<16;
				temp = udp_demo_recvbuf[5];
				line_delay_2 |= temp<<24;
				temp = udp_demo_recvbuf[6];
				line_delay_2 |= temp<<32;
				temp = udp_demo_recvbuf[7];
				line_delay_2 |= temp<<40;
				temp = udp_demo_recvbuf[8];
				line_delay_2 |= temp<<48;
				temp = udp_demo_recvbuf[9];
				line_delay_2 |= temp<<56;
				udp_demo_flag&=~(1<<6);//��������Ѿ���������.
				break;
			}
			udp_demo_flag&=~(1<<6);//��������Ѿ���������.
		}
		t++;
		delay_ms(10);
		if(t>11)
			break; //�ӻ�2У�����
	}
	PTP_Sync(udppcb,0x01,0x00);
	delay_ms(10);
	PTP_END(udppcb,0x00);
	delay_ms(10);
	PTP_END(udppcb,0x01);
	delay_ms(10);
}
void PTP_END(struct udp_pcb *upcb, u8 num)
{
	u8 ptp_end[9] = {0};
	struct pbuf *ptr;
	ptp_end[0] = 0x03;
	switch(num) //ѡ��ӻ����
	{
		case 0:ptp_end[8] = 0x10;break;
		case 1:ptp_end[8] = 0x11;break;
	}
	ptr=pbuf_alloc(PBUF_TRANSPORT,9,PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_end,9); //��ptpsync�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	}
}
void Send_FPGA(u8 num) //0:DAC1, 1:DAC2
{
	u32 i;
	ENABLE(num);
	for(i=0; i<Rx_size/4; i++)
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_3,GPIO_PIN_RESET);	
		D0(*p&0x01);
		D1(*p&0x02);
		D2(*p&0x04);
		D3(*p&0x08);
		D4(*p&0x10);
		D5(*p&0x20);
		D6(*p&0x40);
		D7(*p&0x80);
		p++;
		
		D8(*p&0x01);
		D9(*p&0x02);
		D10(*p&0x04);
		D11(*p&0x08);
		D12(*p&0x10);
		D13(*p&0x20);
		D14(*p&0x40);
		D15(*p&0x80);
		p++;
		
		D16(*p&0x01);
		D17(*p&0x02);
		D18(*p&0x04);
		D19(*p&0x08);
		D20(*p&0x10);
		D21(*p&0x20);
		D22(*p&0x40);
		D23(*p&0x80);
		p++;
		
		D24(*p&0x01);
		D25(*p&0x02);
		D26(*p&0x04);
		D27(*p&0x08);
		D28(*p&0x10);
		D29(*p&0x20);
		D30(*p&0x40);
		D31(*p&0x80);
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_3,GPIO_PIN_SET);	
		p++;
	}
	p = udp_demo_recvbuf;
}





















