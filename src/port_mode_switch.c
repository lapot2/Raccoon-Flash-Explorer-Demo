#include "port_mode_switch.h"


typedef struct{
	uint16_t PM0;
	uint16_t PM1;
	uint16_t PM2;
	uint16_t PM3;
	uint16_t PM4;
	uint16_t PM5;
	uint16_t PM6;
	uint16_t PM7;
}PORT_MODE;

extern PORT_MODE xdata * pin_mode_set(char ic_pin, uint16_t  pinmode, PORT_MODE xdata * portmode);
extern void port_cfg_get(PORT_MODE xdata * portmode);
extern void port_cfg_submit(PORT_MODE xdata * portmode);

//void main(){
//	port_cfg(PIN_5, I2C | PIN_PPULL);// dummy
//	port_cfg(PIN_NUM|5, I2C | PIN_PPULL);// dummy
//}
void port_cfg(char ic_pin, uint16_t pinmode)
{
	xdata PORT_MODE xdata * portmode = scsibuf;
	data char state = (char)(pinmode & 0xfe);

	port_cfg_get(portmode);	
	
	if(PIN_DEFAULT == ic_pin){
		switch(state)
		{
			case SPI: 
			{
				portmode->PM1 = 0xFFFF;
				portmode->PM2 = 0x00FF;
				portmode->PM3 = 0x20DF;
				portmode->PM5 = 0x1F10;
				portmode->PM7 = 0x00FE;
				break;
			}
			case I2C:
			{
				pin_mode_set(FPIN_5, PIN_INPUT, \
				pin_mode_set(FPIN_6, PIN_INPUT, portmode));
				
				pin_mode_set(FPIN_5_I2C, PIN_OPDRN, \
				pin_mode_set(FPIN_6_I2C, PIN_OPDRN, portmode));
				break;
			}
			case E93:
			{
				pin_mode_set(FPIN_2_E93, PIN_OPDRN, \
				pin_mode_set(FPIN_3_E93, PIN_OPDRN, \
				pin_mode_set(FPIN_4_E93, PIN_OPDRN, portmode)));
				break;
			}
		}
		port_cfg_submit(portmode);
		return;
	}
	//
	switch(ic_pin)
	{
		case 0x80:
		case FPIN_1: { ic_pin = FPIN_1; break;}
		case 0x81:
		case FPIN_2: 
		{
			if(E93 == state) ic_pin = FPIN_2_E93;
			else ic_pin = FPIN_2;
			break;}
		case 0x82:
		case FPIN_3: 
		{
			if(E93 == state) ic_pin = FPIN_3_E93;
			else ic_pin = FPIN_3;
			break;}
		case 0x83:
		case FPIN_4: 
		{
			if(E93 == state) ic_pin = FPIN_4_E93;
			else ic_pin = FPIN_4;
			break;}
		case 0x84:
		case FPIN_5: 
		{
			if(I2C == state) ic_pin = FPIN_5_I2C;
			else ic_pin = FPIN_5;
			break;}
		case 0x85:
		case FPIN_6: 
		{
			if(I2C == state) ic_pin = FPIN_6_I2C;
			else ic_pin = FPIN_6;
			break;}
		case 0x86:
		case FPIN_7: { ic_pin = FPIN_7; break;}
		case 0x87:
		case FPIN_8: { ic_pin = FPIN_8; break;}
		default: break;
	}
	//
	pin_mode_set(ic_pin, pinmode, portmode);
	
	port_cfg_submit(portmode);
}
