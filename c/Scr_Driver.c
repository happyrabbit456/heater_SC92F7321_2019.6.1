//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"

//INT24 P20 ZERO

//HEAT TRA PWM1 P01

//HEAT RLY 继电器控制 P02

//T/S1 温度保险 HEAT ERROR COM3/P03
//如果检测到温度过高，T/S1会自动跳闸，断开电路 
//继电器停止工作 保护整个可控硅驱动电路
//HEAT ERROR 为输入端，如果等于高电平，表明热水器温度过高
//如果为低电平，表明热水器温度在正常范围内

//当前热水器运行或停止状态 控制继电器动作 0：停止 1：运行
bit heater_relay_on=0;

////热水器内部异常状态
Enum_Ex_Flag Ex_Flag;

//35度~60度 自动调节  最佳：40 - 50
int good_temp_out_low=28;
int good_temp_out_high=60;
int best_temp_out=37;
int current_out_temp=28; //当前出水温度

void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();

//HEAT TRA  功率调节方式 flag 0:不用调节 1：增加功率 2：减少功率
void Scr_Driver_power_Adjust(uint flag);

//软件延时
void soft_delay(uint n);
void delay_1ms(uint x);
void delay(uint n);

int Scr_Driver_Check_Insurance();//检测温度保险
void Scr_Driver_Control_Heat_RLY(int on);//继电器控制 HEAT RLY P02

//HEAT TRA PWM1 功率调节方式 flag 0:不用调节 1：增加功率 Duty增大 2：减少功率 Duty减少
void Scr_Driver_PWM_Adjust(uint flag);

uchar Zero_Crossing_INT1_flag = 0x00;
/*****************************************************
*函数名称：void EXTI_Test(void)
*函数功能：外部中断测试
*入口参数：void
*出口参数：void
*****************************************************/
void Zero_Crossing_EXTI_Test(void)
{
    Zero_Crossing_EX_Init();
    while(1)
    {
    }
}
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
    EA = 1;
}

void Zero_Crossing_EX2_Handle()
{
	
    //如果中断2有两路输入，根据上升沿或者下降沿来确认，上升沿中断，所以端口电平是1
    //if(ZERO == 1) //INT24 P20 ZERO 过零检测到零点
    {			
			//全功率
//			HEAT_TRA=1;
			
//			//过零检测中断，可控硅关闭
			HEAT_TRA=0;
			
			//定时器关闭
			TR1 = 0;
			
			if(ZERO==1)
			{
				scr_open_time_max=zero_period_high_time;
			}
			else
			{
				scr_open_time_max=zero_period_low_time;
			}			
			
//		 scr_open_time=scr_open_time_max-zero_peroid_last_time;//17200;//20000;//5;//低电平 8.6ms 17200---0  高电平 10ms  20000---0
//		 scr_curr_time=scr_open_time_max-zero_peroid_last_time;//20000;//6;
			
			scr_open_time=0;//8600;//17200;//20000;//5;//低电平 8.6ms 17200---0  高电平 10ms  20000---0
			scr_curr_time=0;//8600;//20000;//6;
			
			//开头
			if(scr_curr_time==0)
			{
				//全功率
				HEAT_TRA=1;
			}
			//末尾
			else if(scr_curr_time>0 && scr_curr_time<=(scr_open_time_max-zero_peroid_last_time))
			{
				Timer_Init();
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

//HEAT TRA  功率调节方式 flag 0:不用调节 1：增加功率 2：减少功率
void Scr_Driver_power_Adjust(uint flag)
{	
	if(flag==1 || flag==2)
	{		
		if(flag==1){ //增加功率
			scr_curr_time -= scr_adjust_step;
			if(scr_curr_time<1)
			{
				scr_curr_time=0;
			}
		}
		else if(flag==2) //减少功率
		{
			scr_curr_time += scr_adjust_step;
			if(scr_curr_time>=scr_open_time_max/scr_adjust_step)
			{
				scr_curr_time=scr_open_time_max;
			}
		}		

		//串口打印log，调试。。。
		//UART_SentChar(scr_curr_time);
	}
	
}

//软件延时 https://blog.csdn.net/nanfeibuyi/article/details/83577641 
/*

 CLR指令消耗1个机器周期

 MOV指令消耗1个机器周期

 INC指令消耗1个机器周期                                      

 CJNE指令消耗2个机器周期

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

//https://blog.csdn.net/nanfeibuyi/article/details/83577641 
//函数名：delay_1ms(uint x)
//功能：利用定时器0精确定时1ms
//调用函数：
//输入参数：x,1ms计数
//输出参数：
//说明：延时的时间为1ms乘以x
//延时1s：delay_1ms(1000); //1000ms = 1s 
void delay_1ms(uint x)
{
	TMOD=0X01;//开定时器0，工作方式为1
	TR0=1;//启动定时器0；
	while(x--)
	{
		TH0=0Xfc;//定时1ms初值的高8位装入TH0
		TL0=0X18;//定时1ms初值的低8位装入TL0
		while(!TF0);//等待，直到TF0为1
		TF0=0;	   //重置溢出位标志
	}		
	TR0=0;//停止定时器0；
}

////https://blog.csdn.net/nanfeibuyi/article/details/83577641 
//如果单片机晶振频率24MHz，机器周期=12*(1/24)us=0.5us
//循环耗时为 ((1+1+600+1+2)*n+1+1)*0.5
void delay(uint n)//延时函数
{
	uint i, j;
	for(i=0; i<n; i++) //执行n次 (1+1+600+1+2)*n
		for(j=0; j<200; j++) //（1+2）*200=600周期
			;
}


