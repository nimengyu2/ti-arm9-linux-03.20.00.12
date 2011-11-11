/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006, 2007
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
/** ============================================================================
 *   @file  cslr_ehrpwm.h
 *
 *   @path  $(CSLPATH)\
 *
 *   @desc  This file contains the Register Descriptions for EHRPWM 
 *
 *  ============================================================================
 */
#ifndef _CSLR_EHRPWM_H_
#define _CSLR_EHRPWM_H_

#include <csl/cslr.h>
#include "tistdtypes.h"

/**************************************************************************\
* Register Overlay Structure for ePWM Module 
\**************************************************************************/
typedef struct  {
    volatile Uint16 TBCTL;
    volatile Uint16 TBSTS;
    volatile Uint16 TBPHSHR;
    volatile Uint16 TBPHS;
    volatile Uint16 TBCNT;
    volatile Uint16 TBPRD;
    volatile Uint8  RSVD1[2];
    volatile Uint16 CMPCTL;
    volatile Uint16 CMPAHR;
    volatile Uint16 CMPA;
    volatile Uint16 CMPB;
    volatile Uint16 AQCTLA;
    volatile Uint16 AQCTLB;
    volatile Uint16 AQSFRC;
    volatile Uint16 AQCSFRC;
} CSL_ePWMRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_ePWMRegs       *CSL_ePWMRegsOvly;
#endif
