#include "lite_utils.h"


/* Software timer ***********************************/
volatile stimer_t Stimer[_St_QTY+1];

void StimerSet(io_stim_t stim, stimer_t Time_ticks){
  Stimer[stim]=Time_ticks;
}

stimer_t StimerGet(io_stim_t stim){
  return Stimer[stim];
}

uint8_t StimerTest(io_stim_t stim){
  if(Stimer[stim]==0)   return 1;
  return 0;
}
uint8_t StimerPeriod(io_stim_t stim,stimer_t Time_ticks){
  if(Stimer[stim]==0){
    Stimer[stim]=Time_ticks;
    return 1;
  }
  return 0;
}

void StimerDelay(stimer_t Time_ticks){
  #ifdef _CMSIS_OS_H
    if(osThreadGetId()!=NULL){
      osDelay(Time_ticks);
    }else{
      Stimer[_St_QTY]=Time_ticks;
      while(Stimer[_St_QTY]!=0);
    }
  #else
    Stimer[_St_QTY]=Time_ticks;
    while(Stimer[_St_QTY]!=0);
  #endif
}

void Stimer_isr_tick(){
  for(uint8_t i=0; i<_St_QTY+1; i++){
    if(Stimer[i]!=0) Stimer[i]--;
  }
}

/* String *******************************************/

uint8_t ascii2byte(uint8_t val)
{
    if (val >= 'a') {
        return val - 'a' + 10; // convert chars a-f
    }
    if (val >= 'A') {
        return val - 'A' + 10; // convert chars A-F
    }
    return val - '0';     // convert chars 0-9
}

uint8_t nibble2ascii(uint8_t byte) {
    byte &= 0x0F;
    return byte < 10 ? byte + 48 : byte + 55;
}

/* FIFO *********************************************/

static int8_t fifo_sem_get(fifo_t* fifo){
  if(fifo->Sem!=0){ //Acces blocked
    return -1;
  }
  fifo->Sem=1;
  return 0;
}

static void fifo_sem_release(fifo_t* fifo){
  fifo->Sem=0;
}

//FIFO initialization
void fifo_ini(fifo_t* fifo, uint8_t* buff, uint16_t Total_Qty, uint16_t Elem_SZ){
  fifo->NewI=0;
  fifo->OldI=0;
  fifo->Count=0;
  fifo->Buffer=buff;
  fifo->Total_SZ=Total_Qty*Elem_SZ;
  fifo->Elem_SZ=Elem_SZ;
  fifo_sem_release(fifo);
}

//put data in fifo
int8_t fifo_put(fifo_t* fifo, uint8_t* element){
  if(fifo_sem_get(fifo)!=0){
    return -1;
  }
  for(fifo_index_t i=0;i<fifo->Elem_SZ;i++){
    fifo->Buffer[fifo->NewI++]=element[i];
  }
  if(fifo->NewI>=fifo->Total_SZ){
     fifo->NewI=0;
  }
  fifo->Count=fifo->Count+fifo->Elem_SZ;
  if(fifo->Count>fifo->Total_SZ){
    fifo->Count=fifo->Total_SZ;
    fifo_sem_release(fifo);
    return -2;
  }
  fifo_sem_release(fifo);
  return 0;
}

//put data from fifo
int8_t fifo_get(fifo_t* fifo, uint8_t* element){
  if(fifo->Count<fifo->Elem_SZ){ //No data blocked
    return -2;
  }
  if(fifo_sem_get(fifo)!=0){
    return -1;
  }
  for(fifo_index_t i=0;i<fifo->Elem_SZ;i++){
    element[i]=fifo->Buffer[fifo->OldI++];
  }
  if(fifo->OldI>=fifo->Total_SZ){
     fifo->OldI=0;
  }
  fifo->Count=fifo->Count-fifo->Elem_SZ;
  fifo_sem_release(fifo);
  return 0;
}

