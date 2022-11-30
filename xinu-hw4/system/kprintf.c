/**
 * @file kprintf.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu zach.thompson@marquette.edu]
 */

#include <xinu.h>

#define UNGETMAX 10             /* Can un-get at most 10 characters. */

static unsigned char ungetArray[UNGETMAX];
static unsigned int bufCount = 0;

/**
 * Synchronously read a character from a UART.  This blocks until a character is
 * available.  The interrupt handler is not used.
 *
 * @return
 *      The character read from the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kgetc(void) // i was here
{
    volatile struct pl011_uart_csreg *regptr;
    uchar c;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x3F201000;

    // TODO: First, check the unget buffer for a character.
    //       Otherwise, check UART flags register, and
    //       once the receiver is not empty, get character c.
	//hint in lab: get char in and send to computer. get it from register or buffer
	//when register is empty then line like 51?
    //ungetArray
    //otherwise fr & regptr->PL011_FR_RXFE
    //dr
    if(bufCount > 0) {
        return ungetArray[--bufCount];
    }
    
    while(regptr->fr & PL011_FR_RXFE) {

    }

    return regptr->dr;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    regptr = (struct pl011_uart_csreg *)0x3F201000;

    // TODO: Check the unget buffer and the UART for characters.
	//hint in lab: if anything in these then do something if else do a diff thing

    if(bufCount <= 0 && regptr->fr & PL011_FR_RXFF) {
        return 0;
    }else {
        return 1;
    }
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
    // TODO: Check for room in unget buffer, put the character in or discard.
	//hint in lab: create global array

    if(bufCount < UNGETMAX) {
        ungetArray[bufCount++] = c;

        return c;
    }

    return SYSERR;
}


/**
 * Synchronously write a character to a UART.  This blocks until the character
 * has been written to the hardware.  The interrupt handler is not used.
 *
 * @param c
 *      The character to write.
 *
 * @return
 *      The character written to the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kputc(uchar c)
{
    volatile struct pl011_uart_csreg *regptr;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x3F201000;

	/*stuff from lab
	//regptr->fr;
	//regptr->dr;
	
	//regptr->fr & FLAG;
	*/

    // TODO: Check UART flags register.
    //       Once the Transmitter FIFO is not full, send character c.
    //while(regptr->fr & PL011_FR_TXFF)
    //regptr->dr=c;
    while(regptr->fr & PL011_FR_TXFF) {
    }

    regptr->dr = c;
    return (int)c;
}

/**
 * kernel printf: formatted, synchronous output to SERIAL0.
 *
 * @param format
 *      The format string.  Not all standard format specifiers are supported by
 *      this implementation.  See _doprnt() for a description of supported
 *      conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
syscall kprintf(const char *format, ...)
{
    //somewhere in kprintf call lock_acquire(parameter)
    //look in spinlock.h for parameter
    //then all the code
    //after code
    //lock_release(parameter)
    //again check spinlock.h for parameter

    int retval;
    va_list ap;

    lock_acquire(serial_lock);
    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(int, int))kputc, 0);
    va_end(ap);
    lock_release(serial_lock);
    return retval;
}
