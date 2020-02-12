#ifndef  _LITE_UTILS_CONF_H_
#define _LITE_UTILS_CONF_H_

//Software timers
#define stimer_t            uint16_t  

//Software timer list
typedef enum{
  /*Name of new timer must be here with prefix St_ (before _St_QTY)*/
  _St_QTY,
}io_stim_t;

#define USE_RTOS 0 //Use RTOS for delay

#define fifo_index_t        uint8_t
#define fifo_bin_index_t    uint8_t


#endif //_LITE_UTILS_CONF_H_
