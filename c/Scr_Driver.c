//Zero Crossing detection
//SCR Driver

#include "H/Function_Init.H"

//INT24 P20

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
int good_temp_out_low=40;
int good_temp_out_high=50;
int current_out_temp=26; //当前出水温度

void Zero_Crossing_EXTI_Test(void);
void Zero_Crossing_EX_Init(void);
void Zero_Crossing_EX2_Handle();


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
*函数名称：void EX_Init(void)
*函数功能：外部中断初始化
*入口参数：void
*出口参数：void
*****************************************************/
void Zero_Crossing_EX_Init(void)
{
    //配置中断口INT24
    P2CON &= 0XFE;     //中断IO口设置为高阻输入
    P2PH  |= 0x01;     //中断IO口设置为高阻带上拉

    //INT24上升中断
    //下降沿设置
    INT2F &= 0x2F; //= 0X00 ;    //0000 xxxx  0关闭 1使能
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
    //if(P20 == 1) //INT24 P20 过零检测到零点
    {
        //PWM计数值重置
//        Scr_Driver_PWM_Init();
			
			Timer_Init();
    }
 /*   if(P21 == 1) //INT25 P21 水流检测计数
    {

    }
	*/
}

//检测温度保险 HEAT ERROR 直接检测端口值 P03   轮询方式
int Scr_Driver_Check_Insurance()
{	
	if(P03==0)
	{
		//温度正常范围内，温度保险不跳闸
		return 0;
	}
	else if(P03==1)
	{
		//温度异常范围内，温度保险已跳闸
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
		P02=1;
	}
	else
	{
		P02=0;
	}
}
