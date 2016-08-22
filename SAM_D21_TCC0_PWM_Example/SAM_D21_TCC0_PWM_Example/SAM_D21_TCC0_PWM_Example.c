/*
 * TC_example.c
 *
 * Created: 5/15/2014 12:11:02 PM
 *  Author: jerome.semette
 *
 * Description : Illustrate PWM generation on SAMD21 Xplained PRO LED0 (TCC0/WO[0]) 
 *
 */

#include "sam.h"

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint8_t duty_cycle ;
	uint32_t delay ;
	
	/* -- Initialize GPIO (PORT) */ 
	
	// - Set PB30 (LED) as TCC0 Waveform out (PMUX : E = 0x03)
	PORT->Group[0].WRCONFIG.reg = (uint32_t)(PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_WRPMUX | 1 << (20 - 16) | PORT_WRCONFIG_PMUXEN | (0x4 << PORT_WRCONFIG_PMUX_Pos) );
		

	/* -- Enable TCC0 Bus Clock and Generic clock*/
	
	// - Enable TCC0 Bus clock (Timer counter control clock)
	PM->APBCMASK.reg |= PM_APBCMASK_TCC0;
	//Enable GCLK for TCC0 (timer counter input clock)
	GCLK->CLKCTRL.reg = (uint16_t) ((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | ( 0x1A << GCLK_CLKCTRL_ID_Pos)));
	
	/* -- Initialize TCC0 */
	// - DISABLE TCC0
	TCC0->CTRLA.reg &=~(TCC_CTRLA_ENABLE);
	// - Set TCC0 in waveform mode Normal PWM
	TCC0->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;
	// - Set PER to maximum counter value (resolution : 0xFF)
	TCC0->PER.reg = 0xFF;
	// - Set WO[0] compare register to 0xFF (PWM duty cycle = 100%)
	TCC0->CC[0].reg = 0xFF;
	// - ENABLE TCC0
	TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV256 | TCC_CTRLA_ENABLE ;
	
    while (1) 
    {
		/* Decrease PWM duty cycle from 100% to 0% */
		for( duty_cycle = 0xFF ; duty_cycle>0x00 ; duty_cycle-- )
		{
			// - DISABLE TCC0
			TCC0->CTRLA.reg &=~(TCC_CTRLA_ENABLE);
			// - Change duty cycle
			TCC0->CC[0].reg = duty_cycle;
			// - ENABLE TCC0
			TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE ;
			for( delay = 0 ; delay<= 1000 ; delay++ ) ;
		}
		
		
		for( delay = 0 ; delay<= 10000 ; delay++ ) ;
		
		/* Increase PWM duty cycle from 0% to 100% */
		for( duty_cycle = 0x00 ; duty_cycle< 0xFF ; duty_cycle++ )
		{
			// - DISABLE TCC0
			TCC0->CTRLA.reg &=~(TCC_CTRLA_ENABLE);
			// - Change duty cycle
			TCC0->CC[0].reg = duty_cycle;
			// - ENABLE TCC0
			TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE ;
			for( delay = 0 ; delay<= 1000 ; delay++ ) ;
		}
		
		for( delay = 0 ; delay<= 10000 ; delay++ ) ;

    }
}
