// Raccoon flash explorer - setup parameters & vars
//
#include "rfe_params.h"

const setup_t code rfe_setup _at_ 0xf800;	// constant/init values in eeprom
active_t xdata rfe_active;								// data params after ic init

bool manual_setup = 0;

//