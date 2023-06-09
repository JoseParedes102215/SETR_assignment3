
/**
\author José Paredes Manuel Miranda
\date 16 May 2023
*/

/** 
\file vending_machine.c
\brief Vending Machine State Machine
 */



#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api*/
#include <zephyr/sys/printk.h>      /* for printk()*/
#include <stdio.h>
#include <stdlib.h>

#define SLEEP_TIME_MS   10 

/**
\brief Define a structure for a movie session.
 */

typedef struct  {
  char name[20];/**< Name of the movie */
  char time[10];/**< Time of the movie session */
  int price;/**< Price of the movie session */
} MovieSession ;

/**
\brief Define the list of movie sessions.
 */
    MovieSession movies[5] = {
  {"Movie A", "19H00", 9},
  {"Movie A", "21H00", 11},
  {"Movie A", "23H00", 9},
  {"Movie B", "19H00", 10},
  {"Movie B", "21H00", 12}
};

// define the current credit of the system
int credit = 0;/**< Current credit of the system */
int up_down;/**< Variable to navigate movie sessions */
int flag_movie = 0;/**< Flag to indicate if movie browsing is active */

/**
\brief Define the states of the state machine.


 */
typedef enum {
  START,            /**< Start state */
  INSERT_COIN,      /**< Insert coin state */
  BROWSE,           /**< Browse state */
  SELECT_MOVIE,     /**< Select movie state */
  RETURN_CREDIT     /**< Return credit state */
} State;


/**
\brief Define the events that can trigger state transitions.


 */
typedef enum {
  NO_EVENT,         /**< No event */
  COIN_1_EUR,       /**< 1 EUR coin event */
  COIN_2_EUR,       /**< 2 EUR coin event */
  COIN_5_EUR,       /**< 5 EUR coin event */
  COIN_10_EUR,      /**< 10 EUR coin event */
  BROWSE_UP,        /**< Browse up event */
  BROWSE_DOWN,      /**< Browse down event */
  SELECT,           /**< Select event */
  RETURN            /**< Return event */
} Event;



/**
\brief Define the structure for the state machine information.


 */
typedef struct {
  State current_state; /**< Current state of the state machine */
  State initial_state; /**< Initial state of the state machine */
  Event last_event;    /**< Last event triggered */
} State_Machine_info;


State_Machine_info Vending_machine;


/**
\brief Initialize the state machine.


 */
void sm_init(){
    Vending_machine.current_state = START;
    Vending_machine.initial_state = START;
    Vending_machine.last_event = NO_EVENT;

}


/**
\brief Reset the state machine to its initial state.


 */
void sm_reset(){
    Vending_machine.initial_state = START;
}


/**
\brief Get the current state of the state machine.


\return The current state.
 */
State sm_get_current_state(){
    return Vending_machine.current_state;
}



/**
\brief Send an event to the state machine.


\param event The event to be sent.
 */
void sm_send_event(Event event){
    Vending_machine.last_event = event;
}


/**
\brief Execute the state machine logic based on the current state and last event.


 */
