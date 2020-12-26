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
 
//UDP接收数据缓冲区
static u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区 
static u8 ad_recvbuf_1[12000];	//UDP接收数据缓冲区 
static u8 ad_recvbuf_2[12000];	//UDP接收数据缓冲区 
static u8* p1 = ad_recvbuf_1;
static u8* p2 = ad_recvbuf_2;
u8 *p = udp_demo_recvbuf;
u8 flag_1 = 0, flag_2 = 0; //记录接受数据次数
uint64_t line_delay_1 = 0, line_delay_2 = 0;
u8 line_delay_1_flag = 0, line_delay_2_flag = 0;
//UDP发送数据内容
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

//UDP 测试全局状态标记变量
//bit7:没有用到
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上;1,连接上了.
//bit4~0:保留
u8 udp_demo_flag;

//设置远端IP地址
void udp_demo_set_remoteip(void)
{
	//前三个IP保持和DHCP得到的IP一致
	lwipdev.remoteip[0]=lwipdev.ip[0];
	lwipdev.remoteip[1]=lwipdev.ip[1];
	lwipdev.remoteip[2]=lwipdev.ip[2]; 
} 

//UDP测试
void udp_demo_test(void)
{
 	err_t err;
	struct udp_pcb *udppcb;  	//定义一个TCP服务器控制块
	struct ip_addr rmtipaddr;  	//远端ip地址
 	
	u8 *tbuf;
 	u8 key;
	u8 res=0;		 
 	
	udp_demo_set_remoteip();//先选择IP
	udppcb=udp_new();
	if(udppcb)//创建成功
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
		err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP客户端连接到指定IP地址和端口号的服务器
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//绑定本地IP地址与端口号
			if(err==ERR_OK)	//绑定完成
			{
				udp_recv(udppcb,udp_demo_recv,NULL);//注册接收回调函数 
				udp_demo_flag |= 1<<5;			//标记已经连接上
				POINT_COLOR=RED;
			}else res=1;
		}else res=1;		
	}else res=1;
	//首次PTP_Correct
		//PTP_Correct(udppcb);
	while(res==0)
	{
		if(udp_demo_flag&1<<6)//是否收到数据?
		{
			if(udp_demo_recvbuf[Rx_size] == 0x10 && udp_demo_recvbuf[Rx_size+1] == 0x02) //接受到了从机2的ad报文，且为报头
			{
				Send_FPGA(0);
				flag_1 = 0;
			}
			else if(udp_demo_recvbuf[Rx_size] == 0x10 && udp_demo_recvbuf[Rx_size+1] == 0x01) //接受到了从机2的ad报文，且为报尾
			{
				Send_FPGA(0);
				flag_1 = 1;
			}
			
			else if(udp_demo_recvbuf[Rx_size] == 0x11 && udp_demo_recvbuf[Rx_size+1] == 0x02) //接受到了从机2的ad报文，且为报头
			{
				Send_FPGA(1);
				flag_2 = 0;
			}
			else if(udp_demo_recvbuf[Rx_size] == 0x11 && udp_demo_recvbuf[Rx_size+1] == 0x01) //接受到了从机2的ad报文，且为报尾
			{
				Send_FPGA(1);
				flag_2 = 1;
			}
			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
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

//UDP回调函数
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//接收到不为空的数据时
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //数据接收缓冲区清零
		for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
		{
			//判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，如果大于
			//的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//拷贝数据
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
		}
		upcb->remote_ip=*addr; 				//记录远程主机的IP地址
		upcb->remote_port=port;  			//记录远程主机的端口号
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		udp_demo_flag|=1<<6;	//标记接收到数据了
		pbuf_free(p);//释放内存
	}else
	{
		udp_disconnect(upcb); 
		udp_demo_flag &= ~(1<<5);	//标记连接断开
	} 
} 

