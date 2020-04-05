#include <reg51.h>

#define LCDLCDDisp_Off   0x3e
#define LCDLCDDisp_On    0x3f
#define Page_Add     0xb8//ҳ��ַ
#define LCDCol_Add     0x40//�е�ַ
#define Start_Line     0xC0//�е�ַ
/*****Һ����ʾ���Ķ˿ڶ���*****/
#define data_ora P0 /*Һ����������*/
sbit LCDMcs=P2^4 ;    /*Ƭѡ1*/
sbit LCDScs=P2^3 ;    /*Ƭѡ2*/
sbit RESET=P2^3 ;  /*��λ�ź�*/
sbit LCDDi=P2^2 ;     /*����/ָ�� ѡ��*/
sbit LCDRW=P2^1 ;     /*��/д ѡ��*/
sbit LCDEnable=P2^0 ;     /*��/д ʹ��*/



char code Hz_Deng[]= 
{
0xE0,0xF0,0xDF,0x7E,0x88,0x08,0x78,0xD8,0xFE,0xFF,0xE0,0x00,0x00,0x00,0x40,0x80,
0x03,0x04,0xC8,0x39,0x09,0x09,0x08,0x08,0xF8,0xF5,0xF3,0xF0,0xE0,0xE0,0xF0,0xCF
};

char code Bmp_Miao[]= 
{
/*------------------------------------------------------------------------------
  Դ�ļ� / ���� : F:\LD������\Һ��\��ģ��ȡ+�Ű����\Һ����ģ��ȡ\ͼ��\ping.bmpƼ
  ����ߣ����أ�: 16��16
  ��ģ��ʽ/��С : ��ɫ����Һ����ģ������ȡģ���ֽڵ���/32�ֽ�
  ����ת������  : 2010-7-29 8:46:30
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
��������:LCD��ʱ����
��ڲ���:t
���ڲ���:
****************************************************************************/
void LCDdelay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<t;i++);
	for(j=0;j<10;j++);
}
/****************************************************************************
״̬��飬LCD�Ƿ�æ


*****************************************************************************/
void CheckState()		
{
   unsigned char dat,DATA;//״̬��Ϣ���ж��Ƿ�æ��
   LCDDi=0; // ����\ָ��ѡ��D/I��RS��="L" ����ʾ DB7��DB0 Ϊ��ʾָ������ 
   LCDRW=1; //R/W="H" ��E="H"���ݱ�����DB7��DB0 
   do
   {
      DATA=0x00;
      LCDEnable=1;	//EN�½�Դ
	  LCDdelay(2);//��ʱ
	  dat=DATA;
      LCDEnable=0;
      dat=0x80 & dat; //������7λΪ0ʱ�ſɲ���(�б�busy�ź�)
    }
    while(!(dat==0x00));
}
/*****************************************************************************
��������:д���LCD����RS(DI)=L,RW=L,EN=H������һ������дһ��
��ڲ���:cmdcode
���ڲ���:
*****************************************************************************/
void write_com(unsigned char cmdcode)
{
    CheckState();//���LCD�Ƿ�æ
	LCDDi=0;
	LCDRW=0;
	P1=cmdcode;		
	LCDdelay(2);
	LCDEnable=1;
	LCDdelay(2);
	LCDEnable=0;
}
/*****************************************************************************
��������:LCD��ʼ������
��ڲ���:
���ڲ���:
*****************************************************************************/
void init_lcd()
{
	LCDdelay(100);	
	LCDMcs=1;//�տ�ʼ�ر�����
	LCDScs=1;
	LCDdelay(100);
	write_com(LCDLCDDisp_Off);	 //д��ʼ������
	write_com(Page_Add+0);
	write_com(Start_Line+0);
	write_com(LCDCol_Add+0);
	write_com(LCDLCDDisp_On);
}
/*****************************************************************************
��������:д���ݵ�LCD����RS(DI)=H,RW=L,EN=H������һ������дһ��
��ڲ���:LCDDispdata
���ڲ���:
*****************************************************************************/
void write_data(unsigned char LCDDispdata)
{
    CheckState();//���LCD�Ƿ�æ
	LCDDi=1;
	LCDRW=0;
	P1=LCDDispdata;
	LCDdelay(2);
	LCDEnable=1;
	LCDdelay(2);
	LCDEnable=0;
}
/*****************************************************************************
��������:���LCD�ڴ����
��ڲ���:pag,col,hzk
���ڲ���:
*****************************************************************************/
void Clr_Scr()
{
	unsigned char j,k;
	LCDMcs=0; //������������ʾ
	LCDScs=0;
 	write_com(Page_Add+0);
 	write_com(LCDCol_Add+0);
 	for(k=0;k<8;k++)//����ҳ��0-7����8ҳ
 	{
		write_com(Page_Add+k); //ÿҳÿҳ����д
  		for(j=0;j<64;j++)  //ÿҳ����д32���������ֻ�64��ASCII�ַ�
		{
      		write_com(LCDCol_Add+j);
			write_data(0x00);//��������0-63����64�У�д�����ݣ��е�ַ�Զ���1
		}
	}
}
/*****************************************************************************
��������:ָ��λ����ʾ����16*16����
��ڲ���:page,column,hzk
���ڲ���:
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
	LCDMcs=0; //��������ʾ
	LCDScs=1;
	hz_LCDDisp16(0,48,Hz_Deng);// Hz_WoΪĳ�����ֵ��׵�ַ
        LCDMcs=1; //��������ʾ
	LCDScs=0;
        hz_LCDDisp16(0,0,Bmp_Miao);
		hz_LCDDisp16(0,16,Bmp_bo);
	while(1)
    {
  
	}
}

