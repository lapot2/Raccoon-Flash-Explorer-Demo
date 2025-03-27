#ifndef __USB_REQ_CLASS_H__
#define __USB_REQ_CLASS_H__

#define DEVICERESET             0xff
#define GETMAXLUN               0xfe

void usb_req_class();

void usb_device_reset();
void usb_get_maxlun();

#endif
