/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

/**
 * COSC 3250 - Project 3
 * Implements kprintf
 * @authors [Noah Kaye; Zach Thompson]
 * Instructor [sabirat]
 * TA-BOT:MAILTO [noah.kaye@marquette.edu; zach.thompson@marquette.edu]
 */

#include <kernel.h>
#include <memory.h>
#include <file.h>



/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.

    //given
    struct dentry *phw = supertab->sb_disk  //found in file.h
    int diskfd = phw - devtab;

    //wait
    //
    wait(supertab->sb_dirlock);

    //error check: dirlist, current file state
    //
	if  ((NULL == supertab) || (NULL == filetab) || (NULL == supertab->sb_dirlist))	{
        signal(supertab->sb_dirlock);
		return SYSERR;
	}
	if(isbadfd(fd))	{
        signal(supertab->sb_dirlock);
		return SYSERR;
	}

    //zero out file values as well as its data
    //
    supertab->sb_dirlst->db_fnodes[fd].fn_state = FILE_FREE;
    supertab->sb_dirlst->db_fnodes[fd].fn_blocknum = 0;
    supertab->sb_dirlst->db_fnodes[fd].fn_cursor = 0;
    supertab->sb_dirlst->db_fnodes[fd].fn_length = 0;
    supertab->sb_dirlst->db_fnodes[fd].fn_name = 0;
    supertab->sb_dirlst->db_fnodes[fd].fn_data = 0;
	filetab[fd].fn_state = FILE_FREE;
    filetab[fd].fn_blocknum = 0;
    filetab[fd].fn_cursor = 0;
    filetab[fd].fn_length = 0;
    filetab[fd].fn_name = 0;
    filetab[fd].fn_data = 0;

    //free up data with sbFreeBlock
    //  error check that that worked
    //
    if(SYSERR == sbFreeBlock(supertab, fd)) {
        signal(supertab->sb_dirlock);
        return SYSERR;
    }

    //rewrite block to end of disk
    //  uses seek and write
    //
    diskSeek(phw, DISKBLOCKLEN);
    if(SYSERR == diskWrite(phw, supertab->sb_dirlst, sizeof(struct dirblock))) {
        signal(supertab->sb_dirlock);
        return SYSERR;
    }

    signal(supertab->sb_dirlock);
    return OK;
}
