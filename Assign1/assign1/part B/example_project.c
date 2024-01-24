
#include <xmc_gpio.h>
#include <xmc_common.h>
#include <xmc_gpio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

 /*GPIO configuration*/ 
#define GPIO_LED2     XMC_GPIO_PORT1,  0
#define GPIO_LED1     XMC_GPIO_PORT1,  1
#define GPIO_BUTTON1  XMC_GPIO_PORT1, 14
#define GPIO_BUTTON2  XMC_GPIO_PORT1, 15

const XMC_GPIO_CONFIG_t out_config = \
      {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};


const XMC_GPIO_CONFIG_t LED_config = \
        {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
         .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
         .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};
         
const XMC_GPIO_CONFIG_t in_config = \
      {.mode=XMC_GPIO_MODE_INPUT_TRISTATE,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};
       
       
uint32_t SysTickCounter = 0; //32bit for maximum
uint32_t counter2 = 0;

void initSysTick() {
	SysTick_Config(SystemCoreClock/1000);			
}	

extern void SysTick_Handler(){
	SysTickCounter++;
	
	counter2++;
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
}

void pause() {
	XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
	delay(100);
}

//dash time dely definition
void dash() {
    XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT1, 1);
    delay(300);
}

void alphaspace(){
	 XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
	delay(300);
}

void wordspace(){
	 XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, 1);
	delay(700);
}


void send() {
     	                             
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
                                               
}


void morsetime(char bh) {
   
	
       switch (bh) {
		case '0':
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
				break;
		case '1':
					dot();
					pause();
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
				break;
		case '2':
					dot();
					pause();
					dot();
					pause();
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
				break;
		case '3':
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
					dash();
					pause();
					dash();
					pause();
				break;
		case '4':
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
					dash();
					pause();
				break;
		case '5':
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
				break;
		case '6':
					dash();
					pause();
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
				break;
		case '7':
					dash();
					pause();
					dash();
					pause();
					dot();
					pause();
					dot();
					pause();
					dot();
					pause();
				break;
		case '8':
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
					dot();
					pause();
					dot();
					pause();
				break;
		case '9':
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
					dash();
					pause();
					dot();
					pause();
		default:
				break;
	}        
                                          
       }
       
int i = 0;       
char timesdr[10];



//Here I want to transfer the time to string
/*void time_to_string(int number) {
	int temp = number;
    int digicount = 0;
        
        while (temp != 0) {
            temp /= 10;
            digicount++;
        }
	char timearray[digicount+1];
	for (int i = digicount-1; i>=0; --i) {
		timearray[i] = number % 10 + '0';
		number /= 10;
	}
	return timearray[i]; 
}*/

/* If b1 first time pressed*/       
int b1_counter = 0;
int b1_time_differ = 0;
int b1_first_press_time = 0;
int b1_previous_time = 0;
int b1_current_time = 0;
int boot_time = 0;

int main (void) {
  XMC_GPIO_Init(GPIO_LED1,     &out_config);
  XMC_GPIO_Init(GPIO_LED2,     &out_config);
  XMC_GPIO_Init(GPIO_BUTTON1,  &in_config);
  XMC_GPIO_Init(GPIO_BUTTON2,  &in_config);
  
  initSysTick();

while (1) {


    if(XMC_GPIO_GetInput(GPIO_BUTTON1) == 0) {
		b1_previous_time = counter2;
    	if (b1_counter == 0){    		
    		boot_time = counter2;
    		counter2=0;				
    	}    	
      send();
      b1_counter++;
      counter2 = 0;	  
	
    } 
    
    if(XMC_GPIO_GetInput(GPIO_BUTTON2) == 0) {
		if (b1_counter == 0) {
				int ah = '0';
				morsetime(ah);
		}
		if (b1_counter == 1) {
				//how to get boot_time 
			b1_time_differ = boot_time;
			sprintf(timesdr,"%u",boot_time);
			
			for (int a = 0; a < strlen(timesdr);a++){
				morsetime(timesdr[a]);
			}
						
			
			
		}
		if (b1_counter > 1) {
			b1_time_differ = b1_previous_time;
            			sprintf(timesdr,"%u",b1_time_differ);
			
			for (int a = 0; a < strlen(timesdr);a++){
				morsetime(timesdr[a]);
			}
						
						
		}
    }
    
    }
    
    return 0;
}

