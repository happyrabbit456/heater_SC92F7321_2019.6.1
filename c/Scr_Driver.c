//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"
#include <stdio.h>

/******************
	*以下四项是需要根据实际情况调试的
	******************/
	//15.6msMS计算一次 计算周期
#define pidt   0.5
//比例系数  0.01 --- 10  采样频率低（如500ms），Kp一般是0.01级别；采样频率高（如1ms），Kp一般是1级别
//#define  Kp   (5/10)    //一定不能这样用，keil 不支持
//积分时间
#define  Ti  5000
//微分时间
#define  Td 600 

//55*10-28*10=270
#define Upper_Limit  270
//如果出水温度超过预设温度，可控硅无功率运行
#define Lower_Limit  -1


//INT24 P20 ZERO

//HEAT TRA PWM1 P01

//HEAT RLY 继电器控制 P02

//T/S1 温度保险 HEAT ERROR COM3/P03
//如果检测到温度过高，T/S1会自动跳闸，断开电路 
//继电器停止工作 保护整个可控硅驱动电路
//HEAT ERROR 为输入端，如果等于高电平，表明热水器温度过高
//如果为低电平，表明热水器温度在正常范围内

volatile uchar heater_power_status=0; // 0:无功率 1：全功率

//当前热水器运行或停止状态 控制继电器动作 0：停止 1：运行
 volatile bit heater_relay_on=0;

volatile bit b_start_pid=0;

////热水器内部异常状态
Enum_Ex_Flag idata Ex_Flag;

//35度~60度 自动调节  最佳：40 - 50
int idata best_temp_out=37;
volatile uchar  current_out_temp=28; //当前出水温度

volatile int  scr_curr_time=0;//zero_period_high_time/2;//20000;//6;



//int idata Out1=0;  //记录上次输出

//int idata ERR=0;       //当前误差
//int idata ERR1=0;      //上次误差
//int  idata ERR2=0;      //上上次误差


//void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//软件延时
void soft_delay(uint n);

// Sv设定温度值  Pv当前温度值
void PIDCalc(int Sv,int Pv);

int Scr_Driver_Check_Insurance();//检测温度保险
void Scr_Driver_Control_Heat_RLY(int on);//继电器控制 HEAT RLY P02

/*****************************************************
*函数名称：void Zero_Crossing_EX_Init(void)
*函数功能：外部中断初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Zero_Crossing_EX_Init(void)
{    
    P2CON &= 0XFE;     //中断IO口设置为高阻输入
    //P2PH  |= 0x01;     //中断IO口设置为高阻带上拉
		P2PH &= 0xfe; //不带上拉 外部有上拉电阻

    //配置中断口INT24  上升沿和下降沿都支持
    //下降沿设置
    INT2F |= 0x10;  //0000 xxxx  0关闭 1使能 打开下降沿
		//INT2F &= 0x2F; //关闭下降沿
    //上升沿设置
    INT2R |= 0X10 ;    //0000 xxxx  0关闭 1使能

    //外部中断优先级设�
    IE1 |= 0x08;	//0000 x000  INT2使能
    IP1 |= 0X00;
//    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
	
    //如果中断2有两路输入，根据上升沿或者下降沿来确认，上升沿中断，所以端口电平是1
    //if(ZERO == 1) //INT24 P20 ZERO 过零检测到零点
    {			
			//全功率
//			HEAT_TRA=1;
			
//			//过零检测中断，可控硅关闭
//			if(HEAT_TRA!=0)
//				HEAT_TRA=0;
			
			//定时器关闭
			//TR1 = 0;
			
			if(heater_power_status==1)
			{
				//scr_curr_time=0;
				
				//全功率
				if(HEAT_TRA!=1)
					HEAT_TRA=1;
				
				//定时器关闭
				if(TR1!=0)
					TR1 = 0;
			}			
			else if(heater_power_status==0)
			{				
				//无功率
				if(HEAT_TRA!=0)
					HEAT_TRA=0;
		
				//定时器关闭
				if(TR1!=0)
					TR1 = 0;
			}
			else
			{
				if(ZERO==1)
				{
					scr_open_time_max=zero_period_high_time;
				}
				else
				{
					scr_open_time_max=zero_period_low_time;
				}	
				
				//scr_open_time = scr_open_time_max/2;//37度

				if(scr_curr_time>0 && scr_curr_time<=(scr_open_time_max-zero_peroid_last_time))
				{
					if(HEAT_TRA!=1)
					{
							HEAT_TRA=1;
					}			
			
					if(scr_open_time != scr_curr_time)
					{
						scr_open_time=scr_curr_time;
					}
		
					Timer_Init();
				}
				else
				{
					if(HEAT_TRA!=0)
						HEAT_TRA=0;
					
					//定时器关闭
					if(TR1!=0)
						TR1 = 0;
				}				
			}
		}
		
 /*   if(HALL_LLJ == 1) //INT25 P21 水流检测计数
    {

    }
	*/
}

//检测温度保险 HEAT ERROR 直接检测端口值 P03   轮询方式
int Scr_Driver_Check_Insurance()
{	
	if(HEAT_ERROR==0)
	{
		//温度正常范围内，温度保险不跳闸
		return 0;
	}
	else if(HEAT_ERROR==1)
	{
		//温度异常范围内，温度保险已跳闸
		if(heater_relay_on==1)
		{
				heater_relay_on=0;
				Scr_Driver_Control_Heat_RLY(heater_relay_on);
		}		
		return -1;
	}
	return -1;
}

