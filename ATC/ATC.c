#include "includes.h"
#include <time.h>

#define TASK_PRIO 10
#define TASK_STK_SIZE 512
#define N_TASKS 10


#define COLORS 7

#define N_AIRCRAFT 15
#define RUNWAY_MAX 77
#define N_RUNWAY 4

#define READY 1
#define LANDING 2
#define REMOVAL 3

#define EMPTY 1
#define HALF 2
#define FULL 3

typedef struct {
	INT8U color;
	INT8U posX;
	INT8U posY;
	INT8U fuel;
	INT8U state;
}AIRCRAFT_INFO;

OS_STK TaskStk[N_TASKS][TASK_STK_SIZE]; // task stack 정의
OS_EVENT *SemAir;
OS_EVENT *SemLanding; 
AIRCRAFT_INFO AircraftInfo[N_AIRCRAFT];

INT8U Collision[N_RUNWAY] = { FALSE, FALSE, FALSE, FALSE};
INT8U StartPos[N_RUNWAY][2] = {5, 9, 5, 12, 5, 15, 5, 18};
INT8U MoveX = 6;
INT8U position = 0;
INT8U ColorArray[COLORS] = { DISP_FGND_RED + DISP_BGND_LIGHT_GRAY,
														DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY,
														DISP_FGND_GREEN + DISP_BGND_LIGHT_GRAY,
														DISP_FGND_YELLOW + DISP_BGND_LIGHT_GRAY,
														DISP_FGND_WHITE + DISP_BGND_LIGHT_GRAY,
														DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY,								
														DISP_FGND_GRAY + DISP_BGND_LIGHT_GRAY 
};


void TaskDispInit();
void TaskDispAircraft();
void TaskDispAirport(void *pdata);


void TaskAircraftLanding(void *pdata);
void TaskAircraftMake(void *pdata);

int main (void) {
	INT8U i;
	OSInit();           // uC/OS-II 초기화
	srand(time(NULL));  // 난수 생성 
	
	SemAir = OSSemCreate(N_AIRCRAFT);
  SemLanding = OSSemCreate(N_RUNWAY);

	//OSTaskCreate(task코드, 전달인자, task stack, 우선순위)	
	
	OSTaskCreate(TaskDispAirport, (void*) NULL, &TaskStk[0][TASK_STK_SIZE - 1], TASK_PRIO/2);
	OSTaskCreate(TaskAircraftMake, (void*) NULL, &TaskStk[1][TASK_STK_SIZE - 1], TASK_PRIO);
  for(i=1; i < N_RUNWAY+1; i++) {
		OSTaskCreate(TaskAircraftLanding, (void*) NULL, &TaskStk[i+1][TASK_STK_SIZE - 1], (INT8U)(TASK_PRIO + i));
	}

	
	OSStart();     // multitasking 시작 
	return 0;
}

//초기화면 그리기
void  TaskDispInit() {
	PC_DispStr(0, 0,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 1,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 2,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 3,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 4,  "                                                  *************************     ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 5,  "                                                   < PRESS 'ESC' TO QUIT >      ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 6,  "                                                  *************************     ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 7,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 8,  "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 9,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 10, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 11, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 13, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 14, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 16, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 17, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 19, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 20, "                                                          _________________     ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 21, "                                                         !                 !    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 22, "                                                         !  control tower  !    ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 23, "                                                          -----------------     ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);	
}

void TaskDispAircraft() {
	INT8U i;	
	PC_DispStr(0, 9,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	for(i = 0; i < N_AIRCRAFT; i++) {
		if(AircraftInfo[i].state == READY || AircraftInfo[i].state == LANDING) 
			PC_DispStr(AircraftInfo[i].posX, AircraftInfo[i].posY, "A", AircraftInfo[i].color );
	}
	
}

void TaskDispAirport(void *pdata) {
	INT8U msg[40];
	INT8U i;
	INT16S key;

	TaskDispInit();

	for (;;) {
	
		if (PC_GetKey(&key)) {                             /* See if key has been pressed              */
			if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
				exit(0);                                   /* Yes, return to DOS                       */
			}
	  }

		PC_GetDateTime(msg);
		PC_DispStr(2, 23, msg, DISP_FGND_YELLOW + DISP_BGND_BLUE);        

		TaskDispAircraft();

		OSTimeDly(1);
	}
}

void TaskAircraftMake(void *data) {
	
	INT8U delay, fuel, color, ERR, posX, posY, state;	

	while(TRUE) {

		OSSemPend(SemAir, 0, &ERR);	
		
		if(position >= N_AIRCRAFT){
			position = 0;
		}

		if(AircraftInfo[position].state != READY && AircraftInfo[position].state != LANDING) {
			srand(time(NULL));
			state = READY;
			posX = (rand()%30) + 1;
			posY = (rand()%5) + 1;
			fuel = (rand()%3) + 1;
			color = ColorArray[(INT8U)(rand() % (COLORS - 1))];

		
			
			AircraftInfo[position].state = state;
			AircraftInfo[position].posX = posX;
			AircraftInfo[position].posY = posY;
			AircraftInfo[position].color = color;
			AircraftInfo[position].fuel = FULL;
		}
		delay = (INT8U)(rand() % 4 + 1);

		OSTimeDly(delay);
		
		position++;
	}
}

void TaskAircraftLanding(void *pdata) {
	INT8U i;
	INT8U landing = 0;
	INT8U ERR, runway = 0;


	while(TRUE) {
		
		if(AircraftInfo[landing].state == READY) {
			
			OSSemPend(SemLanding, 0, &ERR);
			
			
			for(i=0; i < N_RUNWAY; i++) {
				if(Collision[i] == FALSE) {
					runway = i;
					break;
				}
			}

			Collision[runway] = TRUE;
			PC_DispStr(AircraftInfo[landing].posX, AircraftInfo[landing].posY, " ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			
			AircraftInfo[landing].state = LANDING;
			AircraftInfo[landing].posX = StartPos[runway][0];
			AircraftInfo[landing].posY = StartPos[runway][1];
			
			while(TRUE) {
				if(AircraftInfo[landing].posX < RUNWAY_MAX) {
					AircraftInfo[landing].posX += MoveX;
				}else {
					PC_DispStr(AircraftInfo[landing].posX, AircraftInfo[landing].posY, " ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
					AircraftInfo[landing].state = REMOVAL;
					OSSemPost(SemAir);
					OSSemPost(SemLanding);
					Collision[runway] = FALSE;
					break;
				}
				OSTimeDly(1);
			}		
		}
		landing++;
		if(landing >= N_AIRCRAFT){
			landing = 0;
		} 	
	}
}




	


