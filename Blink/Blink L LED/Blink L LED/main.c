#include "sam.h"


int main(void)
{
	/* Initialize the SAM system */
	SystemInit();
	/* Replace with your application code */
	
	REG_PORT_DIRSET0 = PORT_PA17;       // define portA 17 as output
	int    i;
	
	while (1)
	{
//		for (i=0 ; i<150000 ; i++) ;                              // wait
		REG_PORT_OUTSET0 = PORT_PA17;     // Switch the output to 1 or HIGH
//		for (i=0 ; i<150000 ; i++) ;                              // wait
		REG_PORT_OUTCLR0 = PORT_PA17;     // Switch the output to 0 or LOW
	}
}
