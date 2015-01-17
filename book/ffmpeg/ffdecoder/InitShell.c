/*
 * =====================================================================================
 *
 *       Filename:  InitShell.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/18/2012 08:14:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  DAI ZHENGHUA (), djx.zhenghua@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <../Library/UefiShellLib/UefiShellLib.h>

#include  <errno.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>
#include  <MainData.h>
#include  <unistd.h>


extern FILE_HANDLE_FUNCTION_MAP      FileFunctionMap;
EFI_STATUS
EFIAPI
ShellLibConstructorWorker2 (
        void* aEfiShellProtocol,
        void* aEfiShellParametersProtocol,
        void* aEfiShellEnvironment2, 
        void* aEfiShellInterface
  )
{

#if 0
  EFI_STATUS  Status;
    gEfiShellProtocol            = gEfiShellProtocol;            
    gEfiShellParametersProtocol  = gEfiShellParametersProtocol;  
    mEfiShellEnvironment2        = mEfiShellEnvironment2;        
    mEfiShellInterface           = mEfiShellInterface;           
#endif
  //
  // UEFI 2.0 shell interfaces (used preferentially)
  //
  
  //
  // only success getting 2 of either the old or new, but no 1/2 and 1/2
  //
  //if ((mEfiShellEnvironment2 != NULL && mEfiShellInterface          != NULL) ||
  //    (gEfiShellProtocol     != NULL && gEfiShellParametersProtocol != NULL)   ) 
  {
//    if (0 && (gEfiShellProtocol != NULL)) {
        if(0){
      FileFunctionMap.GetFileInfo     = gEfiShellProtocol->GetFileInfo;
      FileFunctionMap.SetFileInfo     = gEfiShellProtocol->SetFileInfo;
      FileFunctionMap.ReadFile        = gEfiShellProtocol->ReadFile;
      FileFunctionMap.WriteFile       = gEfiShellProtocol->WriteFile;
      FileFunctionMap.CloseFile       = gEfiShellProtocol->CloseFile;
      FileFunctionMap.DeleteFile      = gEfiShellProtocol->DeleteFile;
      FileFunctionMap.GetFilePosition = gEfiShellProtocol->GetFilePosition;
      FileFunctionMap.SetFilePosition = gEfiShellProtocol->SetFilePosition;
      FileFunctionMap.FlushFile       = gEfiShellProtocol->FlushFile;
      FileFunctionMap.GetFileSize     = gEfiShellProtocol->GetFileSize;
    } else {
      FileFunctionMap.GetFileInfo     = (EFI_SHELL_GET_FILE_INFO)FileHandleGetInfo;
      FileFunctionMap.SetFileInfo     = (EFI_SHELL_SET_FILE_INFO)FileHandleSetInfo;
      FileFunctionMap.ReadFile        = (EFI_SHELL_READ_FILE)FileHandleRead;
      FileFunctionMap.WriteFile       = (EFI_SHELL_WRITE_FILE)FileHandleWrite;
      FileFunctionMap.CloseFile       = (EFI_SHELL_CLOSE_FILE)FileHandleClose;
      FileFunctionMap.DeleteFile      = (EFI_SHELL_DELETE_FILE)FileHandleDelete;
      FileFunctionMap.GetFilePosition = (EFI_SHELL_GET_FILE_POSITION)FileHandleGetPosition;
      FileFunctionMap.SetFilePosition = (EFI_SHELL_SET_FILE_POSITION)FileHandleSetPosition;
      FileFunctionMap.FlushFile       = (EFI_SHELL_FLUSH_FILE)FileHandleFlush;
      FileFunctionMap.GetFileSize     = (EFI_SHELL_GET_FILE_SIZE)FileHandleGetSize;
    }
    return (EFI_SUCCESS);
  }
  return (EFI_NOT_FOUND);
}

INTN
EFIAPI
DriverInitMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  struct __filedes   *mfd;
  //char              **nArgv;
  INTN   ExitVal;
  int                 i;

  ExitVal = (INTN)RETURN_SUCCESS;
  gMD = AllocateZeroPool(sizeof(struct __MainData));
  if( gMD == NULL ) {
    ExitVal = (INTN)RETURN_OUT_OF_RESOURCES;
  }
  else {
    /* Initialize data */
      extern   int __sse2_available;
    __sse2_available      = 0;
    _fltused              = 1;
    errno                 = 0;
    EFIerrno              = 0;

    gMD->ClocksPerSecond  = 1;
    gMD->AppStartTime     = (clock_t)((UINT32)time(NULL));

    // Initialize file descriptors
    mfd = gMD->fdarray;
    for(i = 0; i < (FOPEN_MAX); ++i) {
      mfd[i].MyFD = (UINT16)i;
    }

    i = open("stdin:", O_RDONLY, 0444);
    if(i == 0) {
      i = open("stdout:", O_WRONLY, 0222);
      if(i == 1) {
        i = open("stderr:", O_WRONLY, 0222);
      }
    }
    if(i != 2) {
      Print(L"ERROR Initializing Standard IO: %a.\n    %r\n",
            strerror(errno), EFIerrno);
    }

    /* Create mbcs versions of the Argv strings. */
#if 0
    nArgv = ArgvConvert(Argc, Argv);
    if(nArgv == NULL) {
      ExitVal = (INTN)RETURN_INVALID_PARAMETER;
    }
    else {
      if( setjmp(gMD->MainExit) == 0) {
        ExitVal = (INTN)main( (int)Argc, gMD->NArgV);
        exitCleanup(ExitVal);
      }
      /* You reach here if:
          * normal return from main()
          * call to _Exit(), either directly or through exit().
      */
      ExitVal = (INTN)gMD->ExitValue;
    }
#endif

#if 0
    if( ExitVal == EXIT_FAILURE) {
      ExitVal = RETURN_ABORTED;
    }

    /* Close any open files */
    for(i = OPEN_MAX - 1; i >= 0; --i) {
      (void)close(i);   // Close properly handles closing a closed file.
    }

    /* Free the global MainData structure */
    if(gMD != NULL) {
      if(gMD->NCmdLine != NULL) {
        FreePool( gMD->NCmdLine );
      }
      FreePool( gMD );
  }
#endif
  }
  return ExitVal;
}
