#include "H/Function_Init.H"

uint time2_count=0;
uint time2_count_max=43;// 25 200us
int time2_curr=25;

uint scr_open_time_max=20000;
uint scr_open_time=19909;//17200;//20000;//5;//低电平 8.6ms 17200---0  高电平 10ms  20000---0
bit scr_open_flag=0;//可控硅开通标志 用于关断定时器 关断可控硅
uint scr_adjust_step=2; //1us
uint scr_curr_time=19909;//20000;//6;

void Timer_Init(void);
/*****************************************************
*函数名称：void Timer_Test(void);
*函数功能：T0/T1/T2测试
*入口参数：void
*出口参数：void
*****************************************************/
void Timer_Test(void)
{
    Timer_Init();
    while(1)
    {
    }
}
/*****************************************************
*函数名称：void Timer_Init(void)
*函数功能：T0/T1/T2初始化
*入口参数：void
*出口参数：void
*****************************************************/

/*
void Timer_Init(void)
{
    TMCON = TMCON|0X04;    //------100 ;Tiemr2选择时钟Fsys
		//TMCON &= 0Xfb;    //------000 ;Tiemr2选择时钟Fsys/12

    //T2设置
    T2MOD = 0x00;

    //T2CON = 0x00;	 //设置为16位重载寄存器
    T2CON &= 0x30;	 //设置为16位重载寄存器

//	RCAP2H = (65536-48000)/256;     //溢出时间：时钟为Fsys，则48000*（1/Fsys）=2ms;
//	RCAP2L = (65536-48000)%256;
//	RCAP2H = (65536-24000)/256;     //溢出时间：时钟为Fsys，则24000*（1/Fsys）=1ms;
//	RCAP2L = (65536-24000)%256;
//	RCAP2H = (65536-2400)/256;     //溢出时间：时钟为Fsys，则2400*（1/Fsys）=100us;
//	RCAP2L = (65536-2400)%256;
    RCAP2H = (65536-4800)/256;     //溢出时间：时钟为Fsys，则4800*（1/Fsys）=200us;
    RCAP2L = (65536-4800)%256;

    TR2 = 0;
    ET2 = 1;//定时器2允许
    TR2 = 1;//打开定时器2

//	HEAT_TRA=1;

		//设置最大值
		time2_count_max=open_max-time2_curr/4;

    time2_count=0;
    if(time2_curr==0)
    {
        HEAT_TRA=1;
    }
    else
    {
        HEAT_TRA=0;
    }

    EA = 1;
}

void Timer2Int_Handle()
{
    TF2 = 0;   //溢出清零

    //串口打印log，调试。。。 ---->>>>> 绝对不能开启，耗时太长，一定会导致问题
    //UART_SentChar(time2_curr);

    time2_count=time2_count+1;
    if(time2_curr == 0)
    {
    }
    else
    {
        if(time2_count<time2_curr)
        {
            if(HEAT_TRA!=0)
            {
                HEAT_TRA=0;
            }
        }
        else if(time2_count>=time2_curr)
        {
            if(HEAT_TRA!=1)
            {
                HEAT_TRA=1;
            }
        }
								
				if(time2_count>=time2_count_max)
				{
						if(HEAT_TRA!=0)
						{
								HEAT_TRA=0;
						}
						if(TR2!=0)
						{
								TR2 = 0;
						}
				}
    }
}

*/


//void Timer_Init(void)
//{
//    //TMCON = TMCON|0X04;    //------100 ;Tiemr2选择时钟Fsys
//		TMCON &= 0Xfb;    //------000 ;Tiemr2选择时钟Fsys/12

//    //T2设置
//    T2MOD = 0x00;

//    //T2CON = 0x00;	 //设置为16位重载寄存器
//    T2CON &= 0x30;	 //设置为16位重载寄存器
//	
//		
//		if(scr_open_time != scr_curr_time)
//		{
//			scr_open_time=scr_curr_time;
//		}
//	
//		//可控硅开通时间点
//		RCAP2H = (65536-scr_open_time)/256;     //溢出时间：时钟为Fsys，则scr_open_time*（1/(Fsys/12)）=scr_open_time*0.5us;
//		RCAP2L = (65536-scr_open_time)%256;		
//		
//    TR2 = 0;
//    ET2 = 1;//定时器2允许
//    TR2 = 1;//打开定时器2
//		
////	HEAT_TRA=1;		
//		
//		//关闭可控硅 设置可控硅开通标记
//		HEAT_TRA=0;
//		scr_open_flag=0;

//    EA = 1;
//}

//void Timer2Int_Handle()
//{
//    TF2 = 0;   //溢出清零
//	
//		if(scr_open_flag==0)
//		{
//			if(HEAT_TRA!=1)
//			{
//					HEAT_TRA=1;
//			}			
//			scr_open_flag=1;			
//			
//			
//			TR2 = 0;
//			ET2 = 0;
//			//可控硅开通时间点之后，计算关断可控硅和定时器时间，然后重置定时器
//			RCAP2H = (65536-(scr_open_time_max-scr_open_time))/256;     //溢出时间：时钟为Fsys，则scr_open_time*（1/(Fsys/12)）=scr_open_time*0.5us;
//			RCAP2L = (65536-(scr_open_time_max-scr_open_time))%256;			
//			ET2 = 1;//定时器2允许
//			TR2 = 1;//打开定时器2
//		}
//		else
//		{
//			if(HEAT_TRA!=0)
//			{
//					HEAT_TRA=0;
//			}
//			if(TR2!=0)
//			{
//					TR2 = 0;
//			}			
//			scr_open_flag=0;
//		}
//}


void Timer_Init(void)
{
	TMCON &= 0xfd;  
	TMOD &=0x9f;
	
	if(scr_open_time != scr_curr_time)
		{
			scr_open_time=scr_curr_time;
		}
	
		
		//关闭可控硅 设置可控硅开通标记
		HEAT_TRA=0;
		scr_open_flag=0;
		
	TL1 = (65536 - scr_open_time)%256;     //溢出时间：时钟为Fsys，则scr_open_time*（1/(Fsys/12)）=scr_open_time*0.5us;
	TH1 = (65536 - scr_open_time)/256;
	TR1 = 0;
	ET1 = 1;//定时器0允许
	TR1 = 1;//打开定时器0
	
	
     EA = 1;
}

void Timer1Int_Handle()
{
		if(scr_open_flag==0)
		{
			if(HEAT_TRA!=1)
			{
					HEAT_TRA=1;
			}			
			scr_open_flag=1;			
			
			
//			TR1 = 0;
//			ET1 = 0;
			//可控硅开通时间点之后，计算关断可控硅和定时器时间，然后重置定时器
			TH1 = (65536-(scr_open_time_max-scr_open_time))/256;     //溢出时间：时钟为Fsys，则scr_open_time*（1/(Fsys/12)）=scr_open_time*0.5us;
			TL1 = (65536-(scr_open_time_max-scr_open_time))%256;			
//			ET1 = 1;//定时器2允许
//			TR1 = 1;//打开定时器2
		}
		else
		{
			if(HEAT_TRA!=0)
			{
					HEAT_TRA=0;
			}
			if(TR2!=0)
			{
				//ET1 = 0;
					TR2 = 0;
			}			
			scr_open_flag=0;
		}
}

