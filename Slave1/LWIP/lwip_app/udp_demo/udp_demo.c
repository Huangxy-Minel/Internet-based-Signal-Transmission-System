#include "udp_demo.h" 
#include "myspi.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
 
//UDP�������ݻ�����
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ����� 
//UDP������������
static u8 udp_send_buf[1001] = {0};
static u8 ad_msg[10000] = {0};
u8 *p = ad_msg;
u8 flag = 1; //0����ɷ���ad���ģ�1�����ɷ���
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
static u32 t1_rate[2][4]={0}, t2_rate[2][4]={0};  //PTPЭ�������Ϣ
static u32 t1[4]={0}, t2[4]={0}, t3[4]={0}, t4[4]={0};  //PTPЭ�������Ϣ
static u32 T1[2] = {0}, T2[2] = {0}, T3[2] = {0}, T4[2] = {0};
uint64_t delay=0, offset=0;
u8 delay_flag = 0, offset_flag = 0; //��¼����, 0Ϊ��
static float rate_ratio[2] = {0};
u8 rate_ratio_idx = 0;
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
 	u8 key,i,flag=0,rate_idx = 0;
	u32 j;
	u8 res=0;
	u16  recv_temp; 
 	rate_ratio[0] = 1;
	rate_ratio[1] = 1;
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
	udp_send_buf[1000] = 0x10; //����Ϊ�ӻ�1
	DP83640_selectPage(4);
	j = 0; //��¼ѭ������
	while(res==0)
	{
		if(udp_demo_flag&1<<6 && udp_demo_recvbuf[8] == 0x10)//�Ƿ��յ�����?
		{
			if(udp_demo_recvbuf[0] == 0x08 && udp_demo_recvbuf[9] == 0x00) //���յ�FLOWUP�ź���ΪRate Correction
			{
				PTP_Read_Tranmit_Reg(rate_idx);
				rate_idx++;
				flag = 1;
				if(rate_idx > 1)
				{
					rate_idx = 0;
					PTP_Rate_Correction(); //���±�������
				}
			}
			else if(udp_demo_recvbuf[0] == 0x08 && udp_demo_recvbuf[9] == 0x01) //���յ�FLOWUP�ź���ΪTime Correction
			{
				//��ȡt1
				for(i=0;i<4;i++)
				{
					t1[i] = udp_demo_recvbuf[2*i+34];
					recv_temp = udp_demo_recvbuf[2*i+35];
					t1[i] |= (recv_temp<<8);
				}
				PTP_Delay_Req(udppcb);
			}
			else if(udp_demo_recvbuf[0] == 0x09) //���յ�Delay_Resq�ź�
			{
				PTP_Time_Correction(); //�޸�ʱ�䲽��
				Response_Line_Delay(udppcb); //������·��ʱ��Ϣ
			}
			else if(udp_demo_recvbuf[0] == 0x03) //���յ�PTP_END�ź�
				flag = 0;
			udp_demo_flag&=~(1<<6);//��������Ѿ���������.
		}
		if(j<2500)
		{
			Get_Adc_Value();
			j++;
		}
		else
		{
			if(flag == 0)
			{
				j=0;
				p = ad_msg;
				UDP_SEND_TASK(udppcb);
			}

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


//�ر�UDP����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//�Ͽ�UDP���� 
	udp_demo_flag &= ~(1<<5);	//������ӶϿ�
}
void Response_Line_Delay(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	u8 response_msg[11] = {0};
	response_msg[0] = 0x02;
	response_msg[1] = delay_flag;
	response_msg[10] = 0x10;
	response_msg[2] = delay&0x00000000000000FF;
	response_msg[3] = (delay&0x000000000000FF00)>>8;
	response_msg[4] = (delay&0x0000000000FF0000)>>16;
	response_msg[5] = (delay&0x00000000FF000000)>>24;
	response_msg[6] = (delay&0x000000FF00000000)>>32;
	response_msg[7] = (delay&0x0000FF0000000000)>>40;
	response_msg[8] = (delay&0x00FF000000000000)>>48;
	response_msg[9] = (delay&0xFF00000000000000)>>56;
	
	ptr=pbuf_alloc(PBUF_TRANSPORT,11,PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr,(char*)response_msg,11); //��ptpsync�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	}
}
void PTP_Delay_Req(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	u8* ptp_delay_req = PTP_Header;
	ptp_delay_req[0] = MsgType_Delay_Req;
	ptp_delay_req[8] = 0x10;
	ptp_delay_req[33] = MsgType_Delay_Req;
	ptr=pbuf_alloc(PBUF_TRANSPORT,44,PBUF_POOL); //�����ڴ�
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_delay_req,44); //��ptpsync�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	}
}
void PTP_Read_Tranmit_Reg(u8 rate_idx)
{
	u8 i, t=0;
	u16 recv_temp=0;
	u32 regval,temp;
	for(i=0;i<4;i++) //��ȡt1_rate
	{
		t1_rate[rate_idx][i] = udp_demo_recvbuf[2*i+34];
		recv_temp = udp_demo_recvbuf[2*i+35];
		t1_rate[rate_idx][i] |= (recv_temp<<8);
	}
	while(1)
	{
		DP83640_ReadPHY(0x16,&regval);
		if(regval & 0x0400) //׼���ý���ʱ�����
		{
			DP83640_ReadPHY(0x1D,&t2_rate[rate_idx][0]); //��ȡ����ʱ����Ĵ���
			DP83640_ReadPHY(0x1D,&t2_rate[rate_idx][1]);
			DP83640_ReadPHY(0x1D,&t2_rate[rate_idx][2]);
			DP83640_ReadPHY(0x1D,&t2_rate[rate_idx][3]);
			for(i=0;i<8;i++)
				DP83640_ReadPHY(0x1D,&temp);
			break;
		}
		t++;
		if(t>5)
			break;
	}
}
void PTP_Rate_Correction(void)
{
	u32 T1[2] = {0}, T2[2] = {0}, T1N[2] = {0}, T2N[2] = {0};
	uint64_t T1_64=0, T2_64=0,T1N_64=0, T2N_64=0,temp;
	T1[0] = t1_rate[0][0];
	T1[0] |= t1_rate[0][1]<<16;
	T1[1] = t1_rate[0][2];
	T1[1] |= t1_rate[0][3]<<16;
	
	T1N[0] = t1_rate[1][0];
	T1N[0] |= t1_rate[1][1]<<16;
	T1N[1] = t1_rate[1][2];
	T1N[1] |= t1_rate[1][3]<<16;
	
	T2[0] = t2_rate[0][0];
	T2[0] |= t2_rate[0][1]<<16;
	T2[1] = t2_rate[0][2];
	T2[1] |= t2_rate[0][3]<<16;
	
	T2N[0] = t2_rate[1][0];
	T2N[0] |= t2_rate[1][1]<<16;
	T2N[1] = t2_rate[1][2];
	T2N[1] |= t2_rate[1][3]<<16;
	
	T1_64 = T1[0];
	T2_64 = T2[0];
	T1N_64 = T1N[0];
	T2N_64 = T2N[0];
	
	temp = T1[1];
	T1_64 |= temp<<32;
	temp = T2[1];
	T2_64 |= temp<<32;
	temp = T1N[1];
	T1N_64 |= temp<<32;
	temp = T2N[1];
	T2N_64 |= temp<<32;
	//����Ƶ�ʲ�
	if(rate_ratio_idx == 0)
	{
		rate_ratio[1] = (float)(T2N_64-T2_64) / (float)(T1N_64-T1_64);
		rate_ratio[1] = rate_ratio[0] + 0.1f * (rate_ratio[1] - rate_ratio[0]);
		rate_ratio_idx = 1;
	}
	else
	{
		rate_ratio[0] = (float)(T2N_64-T2_64) / (float)(T1N_64-T1_64);
		rate_ratio[0] = rate_ratio[1] + 0.1f * (rate_ratio[0] - rate_ratio[1]);
		rate_ratio_idx = 0;
	}
}
void PTP_Time_Correction(void)
{
//��ȡt4
	u8 i, t=0;
	u16 recv_temp=0;
	u32 regval,temp;
	uint64_t T1_64=0, T2_64=0,T3_64=0, T4_64=0,temp_64;
	for(i=0;i<4;i++)
	{
		t4[i] = udp_demo_recvbuf[2*i+34];
		recv_temp = udp_demo_recvbuf[2*i+35];
		t4[i] |= (recv_temp<<8);
	}
	//��ȡt2
	t=0;
	while(1)
	{
		DP83640_ReadPHY(0x16,&regval);
		if(regval & 0x0400) //׼���ý���ʱ�����
		{
			DP83640_ReadPHY(0x1D,&t2[0]); //��ȡ����ʱ����Ĵ���
			DP83640_ReadPHY(0x1D,&t2[1]);
			DP83640_ReadPHY(0x1D,&t2[2]);
			DP83640_ReadPHY(0x1D,&t2[3]);
			break;
		}
		if(t>5)
			break;
	}
	//��ȡt3
	t=0;
	while(1)
	{
		DP83640_ReadPHY(0x16,&regval);
		if(regval & 0x0800) //׼���ý���ʱ�����
		{
			DP83640_ReadPHY(0x1C,&t3[0]); //��ȡ����ʱ����Ĵ���
			DP83640_ReadPHY(0x1C,&t3[1]);
			DP83640_ReadPHY(0x1C,&t3[2]);
			DP83640_ReadPHY(0x1C,&t3[3]);
			//flag = 1; //PTPЭ�����ʱ�Ӷ�ȡ���
			break;
		}
		if(t>5)
			break;
	}
	//��ռĴ���
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
	//�ϲ���32λ�޷�����
	T1[0] = t1[0];
	T2[0] = t2[0];
	T3[0] = t3[0];
	T4[0] = t4[0];

	T1[1] = t1[2];
	T2[1] = t2[2];
	T3[1] = t3[2];
	T4[1] = t4[2];

	T1[0] |= t1[1]<<16;
	T2[0] |= t2[1]<<16;
	T3[0] |= t3[1]<<16;
	T4[0] |= t4[1]<<16;

	T1[1] |= t1[3]<<16;
	T2[1] |= t2[3]<<16;
	T3[1] |= t3[3]<<16;
	T4[1] |= t4[3]<<16;
	//�ϲ�Ϊ64λ��
	T1_64 = T1[0];
	T2_64 = T2[0];
	T3_64 = T3[0];
	T4_64 = T4[0];
	
	temp_64 = T1[1];
	T1_64 |= temp_64<<32;
	temp_64 = T2[1];
	T2_64 |= temp_64<<32;
	temp_64 = T3[1];
	T3_64 |= temp_64<<32;
	temp_64 = T4[1];
	T4_64 |= temp_64<<32;
	//����delay
	if((T4_64+T2_64) > (T3_64+T1_64))
	{
		delay = (T4_64 - T3_64 + T2_64 - T1_64)/2;
		delay_flag = 0;
	}
	else
	{
		delay = (-T4_64 + T3_64 - T2_64 + T1_64)/2;
		delay_flag = 1;
	}
	//����offset
	if((T2_64+T3_64) > (T1_64+T4_64))
	{
		offset = (T2_64 - T1_64 - T4_64 + T3_64)/2;
		offset_flag = 0;
	}
	else
	{
		offset = (-T2_64 + T1_64 + T4_64 - T3_64)/2;
		offset_flag = 1;
	}
}

