
#include "stc.h"
#include "usb.h"
#include "util.h"
#include "scsi.h"
#include "scsi_rw_parser.h"
#include "memory.h"
#include "spi.h"
#include "pin_checker.h"
#include "hw_monitor.h"
#include "port_mode_switch.h"
#include "rfe_params.h"
#include "sfdp.h"

BYTE sfdp_header[256];
DWORD sfdp_mem_size = 0;
BYTE sfdp_read(BYTE * buf, WORD len)
{
	WORD data i, y;
	
	sfdp_base_t 		xdata * sfdp_base = buf;
	sfdp_ptheader_t xdata * sptheader = buf + sizeof(sfdp_base_t);
	sfdp_bfptable_t xdata * sbfptable = nullptr;
	
	memset_xram(buf, 0, len);
	
	// read base header
	SPI_CS = 0;
	spi_send(SPI_SFDP);
	spi_send(0); spi_send(0); spi_send(0); spi_send(0);
	for(i = 0; i < sizeof(sfdp_base_t); i++)
	{
		*((BYTE *)sfdp_base+i) = spi_send(0);
	} SPI_CS = 1;

	// set the actual pt table address
	sbfptable = (sfdp_bfptable_t *)(sptheader + sfdp_base->num_headers + 1);
	buf = (BYTE *)sbfptable;//(sptheader + sfdp_base->num_headers + 1);

	// read pt headers
	SPI_CS = 0;
	spi_send(SPI_SFDP);
	spi_send(0); spi_send(0); spi_send(sizeof(sfdp_base_t)); spi_send(0);
	for(i = 0; i < (sfdp_base->num_headers + 1) * sizeof(sfdp_ptheader_t); i++)
	{
		*((BYTE *)sptheader+i) = spi_send(0);
	} SPI_CS = 1;

	// read tables	
	for(y = 0; y < sfdp_base->num_headers + 1; y++)
	{
		SPI_CS = 0;
		spi_send(SPI_SFDP);
		spi_send(sptheader->pt_a2); spi_send(sptheader->pt_a1); spi_send(sptheader->pt_a0); spi_send(0);
		for(i = 0; i < sptheader->pt_table_len * 4; i++)
		{
			*buf++ = spi_send(0);
		} SPI_CS = 1;
		sptheader++;
	}
	
	sfdp_mem_size = reverse4(sbfptable->mem_size);
	
	memcpy(sfdp_header, scsibuf, 256);
	
	return 0;
}