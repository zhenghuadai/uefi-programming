###################################################################################################
#
# UEFI principles and programming
#
###################################################################################################
[Defines]
  DEFINE UEFI_BOOK_DIR                  = uefi\book

[LibraryClasses]
  sstdLib|$(UEFI_BOOK_DIR)/sstdPkg/Library/sstdLib.inf

[LibraryClasses.common]
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
  FileHandleLib|ShellPkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  SortLib|ShellPkg/Library/UefiSortLib/UefiSortLib.inf
  HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
  
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PathLib|ShellPkg/Library/BasePathLib/BasePathLib.inf

[LibraryClasses.ARM]
  #
  # It is not possible to prevent the ARM compiler for generic intrinsic functions.
  # This library provides the instrinsic functions generate by a given compiler.
  # [LibraryClasses.ARM] and NULL mean link this library into all ARM images.
  #
  #NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf

  # Add support for GCC stack protector
  NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf

[Components]
############### UEFI BOOK ####################################
  #$(UEFI_BOOK_DIR)/CppPkg/Library/CppLib.inf
  #$(UEFI_BOOK_DIR)/CppPkg/test/testcpp.inf
  #$(UEFI_BOOK_DIR)/disk/disk.inf
  $(UEFI_BOOK_DIR)/Event/TestEvent.inf
  # ffmpeg can only be built under LINUX ONLY
  #$(UEFI_BOOK_DIR)/ffmpeg/EFI_FFDECODER_PROTOCOL_1.0/ffdecoder/ffdecoder.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/EFI_FFDECODER_PROTOCOL_1.0/ffplayer/fplayer.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/ffdecoder/ffdecoder.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/ffdecoder/afplayer/afplayer.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/ffplayer/fplayer.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/fplayer/fplayer.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/libavcodec/libavcodec.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/libavformat/libavformat.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/libavutil/libavutil.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/libswscale/libswscale.inf
  #$(UEFI_BOOK_DIR)/ffmpeg/zlib/zlib.inf

  #$(UEFI_BOOK_DIR)/FileIo/FileIo.inf #spec 2.4
  #$(UEFI_BOOK_DIR)/GcppPkg/Library/GcppLib.inf
  #$(UEFI_BOOK_DIR)/GcppPkg/test/testcpp.inf
  $(UEFI_BOOK_DIR)/GUIbasics/String/example.inf
  #$(UEFI_BOOK_DIR)/GUIPkg/Library/UGui.inf
  $(UEFI_BOOK_DIR)/Hob/Hob.inf
  $(UEFI_BOOK_DIR)/HotKey/HotKey.inf
  $(UEFI_BOOK_DIR)/infs/Debug/Main.inf
  $(UEFI_BOOK_DIR)/infs/ShellAppMain/Main.inf
  $(UEFI_BOOK_DIR)/infs/UefiMain/UefiMain.inf
  $(UEFI_BOOK_DIR)/mouse/mouse.inf
  $(UEFI_BOOK_DIR)/multitask/processor/process.inf
  $(UEFI_BOOK_DIR)/Network/tcpsocket.inf
  $(UEFI_BOOK_DIR)/Shell/Execute/Execute.inf
  $(UEFI_BOOK_DIR)/Shell/Execute2/Execute.inf
  $(UEFI_BOOK_DIR)/sstdPkg/Library/sstdLib.inf
  #$(UEFI_BOOK_DIR)/std/main.inf
  $(UEFI_BOOK_DIR)/std/sstd/Testsstd.inf
  $(UEFI_BOOK_DIR)/systemtable/BS/BS.inf
  $(UEFI_BOOK_DIR)/systemtable/gST/ST.inf
  $(UEFI_BOOK_DIR)/systemtable/memory/BSmem.inf
  $(UEFI_BOOK_DIR)/systemtable/ST/ST.inf
  $(UEFI_BOOK_DIR)/thread/setjmp/Main.inf
  $(UEFI_BOOK_DIR)/systemtable/gRT/RT.inf
  $(UEFI_BOOK_DIR)/disk/GPT/TestGpt.inf
  $(UEFI_BOOK_DIR)/disk/DevicePath/TestDevicePath.inf
  $(UEFI_BOOK_DIR)/audio/audio.inf
  #$(UEFI_BOOK_DIR)/audio/testac97.inf