//UDP服务器发送数据
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_demo_sendbuf),PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(char*)tcp_demo_sendbuf,strlen((char*)tcp_demo_sendbuf)); //将tcp_demo_sendbuf中的数据打包进pbuf结构中
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
	} 
} 
//关闭UDP连接
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//断开UDP连接 
	udp_demo_flag &= ~(1<<5);	//标记连接断开
}
//发送PTP同步信号
void PTP_Sync(struct udp_pcb *upcb, u8 num, u8 type) //type :0x00-Rate Correction;0x01-Time Correction 
{																									//num  :0x00-从机1;0x01-从机2
	struct pbuf *ptr;																//返回0，正常
	u8 t=0;
	u32 regval =0, temp;
	u32 Timestamp_ns_low=0, Timestamp_ns_high=0, Timestamp_sec_low=0, Timestamp_sec_high=0;
	u8* ptp_sync = PTP_Header;
	ptp_sync[0] = MsgType_Sync;
	switch(num) //选择从机编号
	{
		case 0:ptp_sync[8] = 0x10;break;
		case 1:ptp_sync[8] = 0x11;break;
	}
	switch(type)
	{
		case 0:ptp_sync[9] = 0x00;break;
		case 1:ptp_sync[9] = 0x01;break;
	}
	ptr=pbuf_alloc(PBUF_TRANSPORT,44,PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_sync,44); //将ptpsync中的数据打包进pbuf结构中
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
	}
	DP83640_selectPage(4);
	while(1)
	{
		DP83640_ReadPHY(0x16,&regval);
		if(regval & 0x0800) //准备好发送时间戳了
		{
			DP83640_ReadPHY(0x1C,&Timestamp_ns_low); //读取发送时间戳寄存器
			DP83640_ReadPHY(0x1C,&Timestamp_ns_high);
			DP83640_ReadPHY(0x1C,&Timestamp_sec_low);
			DP83640_ReadPHY(0x1C,&Timestamp_sec_high);

			break;
		}
		if(t>5)
			return;
		t++;
	}
	//发送Follow_up报文
	ptp_sync[0] = MsgType_Follow_Up; //修改PTP报头
	ptp_sync[33] = 0x02;
	//添加时间戳
	ptp_sync[34] = Timestamp_ns_low&0x00FF;
	ptp_sync[35] = (Timestamp_ns_low&0xFF00)>>8;
	ptp_sync[36] = Timestamp_ns_high&0x00FF;
	ptp_sync[37] = (Timestamp_ns_high&0xFF00)>>8;
	ptp_sync[38] = Timestamp_sec_low&0x00FF;
	ptp_sync[39] = (Timestamp_sec_low&0xFF00)>>8;
	ptp_sync[40] = Timestamp_sec_high&0x00FF;
	ptp_sync[41] = (Timestamp_sec_high&0xFF00)>>8;
	ptr=pbuf_alloc(PBUF_TRANSPORT,44,PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_sync,44); //将ptpsync中的数据打包进pbuf结构中
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
	}
	DP83640_ReadPHY(0x16,&regval);
	while(regval & 0x0800)								//清空发送时间戳寄存器
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
	//获取接受时间戳
	while(1)
	{
		DP83640_ReadPHY(0x16,&regval);
		if(regval & 0x0400) //准备好接受时间戳了
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
	switch(num) //选择从机编号
	{
		case 0:ptp_delay_resp[8] = 0x10;break;
		case 1:ptp_delay_resp[8] = 0x11;break;
	}
	//添加时间戳
	ptp_delay_resp[34] = Timestamp_ns_low&0x00FF;
	ptp_delay_resp[35] = (Timestamp_ns_low&0xFF00)>>8;
	ptp_delay_resp[36] = Timestamp_ns_high&0x00FF;
	ptp_delay_resp[37] = (Timestamp_ns_high&0xFF00)>>8;
	ptp_delay_resp[38] = Timestamp_sec_low&0x00FF;
	ptp_delay_resp[39] = (Timestamp_sec_low&0xFF00)>>8;
	ptp_delay_resp[40] = Timestamp_sec_high&0x00FF;
	ptp_delay_resp[41] = (Timestamp_sec_high&0xFF00)>>8;
	ptr=pbuf_alloc(PBUF_TRANSPORT,44,PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_delay_resp,44); //将ptpsync中的数据打包进pbuf结构中
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
	}
	
	DP83640_ReadPHY(0x16,&regval);
	while(regval & 0x0800)								//清空发送时间戳寄存器
	{
		DP83640_ReadPHY(0x1C,&temp);
		DP83640_ReadPHY(0x16,&regval);
	}
	DP83640_ReadPHY(0x16,&regval);
	while(regval & 0x0400)								//清空发送时间戳寄存器
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
	//停止发送报文,开始PTP校验
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
		if(udp_demo_flag&1<<6)//是否收到数据?
		{
			if(udp_demo_recvbuf[0] == 0x01 && udp_demo_recvbuf[8] == 0x10 && udp_demo_recvbuf[Rx_size] == 0x00) //接收到Delay_Request
				PTP_Delay_Resp(udppcb, 0x00);
			else if(udp_demo_recvbuf[0] == 0x02 && udp_demo_recvbuf[10] == 0x10 && udp_demo_recvbuf[Rx_size] == 0x00) //接收到line_delay_1
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
			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
		}
		t++;
		if(t>11)
			break; //从机1校验结束
	}
	t=0;
	PTP_Sync(udppcb,0x00,0x00);
	PTP_Sync(udppcb,0x01,0x00);
	while(1)
	{
		if(udp_demo_flag&1<<6)//是否收到数据?
		{
			if(udp_demo_recvbuf[0] == 0x05 && udp_demo_recvbuf[8] == 0x11) //接受到Ack
				break;
		}
		delay_ms(10);
		PTP_Sync(udppcb,0x01,0x00);
	}
	udp_demo_flag&=~(1<<6);
	PTP_Sync(udppcb,0x01,0x01);
	while(1)
	{
		if(udp_demo_flag&1<<6)//是否收到数据?
		{
			if(udp_demo_recvbuf[0] == 0x01 && udp_demo_recvbuf[8] == 0x11 && udp_demo_recvbuf[Rx_size] == 0x00) //接收到Delay_Request
				PTP_Delay_Resp(udppcb, 0x01);
			else if(udp_demo_recvbuf[0] == 0x02 && udp_demo_recvbuf[10] == 0x11 && udp_demo_recvbuf[Rx_size] == 0x00) //接收到line_delay_1
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
				udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
				break;
			}
			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
		}
		t++;
		delay_ms(10);
		if(t>11)
			break; //从机2校验结束
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
	switch(num) //选择从机编号
	{
		case 0:ptp_end[8] = 0x10;break;
		case 1:ptp_end[8] = 0x11;break;
	}
	ptr=pbuf_alloc(PBUF_TRANSPORT,9,PBUF_POOL); //申请内存
	if(ptr)
	{
		pbuf_take(ptr,(char*)ptp_end,9); //将ptpsync中的数据打包进pbuf结构中
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
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





















