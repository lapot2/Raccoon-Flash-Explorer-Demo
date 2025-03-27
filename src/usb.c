/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "usb.h"
#include "usb_req_std.h"
#include "usb_req_class.h"
#include "usb_req_vendor.h"
#include "util.h"
#include "scsi.h"
//#include "sound.h"

BYTE DeviceState;
SETUP Setup;
EPSTATE Ep0State;
BYTE InEpState;
BYTE OutEpState;

void usb_init()
{
    P3M0 &= ~0x03;
    P3M1 |= 0x03;
    
    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01));
		CLKDIV = 2;
    CLKSEL = 0x0C;
		USBCON = 0x90;

    usb_write_reg(FADDR, 0x00);
    usb_write_reg(POWER, 0x81);
    usb_write_reg(INTRIN1E, 0x01);
    usb_write_reg(INTROUT1E, 0x02);
    usb_write_reg(INTRUSBE, 0x07);
    usb_write_reg(POWER, 0x01);
    
    DeviceState = DEVSTATE_DEFAULT;
    Ep0State.bState = EPSTATE_IDLE;
    InEpState = 0x00;
    OutEpState = 0x00;
    
    IE2 |= 0x80;    //EUSB = 1;
}

BYTE usb_read_reg(BYTE addr)
{
	BYTE dat;
    
	while (USBADR & 0x80);
	USBADR = addr | 0x80;
	while (USBADR & 0x80);
	dat = USBDAT;
    
	return dat;
}

void usb_write_reg(BYTE addr, BYTE dat)
{
	while (USBADR & 0x80);
	USBADR = addr & 0x7f;
	USBDAT = dat;
}

BYTE usb_read_fifo(BYTE fifo, BYTE *pdat)
{
    BYTE cnt;
    BYTE ret;

    ret = cnt = usb_read_reg(COUNT0);
    while (cnt--)
    {
    	*pdat++ = usb_read_reg(fifo);
    }

    return ret;
}

void usb_write_fifo(BYTE fifo, BYTE *pdat, BYTE cnt)
{
	while (cnt--)
	{
        usb_write_reg(fifo, *pdat++);
    }
}

//
//BOOL init_ok = 0;
void usb_isr() interrupt 25
{
    BYTE intrusb;
    BYTE intrin;
    BYTE introut;
		BOOL init_ok = 0;
    
    intrusb = usb_read_reg(INTRUSB);
    intrin = usb_read_reg(INTRIN1);
    introut = usb_read_reg(INTROUT1);
    
    if (intrusb & RSUIF) usb_resume();
    if (intrusb & RSTIF) usb_reset();

    if (intrin & EP0IF)
		{
			usb_setup();
			if(!init_ok)
			{
				P7PU = 0x01;// P70 LED PU /// Increase LED brightness
				blinker(255,1);
				init_ok = 1;
			}
		}

#ifdef EN_EP1IN
    if (intrin & EP1INIF) usb_in_ep1();
#endif
#ifdef EN_EP2IN
    if (intrin & EP2INIF) usb_in_ep2();
#endif
#ifdef EN_EP3IN
    if (intrin & EP3INIF) usb_in_ep3();
#endif
#ifdef EN_EP4IN
    if (intrin & EP4INIF) usb_in_ep4();
#endif
#ifdef EN_EP5IN
    if (intrin & EP5INIF) usb_in_ep5();
#endif

#ifdef EN_EP1OUT
    if (introut & EP1OUTIF) usb_out_ep1();
#endif
#ifdef EN_EP2OUT
    if (introut & EP2OUTIF) usb_out_ep2();
#endif
#ifdef EN_EP3OUT
    if (introut & EP3OUTIF) usb_out_ep3();
#endif
#ifdef EN_EP4OUT
    if (introut & EP4OUTIF) usb_out_ep4();
#endif
#ifdef EN_EP5OUT
    if (introut & EP5OUTIF) usb_out_ep5();
#endif

    if (intrusb & SUSIF) usb_suspend();
}

void usb_resume()
{
}

