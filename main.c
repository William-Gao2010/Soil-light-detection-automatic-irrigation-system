#include <STC8G.H>
#include "intrins.h"

sbit RS = P3^2;
sbit RW	= P3^3;
sbit E  = P3^4;
sbit DJ = P3^7;

int NumberOfTimes;
unsigned char result;

unsigned char count = 0; 
unsigned char DJ_ds = 0;

xdata unsigned char Welcomeline1[]="Welcome to use  ";
xdata unsigned char Welcomeline2[]="V1.0.0 Gaohaojun";

xdata unsigned char Homeline1[]="Work:          0";
xdata unsigned char Homeline2[]="Light:    Common";

xdata unsigned char DJline1[]="Work:     (SG90)";
xdata unsigned char DJline2[]="Degree:       90";

xdata unsigned char ScuessLine1[]="WorkSucessly    ";
xdata unsigned char ScuessLine2[]="Fail:          0";

void Uart1_Init(void)	//9600bps@33.1776MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xA0;			//设置定时初始值
	TH1 = 0xFC;			//设置定时初始值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时
	EA = 1;
	ES = 1;
}


unsigned char  degreecount(unsigned char ds)
{
    unsigned char ds_temp = 0;
    ds_temp = ds/9 + 5;

    return ds_temp;
}

void pmw_degree(unsigned char degree)
{
	DJ_ds = degreecount(degree);
}

void Delay5ms(void)	//@33.1776MHz
{
	unsigned char data i, j;

	_nop_();
	_nop_();
	i = 216;
	j = 109;
	do
	{
		while (--j);
	} while (--i);
}


void Timer0_Init(void)        //100??@33.1776MHz
{
    EA = 1;                    //?????
    ET0 = 0;                //?????0??
    AUXR |= 0x80;            //?????1T??
    TMOD &= 0xF0;            //???????
    TL0 = 0x0A;                //???????
    TH0 = 0xF3;                //???????
    TF0 = 0;                //??TF0??
    TR0 = 1;                //???0????
}

void write_introduce(unsigned char pin)
{
	RS = RW = 0; E = 1; 
	P1 = pin; Delay5ms();
	E = 0;
}

void write_data(unsigned char byte)
{
	RW = 0; RS = E = 1; 
	P1 = byte; Delay5ms();
	E = 0;
}

void LCDstart()
{
	write_introduce(0x38);  //gn set
	write_introduce(0x0c);  //lightdesign back home
	write_introduce(0x06);  //disp key
	write_introduce(0x01);  //clear display
}

void Delay1ms(unsigned int ms)	reentrant  //@33.1776MHz
{
	while(ms--)
	{	
	unsigned char data i, j;

	_nop_();
	_nop_();
	i = 44;
	j = 19;
	do
	{
		while (--j);
	} while (--i);
}
}

void Delay1us(unsigned int us)	//@33.1776MHz
{
	while(us--)
	{
	unsigned char data i;

	i = 9;
	while (--i);
	}
}


void Urtlstart()
{
		REN=1;
    Delay1ms(100);
    REN=0;
}

void SendData(int Data)
{
	Urtlstart();
	SBUF = Data;
	Delay1ms(10);
}

void conversion()
{
    result = ((ADC_RES & 0x03) << 8) | ADC_RESL;
}


void CheckLight()
{

			ADC_CONTR	=	0xCE;								
			ADCCFG	=	0x2F;													
			ADCTIM	=	0x2A;													
			Delay1ms(1);
			ADC_CONTR &= 0x20;
			conversion();
			SendData(result);

}

void Welcome()
{
	unsigned char i = 0;
	write_introduce(0x80);
	for(i=0;i<16;i++)
	{
		write_data(Welcomeline1[i]);
		Delay1us(5);
	}
	write_introduce(0xc0);
	for(i=0;i<16;i++)
	{
		write_data(Welcomeline2[i]);
		Delay1us(5);
	}
	Delay1ms(1000);
}



void DJWork()
{
	unsigned char i = 0;
	write_introduce(0x80);
	for(i=0;i<16;i++)
	{
		write_data(DJline1[i]);
		Delay1us(5);
	}
	write_introduce(0xc0);
	for(i=0;i<16;i++)
	{
		write_data(DJline2[i]);
		Delay1us(5);
	}
	
	ET0 = 1;
	pmw_degree(0);
	Delay1ms(500);	
	pmw_degree(90);
	Delay1ms(500);	
	pmw_degree(0);
	Delay1ms(500);	
	ET0 = 0;
	
	for(i=0;i<16;i++)
	{
		write_data(ScuessLine1[i]);
		Delay1us(5);
	}
	write_introduce(0xc0);
	for(i=0;i<16;i++)
	{
		write_data(ScuessLine2[i]);
		Delay1us(5);
	}
	Delay1ms(1000*3);
}

void change()
{
	if(SBUF == 0xCD)
	{
		SendData(0xFE);
	}
}

void Home()
{
	unsigned char i = 0;
	write_introduce(0x80);
	for(i=0;i<16;i++)
	{
		write_data(Homeline1[i]);
		Delay1us(5);
	}
	write_introduce(0xc0);
	for(i=0;i<16;i++)
	{
		write_data(Homeline2[i]);
		Delay1us(5);
	}
	change();
	CheckLight();
	if(result<0X05)
	{
		DJWork();
	}
}


void main()
{
	P1M0 = 0x00; P1M1 = 0x00;
	P3M0 = 0x00; P3M1 = 0x00;
	P5M0 = 0x00; P5M1 = 0x00;
	Timer0_Init();
	LCDstart();
	Welcome();
	Uart1_Init();
	ET0 = 1;
	while(1)
	{
		
		Home();
		
	}
	
}


void Timer0() interrupt 1
{
    TL0 = 0x0A;                //???????
    TH0 = 0xF3;                //???????
    
		if (DJ_ds > count){DJ = 1;}else{DJ = 0;}     
    count++;
    count %= 200;
}

void Uart1() interrupt 4
{

	if(TI){TI=0;}
	if(RI)
	{		
		
		RI=0;
	}
	
}