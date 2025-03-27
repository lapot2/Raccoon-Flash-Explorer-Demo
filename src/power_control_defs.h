#ifndef __POWER_CONTROL_DEFS_H__
#define __POWER_CONTROL_DEFS_H__

//#define VCC_OFF					0x01
#define VCC18				0x01
#define VCC33				0x03
#define EXT_PU_ON 		0x10
#define INT_PU_ON 		0x20

#define GND				P50
#define GND_93		P51
#define VCC				P53
#define V18				P52

#define GND_ON		 (P50 = 1)
#define GND_OFF	 	 (P50 = 0)
#define GND_93_ON	 (P51 = 1)
#define GND_93_OFF (P51 = 0)
#define VCC_ON	 	 (P53 = 0)
#define VCC_OFF	   (P53 = 1)
#define V18_ON	   (P52 = 1)
#define V18_OFF	   (P52 = 0)
#define PU_EXT_ON			(P35 = 0)
#define PU_EXT_OFF		(P35 = 1)

#endif //__POWER_CONTROL_DEFS_H__