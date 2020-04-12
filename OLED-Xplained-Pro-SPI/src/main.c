// INCLUDES

#include <asf.h>
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// DEFINES

// buttons

#define BUT1_PIO            PIOD
#define BUT1_PIO_ID         16
#define BUT1_PIO_IDX        28
#define BUT1_PIO_IDX_MASK   (1u << BUT1_PIO_IDX)

#define BUT2_PIO			PIOC
#define BUT2_PIO_ID			ID_PIOC
#define BUT2_PIO_IDX		31
#define BUT2_PIO_IDX_MASK	(1 << BUT2_PIO_IDX)

#define BUT3_PIO			PIOA
#define BUT3_PIO_ID			ID_PIOA
#define BUT3_PIO_IDX		19
#define BUT3_PIO_IDX_MASK	(1 << BUT3_PIO_IDX)

// leds

//#define LED_PIO_ID	   ID_PIOC
#define LED_PIO        PIOC
#define LED_PIN		   8
#define LED_PIN_MASK   (1 << LED_PIN)

#define LED1_PIO       PIOA
#define LED1_PIO_ID    ID_PIOA
#define LED1_IDX       0
#define LED1_IDX_MASK  (1 << LED1_IDX)

#define LED2_PIO       PIOC
#define LED2_PIO_ID    ID_PIOC
#define LED2_IDX       30
#define LED2_IDX_MASK  (1 << LED2_IDX)

#define LED3_PIO		PIOB
#define LED3_PIO_ID		ID_PIOB
#define LED3_IDX		2
#define LED3_IDX_MASK	(1 << LED3_IDX)

// GLOBAL VARIABLES

volatile uint8_t flag_led0 = 1;

// PROTOTYPES

void BUT_init(void);
void LED_init(int estado);
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq);

// HANDLERS

// handle interruption but 1

static void Button1_Handler(uint32_t id, uint32_t mask)
{

}

void TC1_Handler(void){
	volatile uint32_t ul_dummy;

	ul_dummy = tc_get_status(TC0, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
	if(flag_led0)
		pin_toggle(LED1_PIO, LED1_IDX_MASK);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();

	// Init OLED
	gfx_mono_ssd1306_init();
  
	// Escreve na tela um circulo e um texto
	gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
	gfx_mono_draw_string("mundo", 50,16, &sysfont);

	/* Insert application code here, after the board has been initialized. */
	while(1) {

	}
}
