#include "sys.h"	
#include "delay.h"	
#include "modbus.h"
#include "led.h"
#include "key.h"

 int a;
/************************************************
 ALIENTEKս��STM32������ʵ��2
 ������ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 �������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
	
	delay_init();	    	 //��ʱ������ʼ��
	KEY_Init();
	RS485_Init();
	Timer7_Init();
	LED_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	while(1)
	{
		a++;
		RS485_Service();
		//Modbus_03_Solve();
	}
}