void sm_execute(){


    switch (Vending_machine.current_state)
    {
    case START:
        if(Vending_machine.last_event == COIN_1_EUR ||  Vending_machine.last_event == COIN_2_EUR || Vending_machine.last_event == COIN_5_EUR || Vending_machine.last_event == COIN_10_EUR){
			Vending_machine.current_state = INSERT_COIN;
		}
		else if(Vending_machine.last_event == BROWSE_UP || Vending_machine.last_event == BROWSE_DOWN){
			flag_movie = 1;
			Vending_machine.current_state = BROWSE;
		}
        break;
    case INSERT_COIN:
		if(Vending_machine.last_event == COIN_1_EUR){
			credit += 1;
			Vending_machine.last_event = NO_EVENT;
		}
		else if(Vending_machine.last_event == COIN_2_EUR){
			credit += 2;
			Vending_machine.last_event = NO_EVENT;
		}
		else if(Vending_machine.last_event == COIN_5_EUR){
			credit += 5;
			Vending_machine.last_event = NO_EVENT;
		}
		else if(Vending_machine.last_event == COIN_10_EUR){
			credit += 10;
			Vending_machine.last_event = NO_EVENT;
		}
		else if(Vending_machine.last_event == RETURN){
			printk( "%d EUR return                                                                                 \n",credit);
			credit = 0;
			Vending_machine.last_event = NO_EVENT;
		}
		else if(Vending_machine.last_event == BROWSE_UP || Vending_machine.last_event == BROWSE_DOWN ){
			flag_movie = 1;
			Vending_machine.current_state = BROWSE;
		}
		printk("credito = %d                                                                                     \r",credit);
		
        break;
    case BROWSE:

			if(Vending_machine.last_event == BROWSE_UP){

				if(flag_movie == 1){
					printk("Name: %s , schedule: %s price: %d , available credit: %d           \r",movies[up_down].name,movies[up_down].time, movies[up_down].price ,credit);
					flag_movie = 0;
				}
				else{
					up_down++;
					if(up_down == 5){
						up_down = 0;
						printk("Name: %s , schedule: %s price: %d , available credit: %d           \r",movies[up_down].name,movies[up_down].time, movies[up_down].price ,credit);
					}
					else{
						printk("Name: %s , schedule: %s price: %d , available credit: %d             \r",movies[up_down].name,movies[up_down].time, movies[up_down].price ,credit);
					}
				}
				Vending_machine.last_event = NO_EVENT;
			}
			if(Vending_machine.last_event == BROWSE_DOWN){
				if(flag_movie == 1){
					printk("Name: %s , schedule: %s price: %d , available credit: %d           \r",movies[up_down].name,movies[up_down].time, movies[up_down].price ,credit);
					flag_movie = 0;
				}
				else{
					up_down--;
					if(up_down == -1){
						up_down = 4;
						printk("Name: %s , schedule: %s price: %d , available credit: %d           \r",movies[up_down].name,movies[up_down].time, movies[up_down].price ,credit);
					}
					else{
						printk("Name: %s , schedule: %s price: %d , available credit: %d             \r",movies[up_down].name,movies[up_down].time, movies[up_down].price ,credit);
					}
				}
				Vending_machine.last_event = NO_EVENT;
			}

			else if(Vending_machine.last_event == RETURN){
				printk( "%d EUR return                                                                                       \n",credit);
				credit = 0;
				printk("Name: %s , schedule: %s price: %d , available credit: %d                                        \r",movies[up_down].name,movies[up_down].time, movies[up_down].price ,credit);
				Vending_machine.last_event = NO_EVENT;
			}

			else if(Vending_machine.last_event == SELECT){
				if(credit >= movies[up_down].price){
					credit -= movies[up_down].price;
					printk("Ticket for %s, session %s issued. Remaining credit: %d                                         \r",movies[up_down].name,movies[up_down].time,credit);
					Vending_machine.last_event = NO_EVENT;
				}
				else if(credit < movies[up_down].price){
				printk("Not enough credit. Ticket not issued                                                                   \r");
				Vending_machine.last_event = NO_EVENT;

				}
			}
			else if(Vending_machine.last_event == COIN_1_EUR ||  Vending_machine.last_event == COIN_2_EUR || Vending_machine.last_event == COIN_5_EUR || Vending_machine.last_event == COIN_10_EUR){
				Vending_machine.current_state = INSERT_COIN;
			}

        break;

    default:
        break;
    }
}  




/* Set the pins used for buttons */
/* buttons 5-8 are connected to pins labeled A0 ... A3 (gpio0 pins 3,4,28,29) */
const uint8_t buttons_pins[] = {11,12,24,25,3,4,28,29}; /* vector with pins where buttons are connected */
	
/* Get node ID for GPIO0, which has leds and buttons */ 
#define GPIO0_NODE DT_NODELABEL(gpio0)

