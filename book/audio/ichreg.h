/********************************************************************************
**    Copyright (c) 1998-2000 Microsoft Corporation. All Rights Reserved.
**
**       Portions Copyright (c) 1998-1999 Intel Corporation
**
********************************************************************************/

#ifndef _ICHREG_H_
#define _ICHREG_H_
#include "wintype.h"
// We define the offsets like PI_BDBAR as ULONG (instead of UCHAR) for run
// time efficiency.

// CoDec AC97 register space offsets
AC_CONST ULONG PRIMARY_CODEC   = 0x00;
AC_CONST ULONG SECONDARY_CODEC = 0x80;

// Native audio bus master control registers (offsets)
AC_CONST ULONG PI_BDBAR        = 0x00; // PCM In Buffer Descriptor Base Address Register
AC_CONST ULONG PI_CIV          = 0x04; // PCM In Current Index Value
AC_CONST ULONG PI_LVI          = 0x05; // PCM In Last Valid Index
AC_CONST ULONG PI_SR           = 0x06; // PCM In Status Register
AC_CONST ULONG PI_PICB         = 0x08; // PCM In Position In Current Buffer
AC_CONST ULONG PI_PIV          = 0x0A; // PCM In Prefetch Index Value
AC_CONST ULONG PI_CR           = 0x0B; // PCM In Control Register
AC_CONST ULONG PO_BDBAR        = 0x10; // PCM Out Buffer Descriptor Base Address Register
AC_CONST ULONG PO_CIV          = 0x14; // PCM Out Current Index Value
AC_CONST ULONG PO_LVI          = 0x15; // PCM Out Last Valid Index
AC_CONST ULONG PO_SR           = 0x16; // PCM Out Status Register
AC_CONST ULONG PO_PICB         = 0x18; // PCM Out Position In Current Buffer
AC_CONST ULONG PO_PIV          = 0x1A; // PCM Out Prefetch Index Value
AC_CONST ULONG PO_CR           = 0x1B; // PCM Out Control Register
AC_CONST ULONG MC_BDBAR        = 0x20; // Mic In Buffer Descriptor Base Address Register
AC_CONST ULONG MC_CIV          = 0x24; // Mic In Current Index Value
AC_CONST ULONG MC_LVI          = 0x25; // Mic In Last Valid Index
AC_CONST ULONG MC_SR           = 0x26; // Mic In Status Register
AC_CONST ULONG MC_PICB         = 0x28; // Mic In Position In Current Buffer
AC_CONST ULONG MC_PIV          = 0x2A; // Mic In Prefetch Index Value
AC_CONST ULONG MC_CR           = 0x2B; // Mic In Control Register
AC_CONST ULONG GLOB_CNT        = 0x2C; // Global Control
AC_CONST ULONG GLOB_STA        = 0x30; // Global Status
AC_CONST ULONG CAS             = 0x34; // Codec Access Semiphore

// Defines for relative accesses (offsets)
AC_CONST ULONG X_PI_BASE       = 0x00; // PCM In Base
AC_CONST ULONG X_PO_BASE       = 0x10; // PCM Out Base
AC_CONST ULONG X_MC_BASE       = 0x20; // Mic In Base
AC_CONST ULONG X_BDBAR         = 0x00; // Buffer Descriptor Base Address Register
AC_CONST ULONG X_CIV           = 0x04; // Current Index Value
AC_CONST ULONG X_LVI           = 0x05; // Last Valid Index
AC_CONST ULONG X_SR            = 0x06; // Status Register
AC_CONST ULONG X_PICB          = 0x08; // Position In Current Buffer
AC_CONST ULONG X_PIV           = 0x0A; // Prefetch Index Value
AC_CONST ULONG X_CR            = 0x0B; // Control Register

