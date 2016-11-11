# Filesystem
#Simple File system implementation

Valid Commands list:

   1.  $./a.out /* execute the program */
   2.  myfs> /* prompt given by this program */
   3.  myfs>mkfs<Your Drive Name> <block Size> <Total Size>MB /* creates the filesystem on file <Drive name>, with specified blocksize  */
   4.  myfs>mkfs <Your Drive Name> <Block Size> <Total Size>MB /* creates the filesystem on file <Drive Name>, with specified blocksize */
   5.  myfs>use <Your Drive name> as C: /* the filesystem on <Your Drive name> will henceforth be accessed as C: */
   6.  myfs>use <Your Drive name> as D: /* the filesystem on <Your Drive name> will henceforth be accessed as D: */
   7.  myfs>cp <Source File> C:<dest File> /* copy the file <Source File> from os to the filesystem C: as <Dest File> */
   8.  myfs>ls C: /* see the contents of the filesystem C: */
   9.  myfs>cp C:<Sourece File> C:<Dest File> /* copy the file <Source File> from C: to the filesystem C: as <Dest File> */
  10.  myfs>cp C:<Source File> D:<Dest File> /* copy the file <Source File> from C: to the filesystem D: as <Dest File> */
  11.  myfs>cp D:<Source File> <Dest File> /* copy the file <Source File> from D: to the to the OS as <Dest File> */
  12.  myfs>rm C:<File-Name> /* Delete the <File> from C: */
  13.  myfs>mv D:<Source File> D:<Dest File>  /* Rename  <Source File> of D: to <Dest File> in D: */ 
  14.  myfs>info /*This will show all information about existing file-systems that we have created so far*/
  15.  myfs>clear /*Clear the terminal*/
  16.  myfs>exit /* terminate the process */

#NOTE:
	
	1. After modifying the file system do not terminate the process using [ctrl+c]. Always use "exit" to close the programme.If [ctrl+c] is used then those changes will not be written into the super block. So again when the programme will be run then previous changes will not be retained and the file system will be corrupted.
	2. Valid File System Name is Specified in the source Code (Line 22). Change the list so that other names can also be used. But all the names should be of same format like "<English Letter:>".
