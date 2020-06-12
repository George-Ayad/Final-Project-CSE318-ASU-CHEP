#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "drivers/pinout.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "uC.h"


//------------ Global Variables -------------
uint32_t g_ui32SysClock ;
uint32_t SECONDS= 100 ;
char State = 'm' ;
/*
 States :
 m = microwave idle 
 M = microwave active 
 g = Grill idle
 G = Grill active
 Refer to finite state machine diagram for more info
*/


//------------ FUNCTIONS -------------
void PAUSE();
void STOP();
void CLEAR();
void START();
void WELCOME();
void ENDSEQUENCE();
void MICROWAVEIDLE();
void MICROWAVESTART();
void GRILLIDLE();
void GRILLSTART();
void TIMER1A_HANDLER();
void TIMER0A_HANDLER();
void TIMER0AINIT();
void TIMER1AINIT();
uint32_t ATI(char I[10]);
void UARTINIT();


void PAUSE(){
	UARTprintf("Paused \n");
	if(State == 'M')MICROWAVEIDLE();
	if(State == 'G')GRILLIDLE();
	UARTprintf("Remaining Time = %d ", SECONDS);
}


void STOP(){
	UARTprintf("Stopped \n");
	if(State == 'M')MICROWAVEIDLE();
	if(State == 'G')GRILLIDLE();
	UARTprintf("Remaining Time = %d ", SECONDS);
	Clear();
}


void CLEAR(){
	SECONDS = 0 ;
	UARTprintf("Timer cleared \n");
}


void START(){

}


void WELCOME(){  
	UARTprintf("WELCOME ! \n");
	UARTprintf("Please choose a mode \n");
	UARTprintf("M -> Microwave mode \n");
	UARTprintf("G -> Grill mode \n");
	UARTprintf("Default : \n");
	MICROWAVEIDLE();  
}


void ENDSEQUENCE(){
	// Flash RLED 5 times
	if(State = 'M') MICROWAVEIDLE(); 
	else if(State = 'G') GRILLIDLE();
	else MICROWAVEIDLE();
}


void MICROWAVEIDLE(){
	State = 'm';
	UARTprintf("MICROWAVE MODE \n ");
	UARTprintf("S -> start \n ");
	UARTprintf("C -> clear counter \n ");
	UARTprintf("G -> switch to grill \n ");
	UARTprintf("Enter any number of seconds to set counter \n ");	 
	// Turn on BLED 
}


void MICROWAVESTART(){
	State = 'M';
	UARTprintf("P -> pause \n ");
	UARTprintf("S -> STOP \n ");
	TimerEnable(TIMER0_BASE, TIMER_A); 
}


void GRILLIDLE(){
	State = 'g';
	UARTprintf("GRILL MODE \n ");
	UARTprintf("S -> start \n ");
	UARTprintf("C -> clear counter \n ");
	UARTprintf("M -> switch to microwave \n ");
	UARTprintf("Enter any number of seconds to set counter \n ");
	// Turn on GLED
}


void GRILLSTART(){
	State = 'G';
	UARTprintf("P -> pause \n ");
	UARTprintf("S -> STOP \n ");
	TimerEnable(TIMER0_BASE, TIMER_A);
}


void TIMER0AINIT(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntPrioritySet( INT_TIMER0A , 0);
	IntRegister( INT_TIMER0A , TIMER0A_HANDLER);
	TimerIntRegister( INT_TIMER0A, TIMER_A, TIMER0A_HANDLER);
	}


void TIMER1AINIT(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER1_BASE, TIMER_A, g_ui32SysClock*3);
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	IntPrioritySet( INT_TIMER1A , 4);
	IntRegister( INT_TIMER1A , TIMER1A_HANDLER);
	TimerIntRegister( INT_TIMER1A, TIMER_A, TIMER1A_HANDLER);  
}


