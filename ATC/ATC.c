#include "includes.h"
#include <time.h>

#define TASK_STK_SIZE 512
#define N_TASKS 10
#define N_MSG 100

#define COLORS 7

#define N_AIRCRAFT 50
#define RUNWAY_MAX 72
#define N_RUNWAY 4

#define READY 1
#define LANDING 2
#define REMOVAL 3

#define EMPTY 1


typedef struct {
	INT16U name;
	INT8U color;
	INT8U posX;
	INT8U posY;
	INT8U fuel;
	INT8U state;
	INT8U speed;
	INT8U radius;
}AIRCRAFT_INFO;

OS_STK TaskStk[N_TASKS][TASK_STK_SIZE]; // task stack 정의
OS_EVENT *SemAir;
OS_EVENT *SemLanding;
OS_EVENT *msg_q;
void *msg_array[N_MSG]; 
AIRCRAFT_INFO AircraftInfo[N_AIRCRAFT];

INT8U Collision[N_RUNWAY] = { FALSE, FALSE, FALSE, FALSE};
INT8U StartPos[N_RUNWAY][2] = {5, 10, 5, 13, 5, 16, 5, 19};
INT8U position = 0;
INT16U cnt = 1;
INT8U landing = 0;
INT8U ColorArray[COLORS] = { DISP_FGND_RED + DISP_BGND_LIGHT_GRAY,
														 DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY,
														 DISP_FGND_BLUE + DISP_BGND_LIGHT_GRAY,
};


void DispInit();
void DispAircraft();

void TaskEmergency(void *pdata);
void TaskDispAirport(void *pdata);
void TaskAircraftLanding(void *pdata);
void TaskAircraftMake(void *pdata);
void TaskArrivals(void *pdata);
void TaskThunder(void *pdata);
void TaskCraftMove(void *data);


int main (void) {
	INT8U i;
	OSInit();           // uC/OS-II 초기화
	srand(time(NULL));  // 난수 생성 
	
	SemAir = OSSemCreate(N_AIRCRAFT);
  SemLanding = OSSemCreate(N_RUNWAY);

	//OSTaskCreate(task코드, 전달인자, task stack, 우선순위)	
	OSTaskCreate(TaskArrivals, (void*) NULL, &TaskStk[0][TASK_STK_SIZE - 1], 9);
	OSTaskCreate(TaskDispAirport, (void*) NULL, &TaskStk[1][TASK_STK_SIZE - 1], 10);
	OSTaskCreate(TaskAircraftMake, (void*) NULL, &TaskStk[2][TASK_STK_SIZE - 1], 11);
	OSTaskCreate(TaskEmergency, (void*) NULL, &TaskStk[3][TASK_STK_SIZE - 1], 20);
	OSTaskCreate(TaskThunder, (void*) NULL, &TaskStk[4][TASK_STK_SIZE - 1], 14);
	OSTaskCreate(TaskCraftMove, (void*) NULL, &TaskStk[5][TASK_STK_SIZE - 1], 15);

	for(i=0; i < N_RUNWAY; i++) {
		OSTaskCreate(TaskAircraftLanding, (void*) NULL, &TaskStk[i+6][TASK_STK_SIZE - 1], (INT8U)(16 + i));
	}


	

	msg_q = OSQCreate(msg_array, (INT16U) N_MSG); // (4)
	if (msg_q == 0) {
		printf("creating msg_q is failed\n");
		return -1;
	}

	OSStart();     // multitasking 시작 
	return 0;
}

//초기화면 그리기
void DispInit() {
	PC_DispStr(0, 0,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 1,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 2,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 3,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 4,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 5,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 6,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 7,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 8,  "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 9,  "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 11, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 12, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 14, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 15, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 17, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 18, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 20, "                    =========================================================== ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 21, "                                    ___________                                 ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 22, "                                    ! control !                                 ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 23, "                                    !  tower  !                                 ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 24, "                                    -----------       < PRESS 'ESC' TO QUIT >   ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

}

void DispAircraft() {
	INT8U i;
	
	char s[20];	
	for(i = 0; i < N_AIRCRAFT; i++) {
		if(AircraftInfo[i].state == READY || AircraftInfo[i].state == LANDING){
			sprintf(s,"A%d", AircraftInfo[i].name); 
			PC_DispStr(AircraftInfo[i].posX, AircraftInfo[i].posY, s, AircraftInfo[i].color );
			
		}
			
	}
	

}

void TaskDispAirport(void *pdata) {
	INT8U msg[40];
	INT8U i;
	INT16S key;

  
  
	for (;;) {
	
		if (PC_GetKey(&key)) {                             /* See if key has been pressed              */
			if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
				exit(0);                                   /* Yes, return to DOS                       */
			}
	  }

		        
    DispInit(); 
		PC_GetDateTime(msg);
		PC_DispStr(2, 24, msg, DISP_FGND_YELLOW + DISP_BGND_BLUE);
		DispAircraft();

		OSTimeDly(1);
	}
}

