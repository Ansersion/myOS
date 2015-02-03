#include "include.h"

#define LED1TURN() (GPIOA->ODR ^= 1<<8)
#define LED2TURN() (GPIOD->ODR ^= 1<<2)

extern OS_TCB OSTCBTbl[OS_MAX_TASKS]; // (OS Task Control Block Table)
extern OS_STK TASK_IDLE_STK[TASK_STACK_SIZE]; //("TaskIdle" Stack)
extern OS_TCB *OSTCBCur; // Pointer to the current running task(OS Task Control Block Current)
extern OS_TCB *OSTCBNext; // Pointer to the next running task(OS Task Control Block Next)
extern INT8U OSTaskNext; // Index of the next task
extern INT32U TaskTickLeft; // Refer to the time ticks left for the current task
extern INT32U TimeMS;                                    
extern INT32U TaskTimeSlice;

OS_STK Task1Stk[TASK_STACK_SIZE];
OS_STK Task2Stk[TASK_STACK_SIZE];

void Task1(void *p_arg);
void Task2(void *p_arg);
void LedInit(void);

int main(void)
{
	
	
	OSInit();
	OSTaskCreate(Task1, (void*)0, (OS_STK*)&Task1Stk[TASK_STACK_SIZE-1]);
	OSTaskCreate(Task2, (void*)0, (OS_STK*)&Task2Stk[TASK_STACK_SIZE-1]);
	
	SysTickInit(5);
	LedInit();
	//while(1);
	OSStart();
	
	// while(1);
	//while(1){
	//	GPIOA->ODR &= ~(1<<8);
	//	GPIOD->ODR |= 1<<2;
	//}
}

void LedInit(void)
{
	RCC->APB2ENR |= 1<<2;
	RCC->APB2ENR |= 1<<5;
	//GPIOE->CRH&=0X0000FFFF;  
	//GPIOE->CRH|=0X33330000;
	
	GPIOA->CRH &= 0xfffffff0;
	GPIOA->CRH |= 0x00000003;
	//GPIOA->ODR &= 0xfffffeff;
	GPIOA->ODR |= 1<<8;

	GPIOD->CRL &= 0xfffff0ff;
	GPIOD->CRL |= 0x00000300;
	//GPIOD->ODR &= 0xfffffffd;	
	GPIOD->ODR |= 1<<2;
	
	//LED1TURN();
	LED2TURN();
	
}

void Task1(void *p_arg)
{
	while(1) {
		delayMs(100);
		LED1TURN();
	}
}
void Task2(void *p_arg)
{
	while(1) {
		delayMs(200);
		LED2TURN();
	}
}