void Get_Adc_Value(void)
{
	u8 temp;
	SCLK(0);
	temp = D0;
	*p = temp;
	temp = D1;
	*p |= temp<<1;
	temp = D2;
	*p |= temp<<2;
	temp = D3;
	*p |= temp<<3;
	temp = D4;
	*p |= temp<<4;
	temp = D5;
	*p |= temp<<5;
	temp = D6;
	*p |= temp<<6;
	temp = D7;
	*p |= temp<<7;
	p++;
	
	temp = D8;
	*p = temp;
	temp = D9;
	*p |= temp<<1;
	temp = D10;
	*p |= temp<<2;
	temp = D11;
	*p |= temp<<3;
	temp = D12;
	*p |= temp<<4;
	temp = D13;
	*p |= temp<<5;
	temp = D14;
	*p |= temp<<6;
	temp = D15;
	*p |= temp<<7;
	p++;
	
	temp = D16;
	*p = temp;
	temp = D17;
	*p |= temp<<1;
	temp = D18;
	*p |= temp<<2;
	temp = D19;
	*p |= temp<<3;
	temp = D20;
	*p |= temp<<4;
	temp = D21;
	*p |= temp<<5;
	temp = D22;
	*p |= temp<<6;
	temp = D23;
	*p |= temp<<7;
	p++;
	
	temp = D24;
	*p = temp;
	temp = D25;
	*p |= temp<<1;
	temp = D26;
	*p |= temp<<2;
	temp = D27;
	*p |= temp<<3;
	temp = D28;
	*p |= temp<<4;
	temp = D29;
	*p |= temp<<5;
	temp = D30;
	*p |= temp<<6;
	temp = D31;
	*p |= temp<<7;
	SCLK(1);
	p++;
}
void UDP_SEND_TASK(struct udp_pcb *upcb)
{
	u8 i;
	u8* p_temp = ad_msg;
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,1001,PBUF_POOL); //�����ڴ�
	for(i=0;i<10;i++)
	{
		memcpy(udp_send_buf, p_temp, 1000);
		pbuf_take(ptr,(char*)udp_send_buf,1001); //��ptpsync�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		p_temp += 1000;
	}
	pbuf_free(ptr);//�ͷ��ڴ�
}