void TaskAircraftMake(void *data) {
	
	INT8U radius, name, speed, delay, fuel, color, ERR, posX, posY, state;	

	while(TRUE) {

		OSSemPend(SemAir, 0, &ERR);	
		
		if(position >= N_AIRCRAFT){
			OSTaskDel(11);
		}

		if(AircraftInfo[position].state != READY && AircraftInfo[position].state != LANDING) {
			srand(time(NULL));
			state = READY;
			posX = (rand()%50) + 10;
			posY = 4;
			fuel = (rand()%5) + 1;
			speed = (rand()%4) + 2;
			radius = 4;
			name = cnt;
			if(fuel == EMPTY) color = ColorArray[0];
			else color = ColorArray[1];

		
			
			AircraftInfo[position].state = state;
			AircraftInfo[position].posX = posX;
			AircraftInfo[position].posY = posY;
			AircraftInfo[position].color = color;
			AircraftInfo[position].fuel = fuel;
			AircraftInfo[position].speed = speed;
			AircraftInfo[position].name = name;
			AircraftInfo[position].radius = radius;
		}
		delay = (INT8U)(rand() % 4 + 1);

		OSTimeDly(delay);
		cnt++;
		position++;
	}
}

void TaskAircraftLanding(void *pdata) {
	INT8U i;
	
	INT8U ERR, err, runway = 0;
	char msg[100];
	
	
	
	while(TRUE) {
		srand(time(NULL));
		runway = rand() % 4;
		if(Collision[runway] == FALSE) {
			OSSemPend(SemLanding, 0, &ERR);
			while(TRUE) {
				if(AircraftInfo[position].fuel != EMPTY && AircraftInfo[position].state == READY) break;
			}

			Collision[runway] = TRUE;
			AircraftInfo[position].state = LANDING;
			AircraftInfo[position].posX = StartPos[runway][0];
			AircraftInfo[position].posY = StartPos[runway][1];
			
			while(TRUE) {
				if((AircraftInfo[position].posX + AircraftInfo[position].speed) < RUNWAY_MAX) {
					AircraftInfo[i].posX += AircraftInfo[position].speed;
					OSTimeDly(1);
				}else {
					AircraftInfo[i].state = REMOVAL;
					sprintf(msg, "A%d arrived", AircraftInfo[i].name);
					Collision[runway] = FALSE;		
					err = OSQPost(msg_q, msg);
				}
			
			}
	
			OSSemPost(SemLanding);
		}
	}
}
		

void TaskEmergency(void *pdata) {
	INT8U i;
	INT8U emerg = 0;


	while(TRUE) {
		
		if(AircraftInfo[emerg].state == READY && AircraftInfo[emerg].fuel == EMPTY) {
			OSTimeDly(2);
			AircraftInfo[emerg].color = ColorArray[2];
			OSTimeDly(1);
			AircraftInfo[emerg].state = REMOVAL;
			
		}
		emerg++;
		if(emerg >= N_AIRCRAFT) emerg = 0;
	}
}

void TaskArrivals(void *pdata) {

	void *msg;
	INT8U err;

	for (;;) {
		msg = OSQPend(msg_q, 0, &err);
		if (msg != 0) {
			PC_DispStr(2, 23, msg, DISP_FGND_YELLOW + DISP_BGND_BLUE);
		}
		OSTimeDly(1);
	}
}
	
void TaskThunder(void *pdata){
	INT8U thunder = 0;
	INT8U i;

	for(;;){
		srand(time(NULL));
		thunder = (rand()%30) + 1;
		
		if(thunder == 4) {
			PC_DispStr(65, 23, "Thunder!!!", DISP_FGND_YELLOW + DISP_BGND_BLUE);
			for(i=16; i<20; i++) {
				OSTaskSuspend(i);
			}
			OSTimeDly(10);
			PC_DispStr(65, 23, "             ", DISP_FGND_YELLOW + DISP_BGND_LIGHT_GRAY);
			for(i=16; i<20; i++) {
				OSTaskResume(i);
			}
		}
		OSTimeDly(1);
	}
}

void TaskCraftMove(void*data){
   INT8U i, ERR;
   //INT8U j[15]={0,};
   INT8U j=0;
   
   while(TRUE){
      //OSSemPend(Sem_move, 0, &ERR);
      //위쪽으로 이동

      for(i=0; i<N_AIRCRAFT; i++){
         if(AircraftInfo[i].state!=READY)
            continue;
               
         if(((j/AircraftInfo[i].radius)%4)==0){
            AircraftInfo[i].posY--;
         }
   
         //왼쪽으로 이동
         else if(((j/AircraftInfo[i].radius)%4)==1){
            AircraftInfo[i].posX--;
         }
   
         //아래로 이동
         else if(((j/AircraftInfo[i].radius)%4)==2){
            AircraftInfo[i].posY++;
         }
   
         //오른쪽으로 이동
         else if(((j/AircraftInfo[i].radius)%4)==3){
            AircraftInfo[i].posX++;
         }
      }

      j++;
            
      OSTimeDly(1);
   }
}


