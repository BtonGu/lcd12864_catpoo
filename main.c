#include <reg51.h>

#define LCDLCDDisp_Off   0x3e
#define LCDLCDDisp_On    0x3f
#define Page_Add     0xb8//页地址
#define LCDCol_Add     0x40//列地址
#define Start_Line     0xC0//行地址
/*****液晶显示器的端口定义*****/
#define data_ora P0 /*液晶数据总线*/
sbit LCDMcs=P2^4 ;    /*片选1*/
sbit LCDScs=P2^3 ;    /*片选2*/
sbit RESET=P2^3 ;  /*复位信号*/
sbit LCDDi=P2^2 ;     /*数据/指令 选择*/
sbit LCDRW=P2^1 ;     /*读/写 选择*/
sbit LCDEnable=P2^0 ;     /*读/写 使能*/



char code Hz_Deng[]= 
{
0xE0,0xF0,0xDF,0x7E,0x88,0x08,0x78,0xD8,0xFE,0xFF,0xE0,0x00,0x00,0x00,0x40,0x80,
0x03,0x04,0xC8,0x39,0x09,0x09,0x08,0x08,0xF8,0xF5,0xF3,0xF0,0xE0,0xE0,0xF0,0xCF
};

char code Bmp_Miao[]= 
{
/*------------------------------------------------------------------------------
  源文件 / 文字 : F:\LD测试仪\液晶\字模提取+排版软件\液晶字模提取\图例\ping.bmp萍
  宽×高（像素）: 16×16
  字模格式/大小 : 单色点阵液晶字模，纵向取模，字节倒序/32字节
  数据转换日期  : 2010-7-29 8:46:30
------------------------------------------------------------------------------*/
0x00,0x00,0x88,0x10,0x00,0xF0,0x90,0x90,0x90,0xFC,0x90,0x90,0x30,0x00,0x00,0x00,
0x00,0x00,0x10,0x09,0x20,0x1F,0x20,0x23,0x14,0x08,0x14,0x23,0x20,0x00,0x00,0x00
};

char code Bmp_bo[]= 
{
0x00,0x80,0x00,0x20,0xE0,0x00,0x08,0x30,0xC0,0x00,0x02,0x0C,0x70,0x80,0x00,0x00,
0x00,0x00,0x00,0x02,0x03,0x00,0x10,0x0C,0x03,0x00,0x40,0x30,0x0E,0x01,0x00,0x00
};

/****************************************************************************
函数功能:LCD延时程序
入口参数:t
出口参数:
****************************************************************************/
void LCDdelay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<t;i++);
	for(j=0;j<10;j++);
}
/****************************************************************************
状态检查，LCD是否忙


*****************************************************************************/
void CheckState()		
{
   unsigned char dat,DATA;//状态信息（判断是否忙）
   LCDDi=0; // 数据\指令选择，D/I（RS）="L" ，表示 DB7∽DB0 为显示指令数据 
   LCDRW=1; //R/W="H" ，E="H"数据被读到DB7∽DB0 
   do
   {
      DATA=0x00;
      LCDEnable=1;	//EN下降源
	  LCDdelay(2);//延时
	  dat=DATA;
      LCDEnable=0;
      dat=0x80 & dat; //仅当第7位为0时才可操作(判别busy信号)
    }
    while(!(dat==0x00));
}
/*****************************************************************************
函数功能:写命令到LCD程序，RS(DI)=L,RW=L,EN=H，即来一个脉冲写一次
入口参数:cmdcode
出口参数:
*****************************************************************************/
void write_com(unsigned char cmdcode)
{
    CheckState();//检测LCD是否忙
	LCDDi=0;
	LCDRW=0;
	P1=cmdcode;		
	LCDdelay(2);
	LCDEnable=1;
	LCDdelay(2);
	LCDEnable=0;
}
/*****************************************************************************
函数功能:LCD初始化程序
入口参数:
出口参数:
*****************************************************************************/
void init_lcd()
{
	LCDdelay(100);	
	LCDMcs=1;//刚开始关闭两屏
	LCDScs=1;
	LCDdelay(100);
	write_com(LCDLCDDisp_Off);	 //写初始化命令
	write_com(Page_Add+0);
	write_com(Start_Line+0);
	write_com(LCDCol_Add+0);
	write_com(LCDLCDDisp_On);
}
/*****************************************************************************
函数功能:写数据到LCD程序，RS(DI)=H,RW=L,EN=H，即来一个脉冲写一次
入口参数:LCDDispdata
出口参数:
*****************************************************************************/
void write_data(unsigned char LCDDispdata)
{
    CheckState();//检测LCD是否忙
	LCDDi=1;
	LCDRW=0;
	P1=LCDDispdata;
	LCDdelay(2);
	LCDEnable=1;
	LCDdelay(2);
	LCDEnable=0;
}
/*****************************************************************************
函数功能:清除LCD内存程序
入口参数:pag,col,hzk
出口参数:
*****************************************************************************/
void Clr_Scr()
{
	unsigned char j,k;
	LCDMcs=0; //左、右屏均开显示
	LCDScs=0;
 	write_com(Page_Add+0);
 	write_com(LCDCol_Add+0);
 	for(k=0;k<8;k++)//控制页数0-7，共8页
 	{
		write_com(Page_Add+k); //每页每页进行写
  		for(j=0;j<64;j++)  //每页最多可写32个中文文字或64个ASCII字符
		{
      		write_com(LCDCol_Add+j);
			write_data(0x00);//控制列数0-63，共64列，写点内容，列地址自动加1
		}
	}
}
/*****************************************************************************
函数功能:指定位置显示汉字16*16程序
入口参数:page,column,hzk
出口参数:
*****************************************************************************/
void hz_LCDDisp16(unsigned char page,unsigned char column, unsigned char code *hzk)
{
	unsigned char j=0,i=0;
	for(j=0;j<2;j++)
	{
		write_com(Page_Add+page+j);
		write_com(LCDCol_Add+column);
		for(i=0;i<16;i++) 
			write_data(hzk[16*j+i]);
	}
}

void main()
{
	init_lcd();
	Clr_Scr();
	LCDMcs=0; //左屏开显示
	LCDScs=1;
	hz_LCDDisp16(0,48,Hz_Deng);// Hz_Wo为某个汉字的首地址
        LCDMcs=1; //右屏开显示
	LCDScs=0;
        hz_LCDDisp16(0,0,Bmp_Miao);
		hz_LCDDisp16(0,16,Bmp_bo);
	while(1)
    {
  
	}
}