#ifdef _CMSIS_OS_H
int8_t fifo_get_to(fifo_t* fifo, uint8_t* element, osStatus (*delay)(uint32_t), uint32_t TO){
  if(TO<5){TO=5;}
  TO/=5;
  uint8_t repeat=5;
  while(repeat--){
    delay(TO);
    if(fifo_get(fifo,element)==0){
      return 0;
    }
  }
  return -1;
}
#endif

//take data from fifo (without deleite)
//fifo - pointer to fifo data structure
//element - pointer to element
int8_t fifo_take(fifo_t* fifo, uint8_t* element){
  if(fifo->Count<fifo->Elem_SZ){ //No data blocked
    return -2;
  }
  if(fifo_sem_get(fifo)!=0){
    return -1;
  }
  fifo_index_t OldItmp=fifo->OldI;
  for(fifo_index_t i=0;i<fifo->Elem_SZ;i++){
    element[i]=fifo->Buffer[OldItmp++];
  }
  fifo_sem_release(fifo);
  return 0;
}

//delete last element from fifo 
//fifo - pointer to fifo data structure
int8_t fifo_release(fifo_t* fifo){
  if(fifo_sem_get(fifo)!=0){
    return -1;
  }
  fifo->OldI+=fifo->Elem_SZ;
  if(fifo->OldI>=fifo->Total_SZ){
     fifo->OldI=0;
  }
  fifo->Count-=fifo->Elem_SZ;
  fifo_sem_release(fifo);
  return 0;
}


//delite all data in fifo
void fifo_flash(fifo_t* fifo){
  fifo->NewI=0;
  fifo->OldI=0;
  fifo->Count=0;
  fifo_sem_release(fifo);
}

//get data count in fifo
fifo_index_t fifo_count(fifo_t* fifo){
  return fifo->Count;
}

//FIFO for bytes

//put data in fifo
int8_t fifo_bin_put_byte(fifo_bin_t* fifo, uint8_t byte){
  fifo->Buffer[fifo->NewI++]=byte;
  if(fifo->NewI >= fifo->Total_SZ){
     fifo->NewI=0;
  }
  fifo->Count=fifo->Count+1;
  if(fifo->Count > fifo->Total_SZ){
     fifo->Count=fifo->Total_SZ;
     return -1;
  }
  return 0;
}

//get data from fifo
int8_t fifo_bin_get_byte(fifo_bin_t* fifo, uint8_t* byte){
  if(fifo->Count==0){
    return -1;
  }
  *byte=fifo->Buffer[fifo->OldI++];
  if(fifo->OldI >= fifo->Total_SZ){
     fifo->OldI=0;
  }
  fifo->Count=fifo->Count-1;
  return 0;
}

//put data in fifo
int8_t fifo_bin_put(fifo_bin_t* fifo, uint8_t* byte,fifo_bin_index_t len){
  int8_t ret=0;
  if(len>fifo->Total_SZ){
    len=fifo->Total_SZ;
    ret=-1;
  }
  int8_t ret2=0;
  for(fifo_bin_index_t i=0; i<len; i++){
    ret2=fifo_bin_put_byte(fifo,byte[i]);
  }
  return ret+ret2;
}

//get data from fifo
int8_t fifo_bin_get(fifo_bin_t* fifo, uint8_t* byte,fifo_bin_index_t len){
  if(fifo->Count<len){
    return -1;
  }
  for(fifo_bin_index_t i=0; i<len; i++){
    if(fifo_bin_get_byte(fifo,&byte[i])!=0){
      return -2;
    }
  }
  return 0;
}

//delite all data in fifo
void fifo_bin_flash(fifo_bin_t* fifo){
  fifo->NewI=0;
  fifo->OldI=0;
  fifo->Count=0;
}

//get data count in fifo
fifo_bin_index_t fifo_bin_count(fifo_bin_t* fifo){
  return fifo->Count;
}

