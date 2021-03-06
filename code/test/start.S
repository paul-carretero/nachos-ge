/* Start.s
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don t want to pull in the entire C library, we define
 *	what we need for a user program here, namely Start and the system
 *	calls.
 */

#include "syscall.h"

        .text
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main".
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
	move	$4,$0
	jal	Exit	 /* if we return from main, exit(0) */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

	.globl Halt
	.ent	Halt
Halt:
	addiu $2,$0,SC_Halt
	syscall
	j	$31
	.end Halt

	.globl Exit
	.ent	Exit
Exit:
	addiu $2,$0,SC_Exit
	syscall
	j	$31
	.end Exit

	.globl Exec
	.ent	Exec
Exec:
	addiu $2,$0,SC_Exec
	syscall
	j	$31
	.end Exec

	.globl Join
	.ent	Join
Join:
	addiu $2,$0,SC_Join
	syscall
	j	$31
	.end Join

	.globl Create
	.ent	Create
Create:
	addiu $2,$0,SC_Create
	syscall
	j	$31
	.end Create

	.globl Open
	.ent	Open
Open:
	addiu $2,$0,SC_Open
	syscall
	j	$31
	.end Open

	.globl Read
	.ent	Read
Read:
	addiu $2,$0,SC_Read
	syscall
	j	$31
	.end Read

	.globl SetCursor
	.ent	SetCursor
SetCursor:
	addiu $2,$0,SC_SetCursor
	syscall
	j	$31
	.end SetCursor

	.globl Write
	.ent	Write
Write:
	addiu $2,$0,SC_Write
	syscall
	j	$31
	.end Write

	.globl RmDir
	.ent	RmDir
RmDir:
	addiu $2,$0,SC_RmDir
	syscall
	j	$31
	.end RmDir

	.globl Remove
	.ent	Remove
Remove:
	addiu $2,$0,SC_Remove
	syscall
	j	$31
	.end Remove

	.globl List
	.ent	List
List:
	addiu $2,$0,SC_List
	syscall
	j	$31
	.end List

	.globl Mkdir
	.ent	Mkdir
Mkdir:
	addiu $2,$0,SC_Mkdir
	syscall
	j	$31
	.end Mkdir

	.globl ChangeDirectory
	.ent	ChangeDirectory
ChangeDirectory:
	addiu $2,$0,SC_ChangeDirectory
	syscall
	j	$31
	.end ChangeDirectory

	.globl Close
	.ent	Close
Close:
	addiu $2,$0,SC_Close
	syscall
	j	$31
	.end Close

	.globl Fork
	.ent	Fork
Fork:
	addiu $2,$0,SC_Fork
	syscall
	j	$31
	.end Fork

	.globl Yield
	.ent	Yield
Yield:
	addiu $2,$0,SC_Yield
	syscall
	j	$31
	.end Yield

	.globl PutChar
	.ent	PutChar
PutChar:
	addiu $2,$0,SC_PutChar
	syscall
	j	$31
	.end PutChar

	.globl PutString
	.ent	PutString
PutString:
	addiu $2,$0,SC_PutString
	syscall
	j	$31
	.end PutString

	.globl GetChar
	.ent	GetChar
GetChar:
	addiu $2,$0,SC_GetChar
	syscall
	j	$31
	.end GetChar

	.globl GetString
	.ent	GetString
GetString:
	addiu $2,$0,SC_GetString
	syscall
	j	$31
	.end GetString

	.globl GetInt
	.ent	GetInt
GetInt:
	addiu $2,$0,SC_GetInt
	syscall
	j	$31
	.end GetInt

	.globl PutInt
	.ent	PutInt
PutInt:
	addiu $2,$0,SC_PutInt
	syscall
	j	$31
	.end PutInt


	.globl UserThreadCreate
	.ent	UserThreadCreate
UserThreadCreate:
	addiu $2,$0,SC_UserThreadCreate /* appel système sur création de thread utilisateur*/
	addiu $6,$0,UserThreadExit /*Passage en paramètre de l addresse du syscall de ret*/
	syscall
	j	$31
	.end UserThreadCreate

	.globl UserThreadExit
	.ent	UserThreadExit
UserThreadExit:
	addiu $2,$0,SC_UserThreadExit
	syscall
	j	$31
	.end UserThreadExit

  .globl UserThreadJoin
	.ent	UserThreadJoin
UserThreadJoin:
	addiu $2,$0,SC_UserThreadJoin
	syscall
	j	$31
	.end UserThreadJoin

  .globl UserSemCreate
	.ent	UserSemCreate
  UserSemCreate:
  	addiu $2,$0,SC_UserSemCreate
  	syscall
  	j	$31
  	.end UserSemCreate

   .globl GetCharInt
	.ent	GetCharInt
  GetCharInt:
  	addiu $2,$0,SC_GetCharInt
  	syscall
  	j	$31
  	.end GetCharInt

  .globl UserSemP
  .ent	UserSemP
  UserSemP:
  	addiu $2,$0,SC_UserSemP
  	syscall
  	j	$31
  	.end UserSemP

  .globl UserSemV
  .ent	UserSemV
  UserSemV:
  	addiu $2,$0,SC_UserSemV
  	syscall
  	j	$31
  	.end UserSemV

  .globl UserSemDelete
  .ent	UserSemDelete
  UserSemDelete:
  	addiu $2,$0,SC_UserSemDelete
  	syscall
  	j	$31
  	.end UserSemDelete

	.globl ForkExec
	.ent	ForkExec
ForkExec:
	addiu $2,$0,SC_ForkExec
	syscall
	j	$31
	.end ForkExec

	.globl SimpleShellProcJoin
	.ent	SimpleShellProcJoin
SimpleShellProcJoin:
	addiu $2,$0,SC_SimpleShellProcJoin
	syscall
	j	$31
	.end SimpleShellProcJoin

  .globl AssertFull
	.ent	AssertFull
AssertFull:
	addiu $2,$0, SC_Assert
	syscall
	j	$31
	.end AssertFull

/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main
