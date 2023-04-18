#include "usbd_cdc_if.h"

#include "FreeRTOS.h"
#include "task.h"

#include "transport.h"

uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

extern TaskHandle_t wire_transmitter_task;

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t CDC_Init_FS();
static int8_t CDC_DeInit_FS();
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
	CDC_Init_FS,
	CDC_DeInit_FS,
	CDC_Control_FS,
	CDC_Receive_FS,
	CDC_TransmitCplt_FS
};

static int8_t CDC_Init_FS() {
	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
	return USBD_OK;
}

static int8_t CDC_DeInit_FS() {
	return USBD_OK;
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length) {
	(void)cmd;
	(void)pbuf;
	(void)length;

	return USBD_OK;
}

static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len) {
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	for(uint32_t i=0; i<*Len; i++) {
		Transport::getInstance().wire_rx_queue.push_ISR(Buf[i], xHigherPriorityTaskWoken);
	}

	USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	return USBD_OK;
}

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len) {
	uint8_t result = USBD_OK;

	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
	if(hcdc->TxState != 0){
		return USBD_BUSY;
	}
	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
	result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);

	return result;
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum) {
	(void)Buf;
	(void)Len;
	(void)epnum;

	vTaskNotifyGiveIndexedFromISR(wire_transmitter_task, 0, NULL);

	return USBD_OK;
}
