/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO 
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <xinu.h>

/* Function prototypes */
void print_sem(semaphore sem);
uchar getc(void);
void putc(uchar);


/* Test process to use for testing semaphores. */
process testSemWait(semaphore sem)
{
    wait(sem);
    printf("process %d after wait()\r\n", currpid[getcpuid()]);
    return 0;
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    uchar c;
    semaphore testsem;

    enable();

    printf("===TEST BEGIN===\r\n");
    printf("0) wait test\r\n");
    printf("1) getc test\r\n");
    printf("2) putc test\r\n");
    printf("3) wait test with signal\r\n");
    printf("4) semfree test\r\n");
    
	// TODO: Test your operating system!
    c = getc();
    switch (c)
    {
    case '0':
        testsem = semcreate(0);
        printf("test 0\r\n");
        testSemWait(testsem);
        print_sem(testsem);
        break;
    case '1':
        printf("test 1\r\n");
        printf("Enter a character\r\n");
        c = getc();
        printf("You typed: %c\r\n", c);
        break;
    case '2':
        printf("test 2\r\n");
        printf("putting 'A'\r\n");
        putc('A');
        printf("\r\n");
        break;
    case '3':
        testsem = semcreate(0);
        signal(testsem);
        printf("test 3\r\n");
        print_sem(testsem);
        testSemWait(testsem);
        print_sem(testsem);
        semfree(testsem);
        break;
    case '4':
        printf("test4\r\n");
        testsem = semcreate(0);
        print_sem(testsem);
        printf("freeing semaphore\r\n");
        semfree(testsem);
        print_sem(testsem);
        break;
    default:
        break;
    }

    // testing new version control system
    while (numproc > 5)  // Four null procs plus this test proc.
    {
        resched();
    }

    printf("\r\n===TEST END===\r\n");
    printf("\r\n\r\nAll user processes have completed.\r\n\r\n");

    while (1);
    return;
}

void print_sem(semaphore sem)
{
    struct sement *semptr;
    semptr = &semtab[sem];

    printf("\r\n%d->state: %s\r\n", sem,
           (semptr->state == 0x01) ? "SFREE" : "SUSED");
    printf("%d->count: %d\r\n", sem, semptr->count);
    printf("%d->queue: %s\r\n\r\n", sem,
           (isempty(semptr->queue)) ? "EMPTY" : "NONEMPTY");
}
