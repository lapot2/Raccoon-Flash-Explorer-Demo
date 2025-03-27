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
#include "util.h"
#include "usb_desc.h"

char code DEVICEDESC[18] =
{
	////////////////////////////////////////////////////////Device Descriptor
    0x12,                   //bLength(18);
    0x01,                   //bDescriptorType(Device);
    0x00,0x02,              //bcdUSB(2.00);
    0x00,                   //bDeviceClass(0);
    0x00,                   //bDeviceSubClass(0);
    0x00,                   //bDeviceProtocol(0);
    0x40,                   //bMaxPacketSize0(64);

    0xbf,0x34,              //idVendor(34BF);// STC VID/PID
    0x06,0xff,              //idProduct(FF06);
	
    0x00,0x01,              //bcdDevice(1.00);
    0x01,                   //iManufacturer(1);
    0x02,                   //iProduct(2);
    0x03,                   //iSerialNumber(3);
    0x01,                   //bNumConfigurations(1);
};

char code CONFIGDESC[32] =
{
	////////////////////////////////////////////////////////Config Descriptor
    0x09,                   //bLength(9);
    0x02,                   //bDescriptorType(Configuration);
    0x20,0x00,              //wTotalLength(32);
    0x01,                   //bNumInterfaces(1);
    0x01,                   //bConfigurationValue(1);
    0x00,                   //iConfiguration(0);
    0x80,                   //bmAttributes(BUSPower);
    //0x32,                   //MaxPower(100mA);
		0xFA,                   //MaxPower(500mA);

	////////////////////////////////////////////////////////Interface Descriptor
    0x09,                   //bLength(9);
    0x04,                   //bDescriptorType(Interface);
    0x00,                   //bInterfaceNumber(0);
    0x00,                   //bAlternateSetting(0);
    0x02,                   //bNumEndpoints(2);
    0x08,                   //bInterfaceClass(Mass Storage Class);
    0x06,                   //bInterfaceSubClass(SCSI Transparent Command Set);
    0x50,                   //bInterfaceProtocol(Bulk Only Transport);
    0x00,                   //iInterface(0);

	////////////////////////////////////////////////////////Endpoint Descriptor
    /*0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x81,                   //bEndpointAddress(EndPoint1 as IN);
    0x02,                   //bmAttributes(Bulk);
    0x40,0x00,              //wMaxPacketSize(64);
    0x00,                   //bInterval(Ignored);

    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x01,                   //bEndpointAddress(EndPoint1 as OUT);
    0x02,                   //bmAttributes(Bulk);
    0x40,0x00,              //wMaxPacketSize(64);
    0x00,                   //bInterval(Ignored);
		*/
	////////////////////////////////////////////////////////Endpoint IN Descriptor
		0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x81,                   //bEndpointAddress(EndPoint1 as IN);
    0x02,                   //bmAttributes(Bulk);
    0x40,0x00,              //wMaxPacketSize(64);
    0x00,                   //bInterval(Ignored);
	////////////////////////////////////////////////////////Endpoint OUT Descriptor
    0x07,                   //bLength(7);
    0x05,                   //bDescriptorType(Endpoint);
    0x01,                   //bEndpointAddress(EndPoint1 as OUT);
    0x02,                   //bmAttributes(Bulk);
    0x40,0x00,              //wMaxPacketSize(64);
    0x00,                   //bInterval(Ignored);
};

char code LANGIDDESC[4] =
{
    0x04,0x03,
    0x09,0x04,
};

char code MANUFACTDESC[16] = 
{
	0x10,0x03,
    'R',0,
    'a',0,
    0xA9,0,
    0xA9,0,
		//'c',0,
    //'c',0,
    'o',0,
    'o',0,
    'n',0,
};

char code PRODUCTDESC[56] =
{
    0x38,0x03,
		'R',0,
    'a',0,
    //'c',0,
    //'c',0,
		0xa9,0,
    0xa9,0,
    'o',0,
    'o',0,
    'n',0,
		' ',0,
    'F',0,
    'l',0,
    'a',0,
    's',0,
    'h',0,
    ' ',0,
    'E',0,
    'x',0,
    'p',0,
    'l',0,
    'o',0,
    'r',0,
    'e',0,
    'r',0,
		' ',0,
    'D',0,
    'E',0,
    'M',0,
    'O',0,
};

char xdata SERIALDESC[30]=//[18] =
{
    0x1e/*2*/,0x03,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
    '0',0,
};


char code PACKET0[2] = 
{
    0, 0,
};

char code PACKET1[2] = 
{
    1, 0,
};

