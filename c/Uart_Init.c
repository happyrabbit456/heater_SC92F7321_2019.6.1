#include "H/Function_Init.H" 

void Uart0_Init(void);
void UartInt_Handle();
void Uart_Process();

bit UartSendFlag = 0; //发送中断标志位
bit UartReceiveFlag = 0; //接收中断标志位
/*****************************************************
*函数名称：void Uart0_Test(void)
*函数功能：Uart0测试
*入口参数：void
*出口参数：void
*****************************************************/
void Uart0_Test(void)
{
	Uart0_Init();
	while(1)
	{
//		SBUF = 0x55;
//		while(!UartSendFlag);
//		UartSendFlag = 0;
		
		if(UartReceiveFlag)
		{
			UartReceiveFlag=0;
			
			P00 = ~P00;
		
			//HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少	
			if(SBUF == 0x01)
			{
//				Scr_Driver_PWM_Adjust(1);		
				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}
			if(SBUF == 0x02)
			{
//				Scr_Driver_PWM_Adjust(2);
				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}	
		}
	}
}

void Uart_Process()
{
	if(UartReceiveFlag)
		{
			UartReceiveFlag=0;
			
			P00 = ~P00;
		
			//HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少	
			if(SBUF == 0x01)
			{
				//Scr_Driver_PWM_Adjust(1);	
				
				time2_curr=1;

//				time2_curr++;
//				if(time2_curr>time2_count_max)
//				{
//					time2_curr=time2_count_max;
//				}
				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}
			if(SBUF == 0x02)
			{
				//Scr_Driver_PWM_Adjust(2);
				
				time2_curr=0;
				
//				time2_curr--;
//				if(time2_curr<0)
//				{
//					time2_curr=0;
//				}
				
				SBUF = 0x55+SBUF;
				while(!UartSendFlag);
				UartSendFlag = 0;
			}	
		}
}

/*****************************************************
*函数名称：void Uart0_Init(void)
*函数功能：Uart0中断初始化
*入口参数：void 
*出口参数：void
*****************************************************/
void Uart0_Init(void)    //选择Timer1作为波特率信号发生器
{
	P1CON &= 0XF3;
	P1PH |= 0X0C;	 //TX/RX为带上拉输入；
	P13 = 1;		 //TX初始高电平；
	SCON = 0X50;     //方式1，允许接收数据
	PCON |= 0X80; 
	T2CON = 0x00;    //使用定时器1作UART时钟
	TMOD = 0X20;     //定时器1  8位自动重载
	TMCON = 0X02;    //定时器1   Fsys；
	TL1 = 217;		
	TH1 = 217;		 //UART 波特率24M情况下=38400；
	TR1 = 0;
	ET1 = 0;		 //Timer1使能
	TR1 = 1;		 //启动Timer0
	EUART = 1;	     //允许UART中断
	EA = 1;		     //开总中断
}

/*
void Uart0_Init(void)    //选择Timer2作为波特率信号发生器
{
	P1CON &= 0XF3;
	P1PH |= 0X0C;	 //TX为强推输出，RX为带上拉输入；
	SCON  = 0X50;    //设置通信方式为模式一，允许接收
	PCON |= 0X80; 
	TMCON |= 0X04;
	T2MOD = 0X00;
	T2CON = 0X30;
	RCAP2H = 0xFF;
	RCAP2L = 0xB2;
	TR2 = 0;
	ET2 = 0;
	TR2 = 1;
	EUART = 1;     //开启Uart0中断
	EA = 1;
}
*/

/*****************************************************
*函数名称：void UartInt(void) interrupt 4
*函数功能：Uart0中断服务函数
*入口参数：void
*出口参数：void
*****************************************************/
void UartInt_Handle()
{
	if(TI)
	{
		TI = 0;	
		UartSendFlag = 1;		
	}
	if(RI)
	{
		RI = 0;	
		UartReceiveFlag = 1;		
	}	
}
