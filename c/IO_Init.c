#include "H/Function_Init.H"
/*****************************************************
*函数名称：void IO_Init(void)
*函数功能：IO初始化
*入口参数：void
*出口参数：void
*****************************************************/
void IO_Init(void)
{
	#if (IC_MODEL == SC92F7323)  //SC92F7323引脚定义
	P0CON = 0x00;  //设置P0为高阻带上拉模式
	P0PH  = 0xFF;
	P1CON = 0x00;  //设置P1为高阻输入模式
	P1PH  = 0x00;
	P2CON = 0xFF;  //设置P2为强推挽模式
	P2PH  = 0x00;
	P5CON = 0xFF;  //设置P5为强推挽模式
	P5PH  = 0x00;
	#endif
	
	#if (IC_MODEL == SC92F7322)  //SC92F7323引脚定义
	P0CON = 0x00;  //设置P0为高阻带上拉模式
	P0PH  = 0xFF;
	P1CON = 0x00;  //设置P1为高阻输入模式
	P1PH  = 0x00;
	P2CON = 0xFF;  //设置P2为强推挽模式
	P2PH  = 0x00;
	SC92F7322_NIO_Init(); //未引出IO口配置
	#endif
	
	#if (IC_MODEL == SC92F7321)  //SC92F7321引脚定义
	P0CON = 0x00;  //设置P0为高阻带上拉模式
	P0PH  = 0xFF;
	P1CON = 0x00;  //设置P1为高阻输入模式
	P1PH  = 0x00;
	P2CON = 0xFF;  //设置P2为强推挽模式
	P2PH  = 0x00;
	SC92F7321_NIO_Init(); //未引出IO口配置
	
	
	// 初始化 继电器关闭，HEAT RLY P02，热水器停止状态
	P0CON = P0CON|0x04;  //设置P02为强推挽模式
	P0VO = P0VO&0xfb; //P02端口设置成普通I/O口  1111 1011
	P02=0;
	
	//过零检测端口配置
	P2CON &= 0XFE;     //中断IO口设置为高阻输入
	P2PH  |= 0x01;     //中断IO口设置为高阻带上拉

	//配置中断口INT24  上升沿和下降沿都支持
	//下降沿设置
	INT2F |= 0x10;  //0000 xxxx  0关闭 1使能 打开下降沿
	//INT2F &= 0x2F; //关闭下降沿
	//上升沿设置
	INT2R |= 0X10 ;    //0000 xxxx  0关闭 1使能
	
	//P00 Beep，发光二极管，调试用
	P0CON= P0CON|0x01;
	P0VO = P0VO&0xfe; //P00端口设置成普通I/O口  1111 1110
	P00=1;
	
	//p01做为IO口控制可控硅，用定时器2计时
	P0CON= P0CON|0x02;
	P0VO = P0VO&0xfd; //P00端口设置成普通I/O口  1111 1101
	P01=0;
	
	
//	//检测温度保险 HEAT ERROR 直接检测端口值 P03   轮询方式
//	P0CON = P0CON&0xf7; //设置P03为输入模式
//	P0PH  = P0PH&0xf7;	//高阻输入模式，上拉电阻关闭
//	P0VO = P0VO&0xf7; //P03端口设置成普通I/O口 轮询方式
	#endif
	
	#if (IC_MODEL == SC92F7320)  //SC92F7320引脚定义
	P1CON = 0x00;  //设置P1为高阻输入模式
	P1PH  = 0x00;
	P2CON = 0xFF;  //设置P2为强推挽模式
	P2PH  = 0x00;
	SC92F7320_NIO_Init(); //未引出IO口配置
	#endif
}