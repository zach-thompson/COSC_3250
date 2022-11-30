/**
 * @file printf.c
 */
/* Embedded Xinu, Copyright (C) 2018. All rights reserved. */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <xinu.h>

extern struct uart serial_port;


/**
 * Asynchrounously read a character from the UART.
 *
 * @return
 *      The character read from the UART.
 */
uchar getc(void)
{
	uchar c;
	irqmask im = disable();

	/* TODO:
	 * Asynchronously read a character from the UART.
	 	- call wait(serial_port.isema) (.i for getc, .o for putc)
	 * Wait for input using the appropriate semaphore.
	 * Store the first byte in the input buffer (as a character) to later be returned.
	 *  - c = serial_port.in[serial_port.istart]
	 * Then, decrement the counter of bytes in the input buffer.
	 * 	- serial_port.icount--
	 * Increment the index of the input buffer's first byte
	 * 	- serial_port.istart = (serial_port.istart + 1) % UART_IBLEN
	 * with respect to the total length of the UART input buffer.
	 */

	// wait for input
	wait(serial_port.isema);

	// store the first byte from input buffer
	c = serial_port.in[serial_port.istart];

	// decrement counter
	serial_port.icount--;

	// Increment index of buffer's first byte
	serial_port.istart = (serial_port.istart + 1) % UART_IBLEN;

	
	restore(im);
	return c;
}


/**
 * Asynchronously write a character to the UART.
 *
 * @param c
 *      The character to write.
 *
 * @return OK on success
 */
syscall putc(char c)
{
	irqmask im = disable();
	
	/* TODO:
	 * First, check if the UART transmitter is idle (see include/uart.h for UART control block information).
	 *	- if (serial_port.oidle == 1)
	 * If it is idle, set its state to busy (hint: not idle), then write the character to the
	 * 	- serial_port.oidle = 0;
	 * UART control and status register's data register.
	 * 	- 
	 * If it is not idle, use the appropriate UART semaphore to wait for the UART's buffer space to be free.
	 * 	- wait(serial_port.osema)
	 * Next, guard the following operation with a spin lock:
	 * 	- serial_port.olock
	 * Place the character into the UART output buffer with respect to the index of the buffer's first byte,
	 * amount of bytes in buffer, and total length of the UART output buffer.
	 * 	- serial_port.out = serial_port.ocount + 1 % oblen
	 * Then, increment the counter of bytes in the output buffer. Release the spinlock.
	 * 	- serial_port.count++, 
	 */

	if (serial_port.oidle == 1) { // is idle
		serial_port.oidle = 0;

		((struct pl011_uart_csreg *)serial_port.csr)->dr = c;
		//regptr->cr = c;
		//serial_port.csr = c;
		//regptr->dr = c;		
	}
	else {
		wait(serial_port.osema);
		
		lock_acquire(serial_port.olock);

		serial_port.out[(serial_port.ocount + serial_port.ostart) % UART_OBLEN] = c;
		serial_port.ocount++;
		lock_release(serial_port.olock);
		
	}

	/*lock_acquire(serial_port.olock);
	serial_port.out[(serial_port.ocount + serial_port.ostart) % UART_OBLEN] = c;
	serial_port.ocount++;
	lock_release(serial_port.olock);*/


	restore(im);
	return OK;
}


/**
 * printf: formatted, asynchronous output to UART device.
 *
 * @param format
 *      The format string.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
syscall printf(const char *format, ...)
{
	int retval;

	va_list ap;
	va_start(ap, format);
	retval = _doprnt(format, ap, (int (*)(int, int))putc, 0);
	va_end(ap);

	return retval;
}
