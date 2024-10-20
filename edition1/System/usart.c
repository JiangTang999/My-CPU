#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
// If using uCOS, include necessary OS headers for compatibility
#if SYSTEM_SUPPORT_OS
#include "includes.h"  // uCOS includes  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
// This code is for learning purposes and is not guaranteed to be perfect or bug-free.
// ALIENTEK STM32 Development Board / UART Initialization	   
// Designed by ALIENTEK
// Technical Forum: www.openedv.com
// Created on: 2012/8/18
// Version: V1.5
// All rights reserved. Unauthorized reproduction or distribution is prohibited.
// Copyright (C) 2009-2019 ALIENTEK Co., Ltd.
//********************************************************************************
// V1.3 Updates:
// Added support for different baud rates and UART communication modes.
// Integrated printf redirection to UART.
// Resolved printf character loss bug.
// V1.4 Updates:
// 1. Fixed UART initialization IO bug.
// 2. Updated USART_RX_STA to 2 bytes (now 16 bits).
// 3. Added configuration for USART_REC_LEN for setting buffer length.
// 4. Added support for custom USART1_RX.
// V1.5 Updates:
// 1. Added uCOSII support.
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
// Enabling printf redirection without the need for MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
// Standard structure definitions required by the I/O functions
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
// Define _sys_exit() to avoid semihosting errors    
_sys_exit(int x) 
{ 
	x = x; 
} 
// Define fputc function
// int fputc(int ch, FILE *f)
// {      
//     while((USART1->SR & 0X40) == 0);  // Wait until transmission is complete
//     USART1->DR = (u8) ch;             // Send the character
//     return ch;
// }

#endif 

/* Using microLib approach */
/* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   // If enabling USART1 receive functionality	
u8 USART_RX_BUF[USART_REC_LEN];  // Receive buffer for USART
// Receive status flags
// bit15: Received complete flag
// bit14: Detected carriage return (0x0D)
// bit13~0: Number of valid received bytes
u16 USART_RX_STA = 0;       // USART receive status	  
  
void uart_init(u32 bound){
  // GPIO Initialization
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);  // Enable clocks for USART1 and GPIOA
  
	// USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  // PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // Alternate Function Push-Pull
  GPIO_Init(GPIOA, &GPIO_InitStructure);  // Initialize GPIOA.9
   
  // USART1_RX	  GPIOA.10 initialization
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  // PA.10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // Input floating
  GPIO_Init(GPIOA, &GPIO_InitStructure);  // Initialize GPIOA.10  

  // NVIC configuration for USART1
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  // Preemption priority level 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  // Subpriority level 3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // Enable IRQ channel
	NVIC_Init(&NVIC_InitStructure);  // Initialize NVIC
  
  // USART initialization

	USART_InitStructure.USART_BaudRate = bound;  // Set baud rate
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 8-bit word length
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 1 stop bit
	USART_InitStructure.USART_Parity = USART_Parity_No;  // No parity bit
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // No hardware flow control
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  // Enable receive and transmit

  USART_Init(USART1, &USART_InitStructure);  // Initialize USART1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  // Enable receive interrupt
  USART_Cmd(USART1, ENABLE);  // Enable USART1

}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Serial_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
void USART1_IRQHandler(void)  // USART1 interrupt handler
{
	u8 Res;
#if SYSTEM_SUPPORT_OS  // If using OS, handle interrupts accordingly
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  // Check if received data is available
	{
		Res = USART_ReceiveData(USART1);  // Read received data		
		if((USART_RX_STA & 0x8000) == 0)  // If not finished receiving
		{
			if(USART_RX_STA & 0x4000)  // If carriage return detected
			{
				if(Res != 0x0A) USART_RX_STA = 0;  // Reset on invalid data
				else USART_RX_STA |= 0x8000;  // Set receive complete flag
			}
			else  // Carriage return not yet received
			{	
				if(Res == 0x0D) USART_RX_STA |= 0x4000;  // Set carriage return flag
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;  // Store received byte
					USART_RX_STA++;
					if(USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0;  // Buffer overflow, reset
				}		 
			}
		}   		 
    } 
#if SYSTEM_SUPPORT_OS  // If using OS, exit interrupt handler
	OSIntExit();  											 
#endif
} 
#endif	
