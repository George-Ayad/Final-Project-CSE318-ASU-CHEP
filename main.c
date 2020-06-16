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
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "gpio.h" 

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
void CLEART();
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
void UARTIntHandler(void);
void GPIO_HANDLER(void);
char keypad_scanner(void);
void delay_ms(int nCount);
void GPIOINIT();


char keypad_scanner(void)  {           
	pinwrite(PB2, LOW);
	pinwrite(PB3, HIGH);

	if (pinread(PB4) == 0) {return '1'; }
	if (pinread(PB5) == 0) {return '2'; }
	if (pinread(PB6) == 0) {return '3'; }
	if (pinread(PB7) == 0) {return '4'; }
	
	pinwrite(PB2, HIGH);
	pinwrite(PB3, LOW);

	if (pinread(PB4) == 0) {return '5'; }
	if (pinread(PB5) == 0) {return '6'; }
	if (pinread(PB6) == 0) {return '7'; }
	if (pinread(PB7) == 0) {return '8'; }
	
	return('\0');
}


void GPIO_HANDLER(void){
	GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
	GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4);
	if (pinread(PushButton1) == 0) {
		if(State=='g'){GRILLSTART();}
		if(State=='m'){MICROWAVESTART();}
	}
	if (pinread(PushButton2) == 0) {
		TimerEnable(TIMER1_BASE, TIMER_A); 
	}
}


void KEYPAD_HANDLER(void){
	GPIOIntDisable(GPIO_PORTB_BASE,GPIO_INT_PIN_4);
	GPIOIntDisable(GPIO_PORTB_BASE,GPIO_INT_PIN_5);
	GPIOIntDisable(GPIO_PORTB_BASE,GPIO_INT_PIN_6);
	GPIOIntDisable(GPIO_PORTB_BASE,GPIO_INT_PIN_7);
	GPIOIntClear(GPIO_PORTB_BASE,GPIO_INT_PIN_4);
	GPIOIntClear(GPIO_PORTB_BASE,GPIO_INT_PIN_5);
	GPIOIntClear(GPIO_PORTB_BASE,GPIO_INT_PIN_6);
	GPIOIntClear(GPIO_PORTB_BASE,GPIO_INT_PIN_7);
	
	char AA = '5';
	AA = keypad_scanner();
	
	if(State == 'm' || State == 'g'){
		if(AA == '1'){SECONDS+=10;UARTprintf("SECONDS = %d ", SECONDS);}// 10 SECONDS
		if(AA == '2'){SECONDS+=60;UARTprintf("SECONDS = %d ", SECONDS);}// 1 MINUTE
		if(AA == '3'){SECONDS+=1800;UARTprintf("SECONDS = %d ", SECONDS);}// 30 MINUTES
		if(AA == '4'){if(State=='m')MICROWAVESTART();if(State=='g')GRILLSTART();}
		if(AA == '5'){MICROWAVEIDLE();}// MICROWAVE IDLE
		if(AA == '6'){GRILLIDLE();}// GRILL IDLE 
		if(AA == '8'){if(State=='m' || State=='g')STOP();if(State=='M' || State=='G')PAUSE();}
	}
	
	if(State == 'M' || State == 'G'){
		if(AA == '1'){SECONDS+=10;UARTprintf("SECONDS = %d ", SECONDS);}// 10 SECONDS
		if(AA == '2'){SECONDS+=60;UARTprintf("SECONDS = %d ", SECONDS);}// 1 MINUTE
		if(AA == '3'){SECONDS+=1800;UARTprintf("SECONDS = %d ", SECONDS);}// 30 MINUTES
		//if(AA == '4'){if(State=='m')MICROWAVESTART();if(State=='g')GRILLSTART();}
		//if(AA == '5'){MICROWAVEIDLE();}// MICROWAVE IDLE
		//if(AA == '6'){GRILLIDLE();}// GRILL IDLE 
		if(AA == '8'){PAUSE();}
	}
	
	pinwrite(PB2, LOW);
	pinwrite(PB3, LOW);
	delay_ms(100);
	
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_4);
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_5);
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_6);
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_7);
}


void delay_ms(int nCount){ /* Wait function */ 
	nCount=nCount*16000; 
	while (nCount--);
}


void GPIOINIT(){
    // ON-BOARD
	pinmode(RLED, OUTPUT);
	pinmode(GLED, OUTPUT);
	pinmode(BLED, OUTPUT);
	pinmode(PushButton2, PULLUP);
	pinmode(PushButton1, PULLUP);
	
	// KEYPAD
	pinmode(PB2, OUTPUT);
	pinmode(PB3, OUTPUT);
	pinmode(PB4, PULLUP);
	pinmode(PB5, PULLUP);
	pinmode(PB6, PULLUP);
	pinmode(PB7, PULLUP);
	
	// ON-BOARD INTERRUPTS
	GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_0 ,GPIO_FALLING_EDGE);
	GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4 ,GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIO_PORTF_BASE,GPIO_HANDLER);
	GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
	GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4);
	
	// KEYPAD INTERRUPTS
	GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_4, GPIO_FALLING_EDGE);
	GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_5, GPIO_FALLING_EDGE);
	GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_6, GPIO_FALLING_EDGE);
	GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_7, GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIO_PORTB_BASE,KEYPAD_HANDLER);
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_4);
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_5);
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_6);
	GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_7);
	
	pinwrite(PB2, LOW);
	pinwrite(PB3, LOW);
}


void PAUSE(){
	UARTprintf("Paused \n");
	if(State == 'M')MICROWAVEIDLE();
	if(State == 'G')GRILLIDLE();
	UARTprintf("Remaining Time = %d ", SECONDS);
}


