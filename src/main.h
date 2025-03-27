#ifndef __MAIN_H__
#define __MAIN_H__

// Setup structure / any set may be loaded from eeprom on start
// data params after ic init
//#define __CX2__

#include <stdlib.h>
#include <intrins.h>
#include "rfe_params.h"
#include "stc.h"
#include "scsi.h"
#include "bit_seek.h"
#include "errors_collector.h"
#include "hw_monitor.h"
#include "pin_checker.h"
#include "hw_init.h"

extern char cset_fwd;
extern char cset_rev;

//#define SFDP_TEST

#endif //__MAIN_H__