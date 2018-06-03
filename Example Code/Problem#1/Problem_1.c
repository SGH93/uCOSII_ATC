#include "includes.h"
#include <time.h>

#define TASK_STK_SIZE 512
#define N_TASKS 5


OS_STK TaskStk[N_TASKS][TASK_STK_SIZE]; // task stack 정의 

void Task(void *data);
void CreateTasks(void);

int main (void) {
    OSInit();           // uC/OS-II 초기화
    srand(time(NULL));  // 난수 생성 
    CreateTasks();      // Task 생성
    OSStart();          // multitasking 시작 
    return 0;
}

void CreateTasks (void) {
    
	INT8U i;	//int형 변수 선언 	
    for (i = 0; i < N_TASKS; i++) {
		//OSTaskCreate(task코드, 전달인자, task stack, 우선순위)	
    OSTaskCreate(Task, (void *) 0, &TaskStk[i][TASK_STK_SIZE - 1], (INT8U) (i + 1));
    }
}

void Task (void *pdata) {
	FILE *out;
	INT8U sleep;
	//무한반복문 
	for (;;) {
		out = fopen("result.txt", "a");
		// (7)
		fprintf(out, "%4u: Task %u, file open\n", OSTimeGet(), OSTCBCur->OSTCBPrio);
		// (8)
		fflush(out);
		sleep = (rand() % 3) + 1;
		OSTimeDly(sleep); // (9)
		fprintf(out, "%4u: Task %u, file close\n", OSTimeGet(), OSTCBCur->OSTCBPrio);
		//(10)
		fflush(out);
		fclose(out); // (11)
		sleep = (rand() % 3) + 1;
		OSTimeDly(sleep); // (12)
  }
}