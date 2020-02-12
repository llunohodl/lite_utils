# lite_utils
generic MCU functions: software timers and FIFO buffers

## Software timer example
for STM32F0xx with CubeMX projet structure

### Step 1: Add timer tick function in time base IRQ
in stm32f0xx_it.h (for STM32F0xx) place `Stimer_isr_tick()` call:
```c
...
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void Stimer_isr_tick();
/* USER CODE END PFP */
...
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  Stimer_isr_tick();
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

``` 
### Step 2: Add timer name in enum
Copy "lite_utils_conf_template.h" and rename to "lite_utils_conf.h"
Then add timer name in `io_stim_t` for example `St_Test`:
```c
//Software timer list
typedef enum{
  St_Test,
  /*Name of new timer must be here with prefix St_ (before _St_QTY)*/
  _St_QTY,
}io_stim_t;
```

### Step 3: Use timer in code
Add `#include "lite_utils.h"` to application code

Set time interval
```c
...
/* Set timer time */
StimerSet(St_Test,100);
...
/* Check timer */
if(StimerTest(St_Test)){
  /*actions (after time elapsed)*/
  
  /*set time to next call of this block*/
  StimerSet(St_Test,150);
}
...
```

Pereodic function process
```c
if(StimerPeriod(St_Test,100)){
  /*actions every 100 ticks (and after reset)*/
  uint8_t need_more_time=0;
  ...
  /*if neaded your may change time to next call of this block */
  if(need_more_time){
    StimerSet(St_Test,200);
  }
}
...
```

Make some actions before time elapsed
```c
uint8_t time_window_fl=0;
...
StimerSet(St_Test,100);
...
stimer_t cur_time=StimerGet(St_Test);
if( (time_window_fl==0) && (50<cur_time) && (cur_time<60)){
  /*actions (before time elapsed)*/
  time_window_fl=1;
}
if(StimerTest(St_Test,100)){
  /*actions (after time elapsed)*/
  time_window_fl=1;
}
```

## Element FIFO example
Add `#include "lite_utils.h"` to application code

Define new fifo (static memory allocation)
```c
/*Example element structure*/
typedef struct{
  uint8_t ID,
  uint8_t Len,
  uint8_t Data[3],
}example_t;

fifo_def(Example_fifo,3,sizeof(example_t));
```
Put element to fifo
```c
example_t Ex_El={.ID=10,.Len=3,.Data={0xAA,0xBB,0xCC}};
...
fifo_put(&Example_fifo,&Ex_El);
```
Get element from fifo
```c
example_t Ex_El_in;
...
if(fifo_put(&Example_fifo,&Ex_El_in)==0){
  if(Ex_El_in.ID==10){
    /* actions for ID 10 */
  }
}
```


