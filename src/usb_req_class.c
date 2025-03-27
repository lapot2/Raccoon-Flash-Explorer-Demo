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
#include "usb_req_class.h"
#include "util.h"
#include "usb_desc.h"

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case DEVICERESET:
        usb_device_reset();
        break;
    case GETMAXLUN:
        usb_get_maxlun();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_device_reset()
{
    usb_setup_status();
}

void usb_get_maxlun()
{
    Ep0State.pData = PACKET0;
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}
