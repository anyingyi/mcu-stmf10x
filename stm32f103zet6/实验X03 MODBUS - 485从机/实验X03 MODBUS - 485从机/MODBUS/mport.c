#include "protocol.h"
#include "mcheck.h" 
#include "mport.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//MODBUS 接口层驱动代码	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2017-2027
//All rights reserved	 
////////////////////////////////////////////////////////////////////////////////// 
 

//串口2中断服务函数
//如果是通过其他方式接受，请按要求实现数据存储
//将一帧数据完整的存储到m_ctrl_dev.rxbuf里面,并将帧数据长度存储在m_ctrl_dev.rxlen里面
void USART2_IRQHandler(void)
{
	u8 res;	
	if(USART2->SR&(1<<5))			//接收到数据
	{	 
		res=USART2->DR; 
		if(m_ctrl_dev.frameok==0)	//接收未完成
		{ 
			m_ctrl_dev.rxbuf[m_ctrl_dev.rxlen]=res;
			m_ctrl_dev.rxlen++;
			if(m_ctrl_dev.rxlen>(M_MAX_FRAME_LENGTH-1))m_ctrl_dev.rxlen=0;	//接收数据错误,重新开始接收	  
  		}  		 									     
	}else if(USART2->SR&(1<<4))		//空闲中断
	{
		res=USART2->DR; 			//读DR，清除IDLE位
		if(m_ctrl_dev.rxlen>=M_MIN_FRAME_LENGTH)m_ctrl_dev.frameok=1;		//标记完成一帧数据接收
		else m_ctrl_dev.rxlen=0;	//错误的数据，直接丢弃，重新开始下一次接收
	}
} 


//初始化接口
//这里我们使用串口1，得根据自己的需要修改
//串口2，我们利用空闲中断，做帧结束判断，如果其他接口，没有类似空闲状态监测电话
//必须自己实现帧数据截取（可以用超时法）
//pclk1：PCLK1时钟;
//bound：波特率
void mp_init(u32 pclk1,u32 bound)
{  	   
	RCC->APB2ENR|=1<<5;   	//使能PORTD口时钟  
 	GPIOD->CRL&=0X0FFFFFFF;	//IO状态设置
	GPIOD->CRL|=0X30000000;	//IO状态设置	 
	
	RCC->APB2ENR|=1<<2;   	//使能PORTA口时钟  
	GPIOA->CRL&=0XFFFF00FF;	//IO状态设置
	GPIOA->CRL|=0X00008B00;	//IO状态设置	  

	RCC->APB1ENR|=1<<17;  	//使能串口2时钟 		  
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=(pclk1*1000000)/bound;;// 波特率设置	 
	USART2->CR1|=0X200C;  	//1位停止,无校验位.
	USART2->CR1|=1<<4;	  	//开启串口总线空闲中断.  
	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//组2，最低优先级  
	
	RS485_TX_EN=0;			//默认为接收模式	 
}


//发送指定长度的数据 
//buf：要发送的数据缓冲区首地址
//len：要发送的数据长度
void mp_send_data(u8* buf, u16 len)		
{
	u16 i=0;
	RS485_TX_EN=1;			//进入发送状态
	for(i=0;i<len;i++)
	{
		while((USART2->SR&0X40)==0);	//等待上一次串口数据发送完成  
		USART2->DR=buf[i];      		//写DR,串口1将发送数据
	}		
	while((USART2->SR&0X40)==0);		//等待最后一次数据发送完成  
	RS485_TX_EN=0;						//进入接收状态

}
















