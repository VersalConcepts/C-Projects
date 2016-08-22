/*
 * Blink_Interrupt_TCC0.c
 *
 * Created: 8/21/2016 6:51:25 PM
 * Author : Simon
 */ 


#include "sam.h"

int genClockDiv = 1;
int genClockID = 0x4;
// int pwmPER = 375000  0x5B8D8	 
//int pwmPER = 93750; // 0x16E36
int pwmPER = 5;
	
int main(void)
{
	
	PORT->Group[0].DIRSET.reg = (PORT_PA17);
 	
 	//Must be enabled before writing to TCC0;
 	PM->APBCMASK.reg = PM_APBCMASK_TCC0;
	
	GCLK->GENDIV.reg = GCLK_GENDIV_DIV(genClockDiv) | GCLK_GENDIV_ID(genClockID); 
	while(GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(genClockID) |   
	GCLK_GENCTRL_GENEN | GCLK_GENCTRL_IDC | 0 << GCLK_GENCTRL_DIVSEL_Pos;
	while(GCLK->STATUS.bit.SYNCBUSY);
	
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TCC0_TCC1 | GCLK_CLKCTRL_GEN(genClockID) | 
	GCLK_CLKCTRL_CLKEN;  


	TCC0->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
	while(TCC0->SYNCBUSY.reg > 0);
	  
	TCC0->INTENSET.reg |= TCC_INTENSET_OVF | TCC_INTENSET_MC0; 
	NVIC_SetPriority(TCC0_IRQn, 0);
	NVIC_EnableIRQ(TCC0_IRQn);
	
	TCC0->WAVE.bit.WAVEGEN |= TCC_WAVE_WAVEGEN_NPWM;
	while(TCC0->SYNCBUSY.reg > 0);
	
	TCC0->PER.reg = pwmPER;
	while(TCC0->SYNCBUSY.reg > 0);
	
	// CCBx is copied into CCx at TCC update time
	//TCC0->CCB.reg = pwmPer*0.5;
	//while(TCC0->SYNCBUSY.reg > 0);

	TCC0->CC[0].reg = pwmPER*0.5;
	while(TCC0->SYNCBUSY.reg > 0);
	
	TCC0->COUNT.reg = 0;
	while(TCC0->SYNCBUSY.reg > 0);
	
	TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;
	while(TCC0->SYNCBUSY.reg > 0);
	
    while (1) 
    {
    }
}

void TCC0_Handler(){
	
	PORT->Group[0].OUTTGL.reg = PORT_PA17;

	TCC0->INTFLAG.reg |= TCC_INTFLAG_MC0 | TCC_INTFLAG_OVF;	
}