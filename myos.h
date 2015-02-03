#ifndef MYOS_H
#define MYOS_H

/**********CPU DEPENDENT************/
#define TASK_TIME_SLICE 	5 		// 5ms for every task to run every time

typedef unsigned char  INT8U;			// Unsigned  8 bit quantity  
typedef unsigned short INT16U;			// Unsigned 16 bit quantity 
typedef unsigned int   INT32U;			// Unsigned 32 bit quantity

typedef unsigned int   OS_STK;			// Each stack entry is 32-bit wide(OS Stack)

// assembling functions
void OS_ENTER_CRITICAL(void); 			// Enter Critical area, that is to disable interruptions
void OS_EXIT_CRITICAL(void);			// Exit Critical area, that is to enable interruptions
void OSCtxSw(void);				// Task Switching Function(OS Context Switch)
void OSStart(void);

OS_STK* OSTaskStkInit(void (*task)(void *p_arg), // task function
	          void *p_arg, // (pointer of arguments)
	      	  OS_STK *p_tos); // (pointer to the top of stack)
/**********CPU INDEPENDENT************/

#define OS_MAX_TASKS	16

#define TASK_STATE_CREATING 	0
#define TASK_STATE_RUNNING	1
#define TASK_STATE_PAUSING	2

#define TASK_STACK_SIZE 	64

typedef struct os_tcb {
	OS_STK	*OSTCBStkPtr; 	// (OS Task Control Block Stack Pointer)
	INT8U 	OSTCBStat; 	// (OS Task Control Block Status)
} OS_TCB; // (OS Task Control Block)

void OSInit(void); 	// (OS Initialization)
void OS_TaskIdle(void *p_arg);
void OSInitTaskIdle(void); // (OS Initialization of "TaskIdle")
void OSTaskCreate(void (*task)(void *p_arg), // task function
		  void *p_arg, 	// (pointer of arguments)
		  OS_STK *p_tos); // (pointer to the top of stack)
void OSTCBSet(OS_TCB *p_tcb, OS_STK *p_tos, INT8U task_state);


void SysTickInit(INT8U Nms);
void SysTick_Handler(void);

INT32U GetTime(void);
void delayMs(volatile INT32U ms); // The argument can't be too large

#endif
