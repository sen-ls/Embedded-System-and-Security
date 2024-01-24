#include <xmc_common.h>
#include <xmc_gpio.h>
#include <time.h>
#include<stdio.h>
#include<stdlib.h>

#define GPIO_LED2     XMC_GPIO_PORT1,  0
#define GPIO_LED1     XMC_GPIO_PORT1,  1

int cb = 0;

const XMC_GPIO_CONFIG_t LED_config = \
        {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
         .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
         .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};
         
const XMC_GPIO_CONFIG_t in_config = \
      {.mode=XMC_GPIO_MODE_INPUT_TRISTATE,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};


char str[] = "..-.-..--.-----.-.....";

// void delay (unsigned long delay)
// {
//   while (delay--)
//   {
//     __NOP();
//   }
// }


uint32_t SysTickCounter = 0; //32bit for maximum

void initSysTick() {
	SysTick_Config(SystemCoreClock/1000);			
}	

extern void SysTick_Handler(){
	SysTickCounter++;
}

void delay(uint32_t delaySet){
	uint32_t nowSysTickCounter;
	nowSysTickCounter = SysTickCounter;
	while(SysTickCounter - nowSysTickCounter < delaySet);
}
//dot time delay definition
void dot() {
    XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT1, 1);
    delay(100);
    //XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
    //delay(100);
}

void pause() {
	XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
	delay(100);
}

//dash time dely definition
void dash() {
    XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT1, 1);
    delay(300);
    //XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
    //delay(300);
}

void alphaspace(){
	 XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
	delay(300);
}

void wordspace(){
	 XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
	delay(700);
}

      
int main(void) {
    
    
    SysTick_Config(SystemCoreClock/1000);
    

    
   
  
    const XMC_GPIO_CONFIG_t LED_config = \
        {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
         .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
         .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};

  
         XMC_GPIO_Init(XMC_GPIO_PORT1, 1, &LED_config);
         
       while(1){
           
           
            dot();
            pause();          
            dot();
            wordspace();

            dash();
           pause();
            dot();
            pause();          
            
            dash();
            pause();
            dot();
            alphaspace();
            dot();
	pause();
            dash();
            alphaspace();
            dash();
	pause();
            dot();
            wordspace();

            dash();
	pause();
            dash();
            alphaspace();
            dash();
	   pause();
            dash();
	pause();
            dash();
            alphaspace();
            dot();
            pause();
            dash();
            pause();
            dot();
            alphaspace();
            dot();
	pause();
            dot();
 	pause();
            dot();
            alphaspace();
            dot();
            pause();

            delay(5000);
           
//             for (int i=0;i>24;i++ ){
//                 if (str[i] == ".")
//                     dot();
//                 else
//                     dash();
//             }
//             delay(5000-100);
            
            
            
       }
             
  
  

//morse code signal last length

         
return 0;
}

  

