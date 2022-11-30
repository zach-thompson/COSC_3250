/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 *
 * and
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

/**
 * COSC 3250 - Project 3
 * tests kprintf methods.
 * @authors [Noah Kaye Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu zach.thompson@marquette.edu]
 */


#include <xinu.h>

devcall putc(int dev, char c) { return 0; }


/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;

    kprintf("===TEST BEGIN===\r\n");
    //kputc must work if kgetc works
    //kgetc works
    c = kgetc();
    switch (c)
    {
    // TODO: Test your operating system!

    //kcheckc 
    case '2': 
    if(kungetc('1') == '1'){}
    if(kcheckc() == 1) {
        kprintf("kcheckc test passed\n");
    }else {
        kprintf("kcheckc test failed\n");
    }
    break;

    //kungetc
    case '1':
    if(kungetc('1') == '1') {
	    kprintf("kungetc test passed\n");
    }else {
        kprintf("kungetc test failed\n");
    }
    break;

    //putc longstring test
    case '9':
    kprintf("This is a line that tests to see if kprintf can print more than 64 characters in one line in a single kprintf statement.");
    break;

    default:
	kprintf("Hello Xinu World!\r\n");
    }


    kprintf("\r\n===TEST END===\r\n");
    return;
}
