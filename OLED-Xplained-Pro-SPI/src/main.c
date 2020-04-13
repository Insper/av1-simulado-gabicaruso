#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// DEFINES
// but 1
#define BUT1_PIO            PIOD
#define BUT1_PIO_ID         16
#define BUT1_PIO_IDX        28
#define BUT1_PIO_IDX_MASK   (1u << BUT1_PIO_IDX)

// but 2
#define BUT2_PIO			PIOC
#define BUT2_PIO_ID			ID_PIOC
#define BUT2_PIO_IDX		31
#define BUT2_PIO_IDX_MASK	(1 << BUT2_PIO_IDX)

// but 3
#define BUT3_PIO			PIOA
#define BUT3_PIO_ID			ID_PIOA
#define BUT3_PIO_IDX		19
#define BUT3_PIO_IDX_MASK	(1 << BUT3_PIO_IDX)

// led placa
#define LED_PIO_ID	   ID_PIOC
#define LED_PIO        PIOC
#define LED_PIN		   8
#define LED_PIN_MASK   (1 << LED_PIN)

// led 1
#define LED1_PIO       PIOA
#define LED1_PIO_ID    ID_PIOA
#define LED1_IDX       0
#define LED1_IDX_MASK  (1 << LED1_IDX)

// led 2
#define LED2_PIO       PIOC
#define LED2_PIO_ID    ID_PIOC
#define LED2_IDX       30
#define LED2_IDX_MASK  (1 << LED2_IDX)

// led 3
#define LED3_PIO		PIOB
#define LED3_PIO_ID		ID_PIOB
#define LED3_IDX		2
#define LED3_IDX_MASK	(1 << LED3_IDX)

// GLOBAL VARIABLES
volatile int but1_flag = 0;
volatile int but2_flag = 0;
volatile int but3_flag = 0;
volatile char tc0_flag = 0;
volatile char tc1_flag = 0;
volatile char tc2_flag = 0;

// PROTOTYPES
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void BUT_init(void);
void LED_init(void);
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq);
void pin_toggle(Pio *pio, uint32_t mask);
void TC0_Handler(void);
void TC1_Handler(void);
void TC2_Handler(void);

// FUNCTIONS
// but1 callback
void but1_callback() {
	but1_flag = 1;
}

// but2 callback
void but2_callback() {
	but2_flag = 1;
}

// but3 callback
void but3_callback() {
	but3_flag = 1;
}

// but init
void BUT_init(void){
	// but1
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_handler_set(BUT1_PIO, BUT1_PIO_ID, BUT1_PIO_IDX_MASK, PIO_IT_RISE_EDGE, but1_callback);
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4);
	
	// but2
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_handler_set(BUT2_PIO, BUT2_PIO_ID, BUT2_PIO_IDX_MASK, PIO_IT_FALL_EDGE, but2_callback);
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4);
	
	// but3
	pmc_enable_periph_clk(BUT3_PIO_ID);
	pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	pio_handler_set(BUT3_PIO, BUT3_PIO_ID, BUT3_PIO_IDX_MASK, PIO_IT_RISE_EDGE, but3_callback);
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4);
};

// led init
void LED_init(){
	// led placa
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIN_MASK, PIO_DEFAULT);
	pio_set(LED_PIO, LED_PIN_MASK);
	
	// led1
	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_IDX_MASK, PIO_DEFAULT);
	pio_clear(LED1_PIO, LED1_IDX_MASK);
	
	// led2
	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_IDX_MASK, PIO_DEFAULT);
	pio_set(LED2_PIO, LED2_IDX_MASK);
	
	// led3
	pmc_enable_periph_clk(LED3_PIO_ID);
	pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_IDX_MASK, PIO_DEFAULT);
	pio_clear(LED3_PIO, LED3_IDX_MASK);
};

// pin toggle
void pin_toggle(Pio *pio, uint32_t mask){
	if(pio_get_output_data_status(pio, mask))
	pio_clear(pio, mask);
	else
	pio_set(pio,mask);
}

// tc init
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	uint32_t channel = 1;

	/* Configura o PMC */
	/* O TimerCounter é meio confuso
	o uC possui 3 TCs, cada TC possui 3 canais
	TC0 : ID_TC0, ID_TC1, ID_TC2
	TC1 : ID_TC3, ID_TC4, ID_TC5
	TC2 : ID_TC6, ID_TC7, ID_TC8
	*/
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  4Mhz e interrupçcão no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura e ativa interrupçcão no TC canal 0 */
	/* Interrupção no C */
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

	/* Inicializa o canal 0 do TC */
	tc_start(TC, TC_CHANNEL);
}

// HANDLERS
// tc0 handler
void TC0_Handler(void){
	volatile uint32_t ul_dummy;
	
	// indicacao de interrupcao
	ul_dummy = tc_get_status(TC0, 0);

	// evita erro ao compilar
	UNUSED(ul_dummy);

	// muda o estado do LED
	tc0_flag = 1;
}

// tc1 handler
void TC1_Handler(void){
	volatile uint32_t ul_dummy;
	
	// indicacao de interrupcao
	ul_dummy = tc_get_status(TC0, 1);

	// evita erro ao compilar
	UNUSED(ul_dummy);

	// muda o estado do LED
	tc1_flag = 1;
}

// tc2 handler
void TC2_Handler(void){
	volatile uint32_t ul_dummy;
	
	// indicacao de interrupcao
	ul_dummy = tc_get_status(TC0, 2);

	// evita erro ao compilar
	UNUSED(ul_dummy);

	// muda o estado do LED
	tc2_flag = 1;
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

	LED_init();
	BUT_init();
	
	// Configure TC's...
	TC_init(TC0, ID_TC0, 0, 5);
	TC_init(TC0, ID_TC1, 1, 10);
	TC_init(TC0, ID_TC2, 2, 1);
	
	int flash_led1 = 0;
	int flash_led2 = 0;
	int flash_led3 = 0;
	
  /* Insert application code here, after the board has been initialized. */
	while(1) {
		if(tc0_flag && flash_led1){
			pin_toggle(LED1_PIO, LED1_IDX_MASK);
			tc0_flag = 0;
		}
		if(tc1_flag && flash_led2){
			pin_toggle(LED2_PIO, LED2_IDX_MASK);
			tc1_flag = 0;
		}
		if(tc2_flag && flash_led3){
			pin_toggle(LED3_PIO, LED3_IDX_MASK);
			tc2_flag = 0;
		}
		if(but1_flag){
			flash_led1 = !flash_led1;
			but1_flag = 0;
		}
		if(but2_flag){
			flash_led2 = !flash_led2;
			but2_flag = 0;
		}
		if(but3_flag){
			flash_led3 = !flash_led3;
			but3_flag = 0;
		}
	}
}