void UARTINIT(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlDelay(10);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 9600 ,
							(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
							 UART_CONFIG_PAR_NONE));
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	UARTIntRegister(UART0_BASE,  UARTIntHandler);

	//UARTEnable(GPIO_PORTA_BASE);
	UARTStdioConfig(0, 9600 , 120000000);
}


void TIMER1A_HANDLER(){
	STOP();
	UARTprintf("TIMER CLEARED \n");
}


void TIMER0A_HANDLER(){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);  

	if(State == 'M' || State == 'G'){
		if (SECONDS > 0){
			UARTprintf("\r  %d s", SECONDS);
			SECONDS--;
		}

		if (SECONDS == 0){
			UARTprintf("\r %d s", SECONDS);
			UARTprintf("\r DONE \n");
			ENDSEQUENCE();
		}
	}
}


// ASCII String to integer conversion
uint32_t ATI(char I[10]){
	uint32_t R = 0 ;
	int i =0 ;

	while(I[i] !=  '\0'){
		R = R*10 + (I[i]-'0') ;
		i++;
	}

	return R;
}


void UARTIntHandler(void){
	uint32_t ui32Status;
	char pc[10], c;
	char f ;

	// Get the interrrupt status.
	ui32Status = UARTIntStatus(UART0_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(UART0_BASE, ui32Status);

	// Loop while there are characters in the receive FIFO.
	int i =0 ;
	while(f != '\n'){
		while(UARTCharsAvail(UART0_BASE)){
			// Read the next character from the UART and write it back to the UART.
			f = UARTCharGetNonBlocking(UART0_BASE);
			if(f != '\n'){
				pc[i]=f;  
			}
			i++;
		}
	}
	pc[i]= '/0' ;
	UARTprintf("wsl b salam ya s3at el basha : %s /n", pc);

	// State based action decision tree
	if(State == 'M'){
		// Microwave active
		if( pc[0] == 'P' ){
			// Pause
			PAUSE();
			MICROWAVEIDLE();
		}

		else if( pc[0] == 'S' ){
			// Stop
			STOP();
			MICROWAVEIDLE();
		}

		else{
			// Unknown
			UARTprintf("aktb 3edl ya zeft");
		}
	}

	if(State == 'G'){
		// Grill active
		if( pc[0] == 'P' ){
			// Pause
			PAUSE();
			GRILLIDLE();
		}
		
		else if( pc[0] == 'S' ){
			// Stop
			STOP();
			GRILLIDLE();
		}

		else{
			// Unknown
			UARTprintf("aktb 3edl ya zeft");
		}
	}

	if(State == 'm'){
		// Microwave Idle
		if(pc[0] <= '9'  && pc[0] >= '0' ){
			// Timer adjust
			SECONDS = ATI(pc);
			UARTprintf("SECONDS = %d ", SECONDS);
		}

		else if(pc[0] == 'S' ){
			// Start
			MICROWAVESTART();
		}

		else if(pc[0] == 'C' ){
			// Clear timer
			STOP();
		}

		else{
			// Unknown
			UARTprintf("aktb 3edl ya zeft");
		}
	}

	if(State == 'g'){
		// Grill idle 
		if(pc[0] <= '9'  && pc[0] >= '0' ){
			// Timer adjust
			SECONDS = ATI(pc);
			UARTprintf("SECONDS = %d ", SECONDS);
		}

		else if(pc[0] == 'S' ){ 
			// Start
			GRILLSTART();
		}

		else if(pc[0] == 'C' ){
			// Clear timer
			STOP();
		}

		else{
			// Unknown
			UARTprintf("aktb 3edl ya zeft");
		}
	}
}


int main(){
	// ---------- CLOCK SET-UP ----------
	g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	// ---------- INITIALIZATIONS ----------
	IntMasterEnable();
	TIMER0AINIT();
	UARTINIT();
	
	// ---------- START SEQUENCE ----------
	WELCOME();

	while (1)
	{
		// Processor enters Sleep Mode to save Power instead of Looping
		// Once Interrupt happens, Processor goes directly into Run mode 
		SysCtlSleep();
	}
	return 0 ;
}