//继电器控制 HEAT RLY P02
void Scr_Driver_Control_Heat_RLY(int on)
{
	P0VO = P0VO&0xfb; //P02端口设置成普通I/O口  1111 1011
	if(on == 1)
	{
		HEAT_RLY=1;
	}
	else
	{
		HEAT_RLY=0;
	}
	
	//软件延时，保证heater_relay_on变量更新完成，避免主循环逻辑错误或者混乱
	soft_delay(48000); // (1+1+（1+2）*48000)*0.5us=72001us=72.001ms
}


//软件延时 https://blog.csdn.net/nanfeibuyi/article/details/83577641 
/*

根据循环条件，INC指令和CJNE指令总共要执行n次,共消耗机器周期（1+2）*n，

加上CLR指令和MOV指令，循环程序总共消耗机器周期：1+1+（1+2）*n

如果单片机的晶振频率为24MHz，则机器周期 = 12*（1/24）us = 0.5us。

那么for循环程序耗时为(1+1+（1+2）*n)*0.5us。
*/
void soft_delay(uint n)
{
	uint k;
	for(k=0;k<n;k++)
	{
		_nop_();  //代表运行一个机器周期 如果这个单片机的晶振是12M的，那么这调代码会运行1US
//		;
	}
}

// Sv设定温度值  Pv当前温度值
void PIDCalc(int Sv,int Pv)
{ 	
	//温度值乘10做处理
	int target_temp=Sv*10;
	int curr_temp=Pv*10;
	
	//int Out=target_temp-curr_temp;
			
	int DERR1 = 0;       //
	int DERR2 = 0;       //

	int Pout = 0;       //比例结果
	int Iout = 0;       //积分结果
	int Dout = 0;       //微分结果
	int Out = 0; //总输出
	
	
	
	
	static int Out1=0;  //记录上次输出

	static int ERR=0;       //当前误差
	static int ERR1=0;      //上次误差
	static int ERR2=0;      //上上次误差

	
//	static uint Upper_Limit= 100; //PID输出上限
//	static uint Lower_Limit= 0; //PID输出下限
	
	
	 ERR = target_temp - curr_temp;   //算出当前误差
	DERR1 = ERR - ERR1;   //上次
	
	//DERR2 = ERR - 2*ERR1 + ERR2; //上上次  //不要在主程序和中断程序中同时做8bit以上的乘除法运算，会出错
	DERR2= ERR  + ERR2;
	DERR2= DERR2 - ERR1;
	DERR2= DERR2 - ERR1;
	
	//先Kp
	Pout = DERR1/2;//DERR1*Kp;    //输出P
	Iout = 0;//(float)(ERR * ((Kp * pidt) / Ti));  //输出I
	Dout = 0;//(float)(DERR2 * ((Kp * Td) / pidt));   //输出D
	//Out = (int)(Out1 + Pout + Iout + Dout);
	Out = Out1+ Pout;
	Out = Out+ Iout;
	Out = Out+ Dout;
	
	if(Out >= Upper_Limit) { //如果输出大于等于上限
		Out = Upper_Limit;
	} 
	else if(Out <= Lower_Limit) { //如果输出小于等于下线
		Out = Lower_Limit;
	}
	
	Out1 = Out;      //记录这次输出的值

	ERR2 = ERR1;    //记录误差
	ERR1 = ERR;     //记录误差
	
	/**/
	
	Out=Out/10;
	
	printf("%d\n",Out);
			
	if(Out>0)
	{
		//printf("111\n");
		
		//偏差大于2度为上限幅值输出(全速加热)
		if(best_temp_out-current_out_temp>20)//温度偏差大于2?
		{
//			if(scr_curr_time!=0)
//			{
//				scr_curr_time=0;				
//			}
			if(heater_power_status!=1)
					heater_power_status=1;//
		}
		else
		{
			//PID算法控制
			if(b_start_pid==0)
			{
				b_start_pid=1;
				
				//全功率调整90% 功率调节是相反的 (100-90)/100=1/10
				scr_curr_time = zero_period_low_time/10; //17200 //不能这样用，可以给固定值
			}
			else
			{
				//20000/270=74
		
				//一定要相减，因为功率调节是相反的，scr_curr_time越小，功率越大
				scr_curr_time = scr_curr_time - Out*74;  //Out=50 Out*74=3700
				if(scr_curr_time<1)
				{
					if(scr_curr_time!=0)
					{
						scr_curr_time=0;				
					}
				}
			}
		}
	}
	else if(Out<0)
	{
		//UART_SentChar(0x57);
		
		//printf("222\n");
		
//		if(HEAT_TRA!=0)
//			HEAT_TRA=0;
//		
//		//定时器关闭
//		if(TR1!=0)
//			TR1 = 0;
		
		if(heater_power_status!=0)
			heater_power_status=0;//scr_curr_time=zero_period_high_time;//scr_open_time_max-zero_peroid_last_time;
	}
	else
	{
		//printf("333\n");
	}
	
	printf("%d\n",scr_curr_time);
}
