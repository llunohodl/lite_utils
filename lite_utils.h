#ifndef  _LITE_UTILS_H_
#define _LITE_UTILS_H_

#include <stdint.h>
#include "lite_utils_conf.h"
#if USE_RTOS>0
#include "cmsis_os.h"
#endif

//Software timers process 
//Must be call every one tick (in time base IRQ)
void Stimer_isr_tick();

//Start timer 
//stim - timer ID (see io_stim_t in lite_utils_conf.h)
//Time_ticks - time in ticks 
void StimerSet(io_stim_t stim, stimer_t Time_ticks);

//Get timer time
//stim - timer ID (see io_stim_t in lite_utils_conf.h)
//return - time in ticks 
stimer_t StimerGet(io_stim_t stim);

//Check is timer triggered
//stim - timer ID (see io_stim_t in lite_utils_conf.h)
//return - 0 when timer ticks > 0 / 1 - when timer ticks == 0
uint8_t StimerTest(io_stim_t stim);                           

//Check is timer triggered and if triggered restart timer 
//stim - timer ID (see io_stim_t in lite_utils_conf.h)
//return - 0 when timer ticks > 0 / 1 - when timer ticks == 0
uint8_t StimerPeriod(io_stim_t stim ,stimer_t Time_ticks);    

//Polling delay
//Time_ticks - time in ticks 
void StimerDelay(stimer_t Time_ticks);                        

//String
uint8_t ascii2byte(uint8_t val);
uint8_t nibble2ascii(uint8_t byte);

//FIFO for elements
//FIFO data structure
typedef struct{
  fifo_index_t NewI;            //Start index (place for new data)
  fifo_index_t OldI;            //Finish index (oldest data index)
  fifo_index_t Count;           //Total data count stored in fifo
  fifo_index_t Elem_SZ;         //One element size in bytes
  fifo_index_t Total_SZ;        //Total number of bytes stored in fifo
  uint8_t* Buffer;              //Pointer for FIFO buffer (static allocation)
  uint8_t  Sem;                 //Blocking flag
}fifo_t;

//Define byte fifo
#define fifo_def(name,qty,size)\
uint8_t name##_buf[qty*size];\
fifo_t name = {.NewI=0,.OldI=0,.Count=0,.Total_SZ=qty,.Elem_SZ=size,.Buffer=name##_buf};

//FIFO initialization
//fifo - pointer to fifo data structure
//buff - pointer to array (uint8_t buff[Total_Qty*Elem_SZ])
//Total_Qty - number of elements in fifo
//Elem_SZ - element size in bytes
void fifo_ini(fifo_t* fifo, uint8_t* buff, uint16_t Total_Qty, uint16_t Elem_SZ);

//put data in fifo
//fifo - pointer to fifo data structure
//element - pointer to element
//return - 0 when put suceed
int8_t fifo_put(fifo_t* fifo, uint8_t* element);

//get data from fifo
//fifo - pointer to fifo data structure
//element - pointer to element
//return - 0 when get suceed
int8_t fifo_get(fifo_t* fifo, uint8_t* element);

#ifdef _CMSIS_OS_H
//get data from fifo
//fifo - pointer to fifo data structure
//element - pointer to element
//return - 0 when get suceed
int8_t fifo_get_to(fifo_t* fifo, uint8_t* element, osStatus (*delay)(uint32_t), uint32_t TO);
#endif

//take data from fifo (without deleite)
//fifo - pointer to fifo data structure
//element - pointer to element
//return - 0 when take suceed
int8_t fifo_take(fifo_t* fifo, uint8_t* element);

//delete last element from fifo 
//fifo - pointer to fifo data structure
//return - 0 when release suceed
int8_t fifo_release(fifo_t* fifo);

//delite all data in fifo
//fifo - pointer to fifo data structure
void fifo_flash(fifo_t* fifo);

//get data count in fifo
//fifo - pointer to fifo data structure
//return - number of elements in fifo
fifo_bin_index_t fifo_count(fifo_t* fifo);

//FIFO for bytes
//FIFO for bytes data structure
typedef struct{
  fifo_bin_index_t NewI;            //Start index (place for new data)
  fifo_bin_index_t OldI;            //Finish index (oldest data index)
  fifo_bin_index_t Count;           //Total byte count stored in fifo
  fifo_bin_index_t Total_SZ;        //Total number of bytes stored in fifo
  uint8_t* Buffer;                  //Pointer for FIFO buffer (static allocation)
}fifo_bin_t;

//Define byte fifo
#define fifo_bin_def(name,sz)\
uint8_t name##_buf[sz];\
fifo_bin_t name = {.NewI=0,.OldI=0,.Count=0,.Total_SZ=sz,.Buffer=name##_buf};


//put data in fifo
//fifo - pointer to fifo data structure
//byte - byte for put
//return - 0 when no rewrite
int8_t fifo_bin_put_byte(fifo_bin_t* fifo, uint8_t byte);

//get data from fifo
//fifo - pointer to fifo data structure
//byte - pointer to byte
//return - 0 when get suceed
int8_t fifo_bin_get_byte(fifo_bin_t* fifo, uint8_t* byte);

//put data in fifo
//fifo - pointer to fifo data structure
//byte - pointer to byte array
//len - length of byte array (in bytes)
//return - 0 when no rewrite
int8_t fifo_bin_put(fifo_bin_t* fifo, uint8_t* byte,fifo_bin_index_t len);

//get data from fifo
//fifo - pointer to fifo data structure
//byte - pointer to byte array
//len - length of byte array (in bytes)
//return - 0 when get suceed
int8_t fifo_bin_get(fifo_bin_t* fifo, uint8_t* byte,fifo_bin_index_t len);

//delite all data in fifo
//fifo - pointer to fifo data structure
void fifo_bin_flash(fifo_bin_t* fifo);

//get data count in fifo
//fifo - pointer to fifo data structure
//return - number of bytes in fifo
fifo_bin_index_t fifo_bin_count(fifo_bin_t* fifo);

#endif //_LITE_UTILS_H_