void usb_reset()
{
    usb_write_reg(FADDR, 0x00);
    DeviceState = DEVSTATE_DEFAULT;
    Ep0State.bState = EPSTATE_IDLE;

#ifdef EN_EP1IN
    usb_write_reg(INDEX, 1);
    usb_write_reg(INCSR1, INCLRDT | INFLUSH);
#endif
#ifdef EN_EP2IN
    usb_write_reg(INDEX, 2);
    usb_write_reg(INCSR1, INCLRDT | INFLUSH);
#endif
#ifdef EN_EP3IN
    usb_write_reg(INDEX, 3);
    usb_write_reg(INCSR1, INCLRDT | INFLUSH);
#endif
#ifdef EN_EP4IN
    usb_write_reg(INDEX, 4);
    usb_write_reg(INCSR1, INCLRDT | INFLUSH);
#endif
#ifdef EN_EP5IN
    usb_write_reg(INDEX, 5);
    usb_write_reg(INCSR1, INCLRDT | INFLUSH);
#endif
#ifdef EN_EP1OUT
    usb_write_reg(INDEX, 1);
    usb_write_reg(OUTCSR1, OUTCLRDT | OUTFLUSH);
#endif
#ifdef EN_EP2OUT
    usb_write_reg(INDEX, 2);
    usb_write_reg(OUTCSR1, OUTCLRDT | OUTFLUSH);
#endif
#ifdef EN_EP3OUT
    usb_write_reg(INDEX, 3);
    usb_write_reg(OUTCSR1, OUTCLRDT | OUTFLUSH);
#endif
#ifdef EN_EP4OUT
    usb_write_reg(INDEX, 4);
    usb_write_reg(OUTCSR1, OUTCLRDT | OUTFLUSH);
#endif
#ifdef EN_EP5OUT
    usb_write_reg(INDEX, 5);
    usb_write_reg(OUTCSR1, OUTCLRDT | OUTFLUSH);
#endif
    usb_write_reg(INDEX, 0);
		//
}

void usb_suspend()
{
}

void usb_setup()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 0);
    csr = usb_read_reg(CSR0);
    
    if (csr & STSTL)
    {
        usb_write_reg(CSR0, csr & ~STSTL);  //对自己写 0 清除标志位
        Ep0State.bState = EPSTATE_IDLE;
    }
    if (csr & SUEND)
    {
        usb_write_reg(CSR0, csr | SSUEND);  //对 SSUEND 写 1 清除 SUEND 标志位
    }
    
    switch (Ep0State.bState)
    {
    case EPSTATE_IDLE:
        if (csr & OPRDY)
        {
            usb_read_fifo(FIFO0, (BYTE *)&Setup);
            Setup.wLength = reverse2(Setup.wLength);
            switch (Setup.bmRequestType & REQUEST_MASK)
            {
            case STANDARD_REQUEST:
                usb_req_std();
                break;
            case CLASS_REQUEST:
                usb_req_class();
                break;
            case VENDOR_REQUEST:
                usb_req_vendor();
                break;
            default:
                usb_setup_stall();
                return;
            }
        }
        break;
    case EPSTATE_DATAIN:
        usb_ctrl_in();
        break;
    case EPSTATE_DATAOUT:
        usb_ctrl_out();
        break;
    }
}

void usb_setup_stall()
{
    Ep0State.bState = EPSTATE_STALL;
    usb_write_reg(CSR0, SOPRDY | SDSTL);
}

void usb_setup_in()
{
    Ep0State.bState = EPSTATE_DATAIN;
    usb_write_reg(CSR0, SOPRDY);
    usb_ctrl_in();
}

void usb_setup_status()
{
    Ep0State.bState = EPSTATE_IDLE;
    usb_write_reg(CSR0, SOPRDY | DATEND);
}

void usb_ctrl_in()
{
    BYTE csr;
    BYTE cnt;
    
    usb_write_reg(INDEX, 0);
    csr = usb_read_reg(CSR0);
    if (csr & IPRDY) return;
    
    cnt = Ep0State.wSize > EP0_SIZE ? EP0_SIZE : Ep0State.wSize;
    usb_write_fifo(FIFO0, Ep0State.pData, cnt);
    Ep0State.wSize -= cnt;
    Ep0State.pData += cnt;
    if (Ep0State.wSize == 0)
    {
        usb_write_reg(CSR0, IPRDY | DATEND);
        Ep0State.bState = EPSTATE_IDLE;
    }
    else
    {
        usb_write_reg(CSR0, IPRDY);
    }
}

void usb_ctrl_out()
{
    BYTE csr;
    BYTE cnt;
    
    usb_write_reg(INDEX, 0);
    csr = usb_read_reg(CSR0);
    if (!(csr & OPRDY)) return;
    
    cnt = usb_read_fifo(FIFO0, Ep0State.pData);
    Ep0State.wSize -= cnt;
    Ep0State.pData += cnt;
    if (Ep0State.wSize == 0)
    {
        usb_write_reg(CSR0, SOPRDY | DATEND);
        Ep0State.bState = EPSTATE_IDLE;
    }
    else
    {
        usb_write_reg(CSR0, SOPRDY);
    }
}

