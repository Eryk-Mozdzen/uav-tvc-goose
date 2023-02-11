#include "usb.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

USBD_HandleTypeDef hUsbDeviceFS;

void USB_Init() {
	USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
	USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
	USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
	USBD_Start(&hUsbDeviceFS);
}

void USB_print(const char *format, ...) {
	va_list args;
    va_start(args, format);

	char buffer[256];

	vsnprintf(buffer, sizeof(buffer), format, args);

	CDC_Transmit_FS((uint8_t *)buffer, strlen(buffer));
}
