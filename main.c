#define USE_STDPERIPH_DRIVER
#include "stm32f10x.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>

/* Filesystem includes */
#include "filesystem.h"
#include "fio.h"
#include "lib.h"
#include "shell.h"

extern const char _sromfs;

static void setup_hardware();

volatile xSemaphoreHandle serial_tx_wait_sem = NULL;

/*to receive byte*/
volatile xQueueHandle serial_rx_queue = NULL;

/* Queue structure used for messages */
typedef struct {
    char str[100];
} serial_str_msg;

/* Queue structure used for characters */
typedef struct {
    char ch;
} serial_ch_msg;

/* IRQ handler to handle USART2 interruptss (both transmit and receive
 * interrupts). */
void USART2_IRQHandler()
{
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	serial_ch_msg rx_msg;

	/* If this interrupt is for a transmit... */
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
		/* "give" the serial_tx_wait_sem semaphore to notfiy processes
		 * that the buffer has a spot free for the next byte.
		 */
		xSemaphoreGiveFromISR(serial_tx_wait_sem, &xHigherPriorityTaskWoken);

		/* Diables the transmit interrupt. */
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		/* If this interrupt is for a receive... */
	}else if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        /* Receive the byte from the buffer. */
        rx_msg.ch = USART_ReceiveData(USART2);

        /* Queue the received byte. */
        if(!xQueueSendToBackFromISR(serial_rx_queue, &rx_msg, &xHigherPriorityTaskWoken)) {
            /* If there was an error queueing the received byte,
             * freeze. */
            while(1);
        }
    }
	else {
		/* Only transmit and receive interrupts should be enabled.
		 * If this is another type of interrupt, freeze.
		 */
		while(1);
	}

	if (xHigherPriorityTaskWoken) {
		taskYIELD();
	}
}

void send_byte(char ch)
{
	/* Wait until the RS232 port can receive another byte (this semaphore
	 * is "given" by the RS232 port interrupt when the buffer has room for
	 * another byte.
	 */
	while (!xSemaphoreTake(serial_tx_wait_sem, portMAX_DELAY));

	/* Send the byte and enable the transmit interrupt (it is disabled by
	 * the interrupt).
	 */
	USART_SendData(USART2, ch);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

char receive_byte()
{
	serial_ch_msg msg;
	while(!xQueueReceive(serial_rx_queue, &msg, portMAX_DELAY));
	return msg.ch;
}
void readwrite_task(void *pvParameters)
{
	char str[100];
	char ch_buf[2] = {'0','\0'};
	char ch;
	int count_char;
	int done;

	while(1) {
		count_char = 0;
		done = 0;
		do{
			ch = receive_byte();
			if ((ch == '\n') || (ch == '\r')){
				str[count_char++] = '\0';
				print_next_line();
				done = -1;
			}
			else if ((ch == BACKSPACE || ch == '\b') && (count_char != 0)){
				str[count_char--] = '\0';

				/* 1.back to last word
 				   2.replace it with space
				   3.cursor back to the last word*/	
				print_msg("\b \b");
			}
			else if(ch != BACKSPACE){
				str[count_char++] = ch;
				ch_buf[0] = ch;
				print_msg(ch_buf);
			}
		} while (!done);
			check_input(str);
	}
}

int main()
{
	init_rs232();
	enable_rs232_interrupts();
	enable_rs232();
	
	fs_init();
	fio_init();
	
	/* Create the queue used by the serial task.  Messages for write to
	 * the RS232. */
	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_ch_msg));

	/* Create a task to output text read from keyboard input. */
	xTaskCreate(readwrite_task,
	            (signed portCHAR *) "Read & Write",
	            512 /* stack size */, NULL, tskIDLE_PRIORITY + 2, NULL);
	/* Start running the tasks. */
	vTaskStartScheduler();

	return 0;
}

void vApplicationTickHook()
{
}