/* Now get the device pointer for GPIO0 */
static const struct device * gpio0_dev = DEVICE_DT_GET(GPIO0_NODE);

/* Define a variable of type static struct gpio_callback, which will latter be used to install the callback
*  It defines e.g. which pin triggers the callback and the address of the function */
static struct gpio_callback button_cb_data;

/**
\brief Callback function triggered when a button is pressed.
\param dev The GPIO device.
\param cb The GPIO callback structure.
\param pins The bitmask of the pins triggered.
 */
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int i=0;
	int val;

	/* Identify the button(s) that was(ere) hit*/
	for(i=0; i<sizeof(buttons_pins); i++){		
		if(BIT(buttons_pins[i]) & pins) {
			//printk("Button %d pressed\n",i+1);
			val = i + 1;
			}
		} 
		 	switch(val){
				case 1:
					Vending_machine.last_event = COIN_1_EUR;
					break;
					

				case 2:
					Vending_machine.last_event = COIN_2_EUR;
					break;

				case 3:
					Vending_machine.last_event = COIN_5_EUR;
					break;

				case 4:
					Vending_machine.last_event = COIN_10_EUR;
					break; 
				 case 5:
					Vending_machine.last_event = BROWSE_UP;
					break;

				case 6:
					Vending_machine.last_event = BROWSE_DOWN;
					break;
				 case 7:
					Vending_machine.last_event = SELECT;
					break;
					

				case 8:
					Vending_machine.last_event = RETURN;
					break;   

				default:
				break;
	}

}

/**
\brief Main function of the vending machine program.
\return The exit status.
 */

int main(){
	
	int ret;
	uint32_t pinmask = 0;
    sm_init();   

	/* Welcome message */
	printk("Digital IO accessing IO pins not set via DT (external buttons in the case) \n\r");
	printk("Hit buttons 1-8 (1...4 internal, 5-8 external connected to A0...A3). Led toggles and button ID printed at console \n\r");

	/* Check if gpio0 device is ready */
	if (!device_is_ready(gpio0_dev)) {
		printk("Error: gpio0 device is not ready\n");
		return;
	} else {
		printk("Success: gpio0 device is ready\n");
	}


	for(int i=0; i<sizeof(buttons_pins); i++) {
		ret = gpio_pin_configure(gpio0_dev, buttons_pins[i], GPIO_INPUT | GPIO_PULL_UP);
		if (ret < 0) {
			printk("Error: gpio_pin_configure failed for button %d/pin %d, error:%d\n\r", i+1,buttons_pins[i], ret);
			return;
		} else {
			printk("Success: gpio_pin_configure for button %d/pin %d\n\r", i+1,buttons_pins[i]);
		}
	}

	/* Configure the interrupt on the button's pin */
	for(int i=0; i<sizeof(buttons_pins); i++) {
		ret = gpio_pin_interrupt_configure(gpio0_dev, buttons_pins[i], GPIO_INT_EDGE_TO_ACTIVE );
		if (ret < 0) {
			printk("Error: gpio_pin_interrupt_configure failed for button %d / pin %d, error:%d", i+1, buttons_pins[i], ret);
			return;
		}
	}

	/* HW init done!*/
	printk("All devices initialized sucesfully!\n\r");
    
	/* Initialize the static struct gpio_callback variable   */
	pinmask=0;
	for(int i=0; i<sizeof(buttons_pins); i++) {
		pinmask |= BIT(buttons_pins[i]);
	}
    gpio_init_callback(&button_cb_data, button_pressed, pinmask); 	
	
	/* Add the callback function by calling gpio_add_callback()   */
	gpio_add_callback(gpio0_dev, &button_cb_data); 

	while(1){
		/* Just sleep. Led on/off is done by the int callback */
		sm_execute(); 
		k_msleep(SLEEP_TIME_MS);

	}


   // libera a memória alocada para a estrutura State_Machine_info
    return 0;
}