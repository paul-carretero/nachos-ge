// exception.cc
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

/**
 * \file exception.cc
 * \brief Gestion des syscalls
*/
#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"
#include "usersemaphore.h"
#include "synch.h"
#include "IziAssert.h"
#include "fork.h"


//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
  int pc = machine->ReadRegister (PCReg);
  machine->WriteRegister (PrevPCReg, pc);
  pc = machine->ReadRegister (NextPCReg);
  machine->WriteRegister (PCReg, pc);
  pc += 4;
  machine->WriteRegister (NextPCReg, pc);
}


void resetbuffer(char * buffer, int buffersize){
  int i;
  for (i = 0; i < buffersize; i++){
    buffer[i] = '\0';
  }
}
//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
  int type = machine->ReadRegister (2);

  if(which == SyscallException)
  {
    switch(type)
    {
      case SC_Exit:
      {
        ProcessExit();
        break;
      }
      case SC_Halt:
      {
        scheduler->Print();
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
        break;
      }
      case SC_PutChar:
      {
        char arg = (char)machine->ReadRegister (4);
        DEBUG('a', "Appel systeme PutChar réalisé\n");
        synchconsole->SynchPutChar(arg);
        break;
      }
      case SC_PutString:
      {
        // lecture de l'adresse de la chaine de caractères MIPS
        int arg = machine->ReadRegister (4);

        // Buffer de la chaine de caractères LINUX (initialement vide et de taille maximale)
        char buffer[MAX_STRING_SIZE];

        // Conversion String MIPS --> String LINUX
        synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);

        synchconsole->SynchPutString(buffer);
        DEBUG('a', "Appel systeme PutString réalisé\n");
        break;
      }
      case SC_GetString:
      {
        int result = machine->ReadRegister(4);
        int maxStringSize = machine->ReadRegister(5);
        char buffer[maxStringSize];
        synchconsole->SynchGetString(buffer, maxStringSize);
        synchconsole->copyMachineFromString(buffer, result, maxStringSize);
        machine->WriteRegister(2, result);

        DEBUG('a', "Appel systeme GetString réalisé\n");
        break;
      }
      case SC_GetInt:
      {
        int addr = machine->ReadRegister(4);
        int n;
        //synchconsole->copyMachineFromInt(&n, result);
        synchconsole->SynchGetInt(&n);
        machine->WriteMem(addr, 1, n);
        DEBUG('a', "Appel systeme GetInt réalisé\n");
        break;
      }
      case SC_PutInt:
      {
        int n = machine->ReadRegister(4);
        //synchconsole->copyIntFromMachine(arg, &n);
        synchconsole->SynchPutInt(n);
        DEBUG('a', "Appel systeme PutInt réalisé\n");
        break;
      }
      case SC_UserThreadCreate:
      {
        struct argRetparams* params = new struct argRetparams;
        int f = machine->ReadRegister(4);
        params->arg = machine->ReadRegister(5);
        params->retaddr = machine->ReadRegister(6);
        machine->WriteRegister(2, do_UserThreadCreate(f, (int) params));
        delete params;
        DEBUG('a', "Appel systeme SC_UserThreadCreate réalisé\n");
        break;
      }
      case SC_UserThreadExit:
      {
        do_UserThreadExit();
        DEBUG('a', "Appel systeme SC_UserThreadExit réalisé\n");
        break;
      }
      case SC_UserThreadJoin:
      {
        int tid = machine->ReadRegister(4);
        machine->WriteRegister(2,do_UserThreadJoin(tid));
        DEBUG('a', "Appel systeme SC_UserThreadJoin réalisé\n");
        break;
      }
      case SC_UserSemCreate:
      {
        int arg = machine->ReadRegister(4);
        int arg2 = machine->ReadRegister(5);
        char f[MAX_SEM_NAME_SIZE];
        synchconsole->copyStringFromMachine(arg, f, MAX_SEM_NAME_SIZE);
        machine->WriteRegister(2,(int) do_UserSemCreate(f,arg2));
        DEBUG('a', "Appel systeme SC_UserSemCreate réalisé\n");
        break;
      }
      case SC_GetCharInt:
      {
        int ch = (int)synchconsole->SynchGetChar();
        machine->WriteRegister(2,ch);
        DEBUG('a', "Appel systeme SC_GetCharInt réalisé\n");
        break;
      }
      case SC_GetChar:
      {
        int ch = (int)synchconsole->SynchGetChar();
        machine->WriteRegister(2,ch);
        DEBUG('a', "Appel systeme SC_GetChar réalisé\n");
        break;
      }
      case SC_UserSemP:
      {
        Semaphore* f = (Semaphore *) machine->ReadRegister(4);
        do_UserSemP(f);
        DEBUG('a', "Appel systeme SC_SemP réalisé\n");
        break;
      }
      case SC_UserSemV:
      {
        Semaphore* f = (Semaphore *)machine->ReadRegister(4);
        do_UserSemV(f);
        DEBUG('a', "Appel systeme SC_UserSemV réalisé\n");
        break;
      }
      case SC_UserSemDelete:
      {
        Semaphore* f = (Semaphore *)machine->ReadRegister(4);
        do_UserSemDelete(f);
        DEBUG('a', "Appel systeme SC_UserSemDelete réalisé\n");
        break;
      }
      case SC_ForkExec:
      {
        // lecture de l'adresse de la chaine de caractères MIPS
        int arg = machine->ReadRegister (4);

        // Buffer de la chaine de caractères LINUX (initialement vide et de taille maximale)
        char buffer[MAX_STRING_SIZE];

        // Conversion String MIPS --> String LINUX
        synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
        ForkExec(buffer);
        DEBUG('a', "Appel systeme SC_ForkExec réalisé\n");
        break;
      }
      case SC_Assert:
      {
        int res = machine->ReadRegister(4);
        int res2 = machine->ReadRegister(5);
        char res2str[MAX_STRING_SIZE];
        synchconsole->copyStringFromMachine(res2, res2str, MAX_STRING_SIZE);
        int res3 = machine->ReadRegister(6);
        int res4 = machine->ReadRegister(7);
        char res4str[MAX_STRING_SIZE];
        synchconsole->copyStringFromMachine(res4, res4str, MAX_STRING_SIZE);
        iziAssert(res, res2str, res3, res4str);
        break;
      }
      case SC_SimpleShellProcJoin:
      {
        Thread::ShellProcOnlyOne->P();
        Thread::ShellProcOnlyOne->P();
        DEBUG('a', "Appel systeme SC_SimpleShellProcJoin réalisé\n");
        break;
      }
      case SC_List:
      {
        #ifdef FILESYS
        fileSystem->List();
        #endif
        DEBUG('a', "Appel systeme SC_List réalisé\n");
        break;
      }
      case SC_Create:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
          fileSystem->Create (buffer, 3000,FileHeader::f);
        #endif
        
        DEBUG('a', "Appel systeme SC_Create réalisé\n");
        break;
      }
      case SC_Mkdir:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
          fileSystem->CreateDir (buffer);
        #endif
        
        DEBUG('a', "Appel systeme SC_Mkdir réalisé\n");
        break;
      }
      case SC_Remove:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
          fileSystem->Remove (buffer);
        #endif
        
        DEBUG('a', "Appel systeme SC_Mkdir réalisé\n");
        break;
      }
      case SC_RmDir:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
          fileSystem->DeleteDir (buffer);
        #endif
        
        DEBUG('a', "Appel systeme SC_Mkdir réalisé\n");
        break;
      }
      case SC_ChangeDirectory:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
          int res = fileSystem->ChangeDirPath (buffer);
          machine->WriteRegister(2,res);
        #endif
        
        DEBUG('a', "Appel systeme SC_ChangeDirectory réalisé\n");
        break;
      }
      case SC_Open:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
          int res = (int)fileSystem->Open (buffer);
          machine->WriteRegister(2,res);
        #endif
        
        DEBUG('a', "Appel systeme SC_open réalisé\n");
        break;
      }
      case SC_Close:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, MAX_STRING_SIZE);
          fileSystem->Close (buffer);
        #endif
        
        DEBUG('a', "Appel systeme SC_Close réalisé\n");
        break;
      }
      case SC_Read:
      {
        #ifdef FILESYS
          int returnBuffer = machine->ReadRegister (4);
          int size = machine->ReadRegister (5);
          OpenFile * OFile = (OpenFile *)machine->ReadRegister (6);
          
          char buffer[MAX_STRING_SIZE];
          resetbuffer(buffer,MAX_STRING_SIZE);

          int res = OFile->Read (buffer,size);
          synchconsole->copyMachineFromString(buffer, returnBuffer, size);
          machine->WriteRegister(2,res);
        #endif
        
        DEBUG('a', "Appel systeme SC_read réalisé\n");
        break;
      }
      case SC_Write:
      {
        #ifdef FILESYS
          int arg = machine->ReadRegister (4);
          int size = machine->ReadRegister (5);
          OpenFile * OFile = (OpenFile *)machine->ReadRegister (6);
          char buffer[MAX_STRING_SIZE];
          synchconsole->copyStringFromMachine(arg, buffer, size);
          int res = (int)(OFile->Write (buffer,size));
          machine->WriteRegister(2,res);
        #endif
        
        DEBUG('a', "Appel systeme SC_Write réalisé\n");
        break;
      }
      case SC_SetCursor:
      {
        #ifdef FILESYS
          OpenFile * OFile = (OpenFile *)machine->ReadRegister (4);
          int pos = machine->ReadRegister (5);
          OFile->Seek (pos);
        #endif
        
        DEBUG('a', "Appel systeme SC_Write réalisé\n");
        break;
      }
      default:
      {

        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
        break;
      }
    }
  }
    // LB: Do not forget to increment the pc before returning!
  UpdatePC ();
    // End of addition
}