// Bits defined in satatus register (*_SR)
AC_CONST USHORT SR_FIFOE       = 0x0010;   // FIFO error
AC_CONST USHORT SR_BCIS        = 0x0008;   // Buffer Completeion Interrupt Status
AC_CONST USHORT SR_LVBCI       = 0x0004;   // Last Valid Buffer Completion Interrupt
AC_CONST USHORT SR_CELV        = 0x0002;   // Last Valid Buffer Completion Interrupt

// Global Control bit defines (GLOB_CNT)
AC_CONST ULONG GLOB_CNT_PCM6   = 0x00200000;   // 6 Channel Mode bit
AC_CONST ULONG GLOB_CNT_PCM4   = 0x00100000;   // 4 Channel Mode bit
AC_CONST ULONG GLOB_CNT_SRIE   = 0x00000020;   // Secondary Resume Interrupt Enable
AC_CONST ULONG GLOB_CNT_PRIE   = 0x00000010;   // Primary Resume Interrupt Enable
AC_CONST ULONG GLOB_CNT_ACLOFF = 0x00000008;   // ACLINK Off
AC_CONST ULONG GLOB_CNT_WARM   = 0x00000004;   // AC97 Warm Reset
AC_CONST ULONG GLOB_CNT_COLD   = 0x00000002;   // AC97 Cold Reset
AC_CONST ULONG GLOB_CNT_GIE    = 0x00000001;   // GPI Interrupt Enable

// Global Status bit defines (GLOB_STA)
AC_CONST ULONG GLOB_STA_MC6    = 0x00200000;   // Multichannel Capability 6 channel
AC_CONST ULONG GLOB_STA_MC4    = 0x00100000;   // Multichannel Capability 4 channel
AC_CONST ULONG GLOB_STA_MD3    = 0x00020000;   // Modem Power Down Semiphore
AC_CONST ULONG GLOB_STA_AD3    = 0x00010000;   // Audio Power Down Semiphore
AC_CONST ULONG GLOB_STA_RCS    = 0x00008000;   // Read Completion Status
AC_CONST ULONG GLOB_STA_B3S12  = 0x00004000;   // Bit 3 Slot 12
AC_CONST ULONG GLOB_STA_B2S12  = 0x00002000;   // Bit 2 Slot 12
AC_CONST ULONG GLOB_STA_B1S12  = 0x00001000;   // Bit 1 Slot 12
AC_CONST ULONG GLOB_STA_SRI    = 0x00000800;   // Secondary Resume Interrupt
AC_CONST ULONG GLOB_STA_PRI    = 0x00000400;   // Primary Resume Interrupt
AC_CONST ULONG GLOB_STA_SCR    = 0x00000200;   // Secondary Codec Ready
AC_CONST ULONG GLOB_STA_PCR    = 0x00000100;   // Primary Codec Ready
AC_CONST ULONG GLOB_STA_MINT   = 0x00000080;   // Mic In Interrupt
AC_CONST ULONG GLOB_STA_POINT  = 0x00000040;   // PCM Out Interrupt
AC_CONST ULONG GLOB_STA_PIINT  = 0x00000020;   // PCM In Interrupt
AC_CONST ULONG GLOB_STA_MOINT  = 0x00000004;   // Modem Out Interrupt

// CoDec Access Semiphore bit defines (CAS)
AC_CONST UCHAR CAS_CAS         = 0x01; // Codec Access Semiphore Bit

// DMA Engine Control Register (*_CR) bit defines
AC_CONST UCHAR CR_IOCE         = 0x10; // Interrupt On Completion Enable
AC_CONST UCHAR CR_FEIE         = 0x08; // FIFO Error Interrupt Enable
AC_CONST UCHAR CR_LVBIE        = 0x04; // Last Valid Buffer Interrupt Enable
AC_CONST UCHAR CR_RPBM         = 0x01; // Run/Pause Bus Master
AC_CONST UCHAR CR_RR           = 0x02; // Reset Registers (RR)

// BDL policy bits
AC_CONST USHORT IOC_ENABLE     = 0x8000;
AC_CONST USHORT BUP_SET        = 0x4000;

#endif  //_ICHREG_H_


