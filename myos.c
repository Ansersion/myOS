#include "myos.h"
#include "stm32f10x.h"


OS_TCB OSTCBTbl[OS_MAX_TASKS]; // (OS Task Control Block Table)
OS_STK TASK_IDLE_STK[TASK_STACK_SIZE]; //("TaskIdle" Stack)
OS_TCB *OSTCBCur; // Pointer to the current running task(OS Task Control Block Current)
OS_TCB *OSTCBNext; // Pointer to the next running task(OS Task Control Block Next)
INT8U OSTaskNext; // Index of the next task
INT32U TaskTickLeft; // Refer to the time ticks left for the current task
INT32U TimeMS;
INT32U TaskTimeSlice;
char * Systick_priority = (char *)0xe000ed23;
// Initialize the stack of a task, it is of much relationship with the specific CPU
OS_STK* OSTaskStkInit(void (*task)(void *p_arg),
	      void *p_arg,
	      OS_STK *p_tos)
{
	OS_STK *stk;
	stk = p_tos;

	*(stk)    = (INT32U)0x01000000L;             // xPSR                                               
	*(--stk)  = (INT32U)task;                    // Entry Point  

	// Don't be serious with the value below. They are of random
	*(--stk)  = (INT32U)0xFFFFFFFEL;             // R14 (LR) 
	*(--stk)  = (INT32U)0x12121212L;             // R12                                                
	*(--stk)  = (INT32U)0x03030303L;             // R3                                                 
	*(--stk)  = (INT32U)0x02020202L;             // R2                                                 
	*(--stk)  = (INT32U)0x01010101L;             // R1                                                 

	// pointer of the argument
	*(--stk)  = (INT32U)p_arg;                   // R0

	// Don't be serious with the value below. They are of random
	*(--stk)  = (INT32U)0x11111111L;             // R11 
	*(--stk)  = (INT32U)0x10101010L;             // R10 
	*(--stk)  = (INT32U)0x09090909L;             // R9  
	*(--stk)  = (INT32U)0x08080808L;             // R8  
	*(--stk)  = (INT32U)0x07070707L;             // R7  
	*(--stk)  = (INT32U)0x06060606L;             // R6  
	*(--stk)  = (INT32U)0x05050505L;             // R5  
	*(--stk)  = (INT32U)0x04040404L;             // R4  
	return stk;
}

// Only to initialize the Task Control Block Table
void OSInit(void)
{
	INT8U i;
	OS_ENTER_CRITICAL();
	for(i = 0; i < OS_MAX_TASKS; i++) {
		OSTCBTbl[i].OSTCBStkPtr = (OS_STK*)0;
		OSTCBTbl[i].OSTCBStat = TASK_STATE_CREATING;
	}
	OSInitTaskIdle();
	OSTCBCur = &OSTCBTbl[0];
	OSTCBNext = &OSTCBTbl[0];
	OS_EXIT_CRITICAL();
}

void OSInitTaskIdle(void)
{
	OS_ENTER_CRITICAL();
	OSTCBTbl[0].OSTCBStkPtr = OSTaskStkInit(OS_TaskIdle, (void *)0, (OS_STK*)&TASK_IDLE_STK[TASK_STACK_SIZE - 1]);
	OSTCBTbl[0].OSTCBStat = TASK_STATE_RUNNING;
	OS_EXIT_CRITICAL();
}

void OSTaskCreate(void (*task)(void *p_arg), 
		  void *p_arg, 
		  OS_STK *p_tos)
{
	OS_STK * tmp;
	INT8U i = 1;
	OS_ENTER_CRITICAL();
	while(OSTCBTbl[i].OSTCBStkPtr != (OS_STK*)0) {
		i++;
	}
	tmp = OSTaskStkInit(task, p_arg, p_tos);
	OSTCBSet(&OSTCBTbl[i], tmp, TASK_STATE_CREATING);
	OS_EXIT_CRITICAL();
}

void OSTCBSet(OS_TCB *p_tcb, OS_STK *p_tos, INT8U task_state)
{
	p_tcb->OSTCBStkPtr = p_tos;
	p_tcb->OSTCBStat = task_state;
}

void OS_TaskIdle(void *p_arg)
{
	p_arg = p_arg; // No use of p_arg, only for avoiding "warning" here.
	for(;;) {
		// OS_ENTER_CRITICAL();
		// Nothing to do
		// OS_EXIT_CRITICAL();
	}
}

// void SysTick_Handler(void)
// {
// 	// OS_ENTER_CRITICAL();
// 	// OS_EXIT_CRITICAL();
// }
void SysTick_Handler(void)
{
	OS_ENTER_CRITICAL();
	if((--TaskTimeSlice) == 0){
		TaskTimeSlice = TASK_TIME_SLICE;
		OSTCBCur = OSTCBNext;
		OSCtxSw();
		OSTaskNext++;
		while(OSTCBTbl[OSTaskNext].OSTCBStkPtr == (OS_STK*)0) { 
			OSTaskNext++;
			if(OSTaskNext >= OS_MAX_TASKS) {
				OSTaskNext = 0;
			}
		}
		OSTCBNext = &OSTCBTbl[OSTaskNext];
		TaskTimeSlice = TASK_TIME_SLICE;
	}
	TimeMS++;
	OS_EXIT_CRITICAL();
}

void SysTickInit(INT8U Nms)
{
	
	OS_ENTER_CRITICAL();
	
	TimeMS = 0;
	TaskTimeSlice = TASK_TIME_SLICE;                             

	SysTick->LOAD  = 1000 * Nms - 1; 
	*Systick_priority = 0x00;
	SysTick->VAL   = 0;  
	SysTick->CTRL = 0x3;
	OS_EXIT_CRITICAL();
}

INT32U GetTime(void)
{
	return TimeMS;
}

void delayMs(volatile INT32U ms)
{
	INT32U tmp;
	tmp = GetTime() + ms;
	while(1){
		if(tmp < GetTime()) break;
	}
}
