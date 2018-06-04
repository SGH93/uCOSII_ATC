void TaskCraftMove(void*data){
   INT8U i, ERR;
   //INT8U j[15]={0,};
   INT8U j=0;
   
   while(TRUE){
      //OSSemPend(Sem_move, 0, &ERR);
      //위쪽으로 이동

      for(i=0; i<position; i++){
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