void STOP(){
	UARTprintf("     Stopped \n");
	if(State == 'M')MICROWAVEIDLE();
	if(State == 'G')GRILLIDLE();
	UARTprintf("Remaining Time = %d ", SECONDS);
	CLEART();
}


void CLEART(){
	SECONDS = 0 ;
	UARTprintf("Timer cleared \n");
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
	pinwrite(GLED, LOW);
	pinwrite(BLED, LOW);
	for(int i = 0; i < 5; i++){
		pinwrite(RLED, HIGH);
		delay_ms(100);
		pinwrite(RLED, LOW);
		delay_ms(100);
	}
	// Return to idle states 
	if(State == 'M') MICROWAVEIDLE(); 
	else if(State == 'G') GRILLIDLE();
	else MICROWAVEIDLE();
}


void MICROWAVEIDLE(){
	State = 'm';
	UARTprintf("\n MICROWAVE MODE \n ");
	UARTprintf("S -> start \n ");
	UARTprintf("C -> clear counter \n ");
	UARTprintf("G -> switch to grill \n ");
	UARTprintf("Enter any number of seconds to set counter \n ");
	pinwrite(GLED, LOW);
	pinwrite(BLED, HIGH);
}


void MICROWAVESTART(){
    pinwrite(BLED, HIGH);
	State = 'M';
    UARTprintf("\n ------- Microwave Started \n ");
	UARTprintf("P -> pause \n ");
	UARTprintf("S -> STOP \n ");
	TimerEnable(TIMER0_BASE, TIMER_A);
	pinwrite(GLED, LOW);
	pinwrite(BLED, HIGH);
}


void GRILLIDLE(){
	State = 'g';
	UARTprintf("\n GRILL MODE \n ");
	UARTprintf("S -> start \n ");
	UARTprintf("C -> clear counter \n ");
	UARTprintf("M -> switch to microwave \n ");
	UARTprintf("Enter any number of seconds to set counter \n ");
	pinwrite(BLED, LOW);
	pinwrite(GLED, HIGH);
}


void GRILLSTART(){
	State = 'G';
	UARTprintf("\n ------- Grill Started \n ");
	UARTprintf("P -> pause \n ");
	UARTprintf("S -> STOP \n ");
	TimerEnable(TIMER0_BASE, TIMER_A);
	pinwrite(BLED, LOW);
	pinwrite(GLED, HIGH);
}


void TIMER0AINIT(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, 16000000);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntPrioritySet( INT_TIMER0A , 4);
	IntRegister( INT_TIMER0A , TIMER0A_HANDLER);
	TimerIntRegister( INT_TIMER0A, TIMER_A, TIMER0A_HANDLER);
}


void TIMER1AINIT(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER1_BASE, TIMER_A, 16000000*3);
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	IntPrioritySet( INT_TIMER1A , 1);
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
	UARTStdioConfig(0, 9600 , 16000000);
}


void TIMER1A_HANDLER(){
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerDisable(TIMER1_BASE, TIMER_A);
	if(pinread(PushButton2) == 0){
		STOP();
		UARTprintf("TIMER CLEARED \n");
	}
}


void TIMER0A_HANDLER(){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);
	if(State == 'M' || State == 'G'){
		if (SECONDS > 0){
			UARTprintf("\r %d s", SECONDS);
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
	char pc[10];
	char f = '0';

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
	pc[i] = '\0' ;
	// UARTprintf("Recieved : %s \n", pc);

	// State based action decision tree
	if(State == 'M'){
		// Microwave active
		if( pc[0] == 'P' ){
			// Pause
			PAUSE();
		}

		else if( pc[0] == 'S' ){
			// Stop
			STOP();
		}

		else{
			// Unknown
			UARTprintf("Wrong Entry ! \n");
		}
	}

	else if(State == 'G'){
		// Grill active
		if( pc[0] == 'P' ){
			// Pause
			PAUSE();
		}
		
		else if( pc[0] == 'S' ){
			// Stop
			STOP();
		}

		else{
			// Unknown
			UARTprintf("Wrong Entry !t \n");
		}
	}

	else if(State == 'm'){
		// Microwave Idle
		if(pc[0] <= '9'  && pc[0] >= '0' ){
			// Timer adjust
			SECONDS = ATI(pc);
			UARTprintf("SECONDS = %d \n", SECONDS);
		}

		else if(pc[0] == 'S' ){
			// Start
			MICROWAVESTART();
		}

		else if(pc[0] == 'C' ){
			// Clear timer
			STOP();
		}
                
                else if(pc[0] == 'M' ){
			// Set to microwave
			MICROWAVEIDLE();
		}
                
                else if(pc[0] == 'G' ){
			// Set to grill
			GRILLIDLE();
		}

		else{
			// Unknown
			UARTprintf("Wrong Entry ! \n");
		}
	}

	else if(State == 'g'){
		// Grill idle 
		if(pc[0] <= '9'  && pc[0] >= '0' ){
			// Timer adjust
			SECONDS = ATI(pc);
			UARTprintf("SECONDS = %d \n", SECONDS);
		}

		else if(pc[0] == 'S' ){ 
			// Start
			GRILLSTART();
		}

		else if(pc[0] == 'C' ){
			// Clear timer
			STOP();
		}
                
                else if(pc[0] == 'M' ){
			// Set to microwave
			MICROWAVEIDLE();
		}
                
                else if(pc[0] == 'G' ){
			// Set to grill
			GRILLIDLE();
		}

		else{
			// Unknown
			UARTprintf("Wrong Entry ! \n");
		}
	}
}


int main(){
	// ---------- CLOCK SET-UP ----------
	g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN), 16000000);


	// ---------- INITIALIZATIONS ----------
	IntMasterEnable();
	TIMER0AINIT();
	TIMER1AINIT();
	UARTINIT();
	GPIOINIT();
	
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