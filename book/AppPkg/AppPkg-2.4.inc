###################################################################################################
#
# UEFI principles and programming
#
###################################################################################################
[defines]
  DEFINE UEFI_BOOK_DIR            = uefi\book

[LibraryClasses]
  LibGui|$(UEFI_BOOK_DIR)/GUIPkg/Library/UGui.inf
  LibCpp|$(UEFI_BOOK_DIR)/CppPkg/Library/CppLib.inf
  
  CppLib|$(UEFI_BOOK_DIR)/CppPkg/Library/CppLib.inf
[Components]
  $(UEFI_BOOK_DIR)/ffmpeg/ffplayer/fplayer.inf
  #$(UEFI_BOOK_DIR)/GcppPkg/test/testcpp.inf 
  $(UEFI_BOOK_DIR)/std/main.inf
  $(UEFI_BOOK_DIR)/infs/main/main.inf
  $(UEFI_BOOK_DIR)/disk/DevicePath/TestDevicePath.inf
  $(UEFI_BOOK_DIR)/disk/AtaPassThru/TestPassThru.inf
  $(UEFI_BOOK_DIR)/audio/audio.inf
  $(UEFI_BOOK_DIR)/systemtable/memory/BSmem.inf
  $(UEFI_BOOK_DIR)/mouse/mouse.inf
  $(UEFI_BOOK_DIR)/HotKey/HotKey.inf
  $(UEFI_BOOK_DIR)/FileIo/FileIo.inf #spec 2.4
  $(UEFI_BOOK_DIR)/infs/ShellAppMain/Main.inf
  $(UEFI_BOOK_DIR)/Shell/Execute/Execute.inf
  $(UEFI_BOOK_DIR)/GUIPkg/test/testgui.inf

[Components.X64]
  $(UEFI_BOOK_DIR)/audio/testac97.inf
