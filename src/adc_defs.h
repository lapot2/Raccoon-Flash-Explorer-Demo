#ifndef __ADC_DEFS_H__
#define __ADC_DEFS_H__

#define AVERAGE1		0x00// CVTIMESEL[2:0]
#define AVERAGE2		0x04
#define AVERAGE4		0x05
#define AVERAGE8		0x06
#define AVERAGE16		0x07


#define DMA_AVERAGE1		0x00// CVTIMESEL[3:0]
#define DMA_AVERAGE2		0x08
#define DMA_AVERAGE4		0x09
#define DMA_AVERAGE8		0x0A
#define DMA_AVERAGE16		0x0B
#define DMA_AVERAGE32		0x0C
#define DMA_AVERAGE64		0x0D
#define DMA_AVERAGE128	0x0E
#define DMA_AVERAGE256	0x0F

#define ADC_START		0x40
#define ADC_FLAG		0x20
//#define VREF				((WORD)(CHIPID7 << 8 | CHIPID8))
#define VREF				(*(WORD *)&CHIPID7)

#endif //__ADC_DEFS_H__