void usb_bulk_intr_in(BYTE *pData, BYTE bSize, BYTE ep)
{
    usb_write_fifo((BYTE)(FIFO0 + ep), pData, bSize);
    usb_write_reg(INCSR1, INIPRDY);
}

BYTE usb_bulk_intr_out(BYTE *pData, BYTE ep)
{
    BYTE cnt;
    
    cnt = usb_read_fifo((BYTE)(FIFO0 + ep), pData);
    usb_write_reg(OUTCSR1, 0);
    
    return cnt;
}

BOOL usb_bulk_intr_in_busy()
{
    return (usb_read_reg(INCSR1) & INIPRDY);
}

BOOL usb_bulk_intr_out_ready()
{
    return (usb_read_reg(OUTCSR1) & OUTOPRDY);
}

#ifdef EN_EP1IN
void usb_in_ep1()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 1);
    csr = usb_read_reg(INCSR1);
    if (csr & INSTSTL)
    {
        usb_write_reg(INCSR1, INCLRDT);
    }
    if (csr & INUNDRUN)
    {
        usb_write_reg(INCSR1, 0);
    }
}
#endif

#ifdef EN_EP2IN
void usb_in_ep2()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 2);
    csr = usb_read_reg(INCSR1);
    if (csr & INSTSTL)
    {
        usb_write_reg(INCSR1, INCLRDT);
    }
    if (csr & INUNDRUN)
    {
        usb_write_reg(INCSR1, 0);
    }
}
#endif

#ifdef EN_EP3IN
void usb_in_ep3()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 3);
    csr = usb_read_reg(INCSR1);
    if (csr & INSTSTL)
    {
        usb_write_reg(INCSR1, INCLRDT);
    }
    if (csr & INUNDRUN)
    {
        usb_write_reg(INCSR1, 0);
    }
}
#endif

#ifdef EN_EP4IN
void usb_in_ep4()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 4);
    csr = usb_read_reg(INCSR1);
    if (csr & INSTSTL)
    {
        usb_write_reg(INCSR1, INCLRDT);
    }
    if (csr & INUNDRUN)
    {
        usb_write_reg(INCSR1, 0);
    }
}
#endif

#ifdef EN_EP5IN
void usb_in_ep5()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 5);
    csr = usb_read_reg(INCSR1);
    if (csr & INSTSTL)
    {
        usb_write_reg(INCSR1, INCLRDT);
    }
    if (csr & INUNDRUN)
    {
        usb_write_reg(INCSR1, 0);
    }
}
#endif

#ifdef EN_EP1OUT
void usb_out_ep1()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 1);
    csr = usb_read_reg(OUTCSR1);
    if (csr & OUTSTSTL)
    {
        usb_write_reg(OUTCSR1, OUTCLRDT);
    }
    if (csr & OUTOPRDY)
    {
        scsi_process();
    }
}
#endif

#ifdef EN_EP2OUT
void usb_out_ep2()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 2);
    csr = usb_read_reg(OUTCSR1);
    if (csr & OUTSTSTL)
    {
        usb_write_reg(OUTCSR1, OUTCLRDT);
    }
    if (csr & OUTOPRDY)
    {
        usb_bulk_intr_out(Ep2OutBuffer, 2);
    }
}
#endif

#ifdef EN_EP3OUT
void usb_out_ep3()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 3);
    csr = usb_read_reg(OUTCSR1);
    if (csr & OUTSTSTL)
    {
        usb_write_reg(OUTCSR1, OUTCLRDT);
    }
    if (csr & OUTOPRDY)
    {
        usb_bulk_intr_out(Ep3OutBuffer, 3);
    }
}
#endif

#ifdef EN_EP4OUT
void usb_out_ep4()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 4);
    csr = usb_read_reg(OUTCSR1);
    if (csr & OUTSTSTL)
    {
        usb_write_reg(OUTCSR1, OUTCLRDT);
    }
    if (csr & OUTOPRDY)
    {
        usb_bulk_intr_out(Ep4OutBuffer, 4);
    }
}
#endif

#ifdef EN_EP5OUT
void usb_out_ep5()
{
    BYTE csr;
    
    usb_write_reg(INDEX, 5);
    csr = usb_read_reg(OUTCSR1);
    if (csr & OUTSTSTL)
    {
        usb_write_reg(OUTCSR1, OUTCLRDT);
    }
    if (csr & OUTOPRDY)
    {
        usb_bulk_intr_out(Ep5OutBuffer, 5);
    }
}
#endif
