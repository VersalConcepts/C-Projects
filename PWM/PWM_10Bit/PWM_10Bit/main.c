/*
 * PWM_10Bit.c
 *
 * Created: 8/17/2016 11:40:53 AM
 * Author : Simon
 */ 


#include "sam.h"

int divFactor = 1; 
int gClock = 0;
int pwmPER =  375000;
int pinOut = 20; // D6

volatile uint32_t mycount;

void syncCount(){
	
	TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_NONE;
	while(TCC0->SYNCBUSY.bit.CTRLB);
	TCC0->CTRLBSET.reg = TCC_CTRLBSET_CMD_READSYNC;
	while(TCC0->SYNCBUSY.bit.CTRLB);
	while (TCC0->SYNCBUSY.bit.COUNT);
	mycount = TCC0->COUNT.reg;
}

int main(void)
{
    /* Initialize the SAM system */
//	SystemInit();

	REG_PORT_DIRSET0 |= PORT_PA17;       // define portA x as output

	/*	I/O pins are assembled in pin groups with up to 32 pins. Group 0 consists of PA pins. Group 1 consists of PB pins.
	PA00 to PA31 offset is 0x00. PB00 to PB31 offset is 0x80. Enable upper half word select, upper 16 pins of PORT group will be configured (we'll use PA17). 
	Allow write to Pin Configuration register. Allow write to Peripheral Multiplexing register. Set peripheral function to E, which is TCC0.
	Enable the peripheral multiplexer in place of the I/O pin. Set TCC0 to output at the selected pin. We select PA17 to output to the "L" LED on the Arduino Zero.
	Using the upper 16 pins (pin16-pin31), we select pin17 or the 1st bit of the Word i.e. 17-16.
	*/
	
	PORT->Group[0].PINCFG[8].bit.PMUXEN = 1;
	PORT->Group[0].PMUX[4].reg = PORT_PMUX_PMUXE_E;
	
//	PORT->Group[0].WRCONFIG.reg	= (uint32_t)(PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG |
//	PORT_WRCONFIG_WRPMUX | PORT_PMUX_PMUXO_E_Val << PORT_WRCONFIG_PMUX_Pos | PORT_WRCONFIG_PMUXEN | 0x1 << (pinOut-16) );

/*
	PORT->Group[0].WRCONFIG.reg	= (uint32_t)( 0x1ul << PORT_WRCONFIG_HWSEL_Pos | PORT_WRCONFIG_WRPINCFG |
	PORT_WRCONFIG_WRPMUX | PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F |
	PORT_PINCFG_PMUXEN | 0x1ul << (pinOut-16) );
*/
	// enable the TCC0 bus clock
	PM->APBCMASK.reg |= PM_APBCMASK_TCC0;
	
	/* Writes to 32 bits. Set the Division Factor in the GENDIV register. This will divide the clock by 1 for generic clock generator 0. 
	We will use the prescaler in the CTRLA register instead. Select the generic clock generator. Generic clock generator 0 is selected.	*/
//	GCLK->GENDIV.reg = (uint32_t)(GCLK_GENDIV_DIV(divFactor) | GCLK_GENDIV_ID(gClock)); 
//	while (GCLK->STATUS.bit.SYNCBUSY); 
	
	/* Divide select must be set to 0 if clock not divided or by 1. Generic clock generator enabled. 48MHz crystal selected for clock source.
	Generic clock generator 0 is selected to be configured. */
//	GCLK->GENCTRL.reg = (uint32_t)( (0x0ul << GCLK_GENCTRL_DIVSEL_Pos) | GCLK_GENCTRL_GENEN |
//	GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(0x6ul) );
//	GCLK->GENCTRL.reg = (uint32_t)((0x0ul << GCLK_GENCTRL_DIVSEL_Pos) | GCLK_GENCTRL_IDC |
//	GCLK_GENCTRL_GENEN |GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(gClock));
//	while (GCLK->STATUS.bit.SYNCBUSY); 


	/* Enable generic clock. Select generic clock generator 0. Select where to output. TCC0 and TCC1 are selected with 0x1A.*/
	GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gClock) |
	GCLK_CLKCTRL_ID_TCC0_TCC1);
	while (GCLK->STATUS.bit.SYNCBUSY); 

	// disable TCC0 to configure.
	TCC0->CTRLA.reg &=~(TCC_CTRLA_ENABLE);
	while (TCC0->SYNCBUSY.bit.ENABLE);
	

//	TCC1->CTRLA.reg &= ~(TCC_CTRLA_ENABLE);
//	while (TCC1->SYNCBUSY.bit.ENABLE);
	
	// Sync the counter with prescaler Reload or reset counter on next prescaler clock.
	//TCC0->CTRLA.reg = TCC_CTRLA_PRESCSYNC(TCC_CTRLA_PRESCSYNC_PRESC_Val);
	
	// set TCC counter direction up	
	TCC0->CTRLBCLR.reg &= ~TCC_CTRLBCLR_DIR;
	while(TCC0->SYNCBUSY.bit.CTRLB);


	// select the waveform generation. Select Normal PWM Single Slope. Saw tooth count.
	TCC0->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;
	while (TCC0->SYNCBUSY.bit.WAVE);
	//TCC0->WAVE.reg |= TCC_WAVE_WAVEGEN_DSBOTTOM |TCC_WAVE_POL(0xF);
//	while (TCC0->SYNCBUSY.bit.WAVE);
	
	/* Resets the period register then set the TOP to 95,999 to get ~500Hz. freq PWM = freq generic clock TCC/(prescaler*(TOP+1))
	Resolution = log(TOP+1)/log(2)  ; 20 bit PWM resolution. For ESC, the resolution is -> 95,999-47,999 = 48,000 clock counts. Approximately, 15-16bit */
//	TCC0->PER.reg = 0x176FFul;
//	while (TCC0->SYNCBUSY.bit.PER);
	TCC0->PER.reg = pwmPER;
	while (TCC0->SYNCBUSY.bit.PER);
		
	/* Compare Value register CCx. Sets the duty cycle. Minimum for ESC is 1ms / 1kHz / 50% duty cycle / 0.5*TOP = 47,999.
	CCx must be between 1ms-2ms / 47,999-95,999. Mid throttle ~= 71,999 or 750Hz. */
//	TCC0->CC[0].reg = 0xBB7Ful;
//	TCC0->CC[0].reg = pwmPER*pwmDuty;
//	while (TCC0->SYNCBUSY.bit.CC0);
	TCC0->CC[0].reg = pwmPER;
	while (TCC0->SYNCBUSY.bit.CC0);

	NVIC_SetPriority(TCC0_IRQn, 0);
	NVIC_EnableIRQ(TCC0_IRQn);
	TCC0->INTFLAG.reg |= TCC_INTFLAG_MC0 | TCC_INTFLAG_OVF;
	TCC0->INTENSET.reg = TCC_INTENSET_MC0 | TCC_INTENSET_OVF;


	TCC0->COUNT.reg = 0x00000000;
	while(TCC0->SYNCBUSY.bit.COUNT);

	// Enable TCC0 now that is configured
	TCC0->CTRLA.reg	|= TCC_CTRLA_PRESCALER_DIV2 | TCC_CTRLA_ENABLE;
	while (TCC0->SYNCBUSY.bit.ENABLE);
	
    /* Replace with your application code */
    while (1) 
    {
		syncCount();

    }
}


void TCC0_Handler(){
	
	REG_PORT_OUTTGL0 = PORT_PA17;

	TCC0->INTFLAG.reg |= TCC_INTFLAG_OVF | TCC_INTFLAG_MC0;
	
}