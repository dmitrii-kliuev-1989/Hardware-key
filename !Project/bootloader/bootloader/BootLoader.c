// 	В загрузчике оставить только следующие функциии
//	1) форматирования по считанному биту из EEPROM 
//	2) Считывание причины форматирования из EEPROM

#define  INCLUDE_FROM_CATERINA_C
#include "BootLoader.h"

static CDC_LineEncoding_t LineEncoding = { .BaudRateBPS = 0,
                                           .CharFormat  = CDC_LINEENCODING_OneStopBit,
                                           .ParityType  = CDC_PARITY_None,
                                           .DataBits    = 8                            };
									

static uint16_t unixTimeStampEEPROMaddr = 10;	// адрес времени синхронизации
static uint16_t serialNumEEPROMaddr = 15; 		// адрес серийного номера в EEPROM
static uint16_t formattingMarkEEPROMaddr = 30;	// адрес метки форматирования

uint8_t sizeSerNum = 10;	// размер серийного номера

///////////////////////////////////////////////////////////////
// Адреса причин перезагрузки в EEPROM
static uint16_t EXTRF_EEPROMaddr = 170;		// аппаратный ресет

static bool ApplicationIsExists = false;	// Наличие приложения по адресу 0x0000

int main(void)
{	
	save_mcusr_inf();

	initialization_func();
	if (pgm_read_word(0) != 0xFFFF) ApplicationIsExists = true;
	while (true)
	{
		uint8_t formatting_mark = eeprom_read_byte ((uint8_t *)formattingMarkEEPROMaddr);
		if (formatting_mark != 0xFF) // если флаг форматирования установлен
		{
			blink();
			FormatAndCheck_Flash();
			EEPROM_erase_withOut_range(5, 31);
			blink();
		}
		
		if (ApplicationIsExists) ApplicationStart(); 		// Перейти в приложение если оно есть
		
		USB_USBTask();
		CDC_Task();
	}	
}

void initialization_func()
{
	wdt_disable();						// отключить сторожевой таймер
	clock_prescale_set(clock_div_1);	// Отключить деление тактовой частоты
	
	// Перенос таблицы векторов прерываний в секцию загрузчика
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);
	
	LED_SETUP();
	RX_LED_OFF();
	TX_LED_OFF();
	CPU_PRESCALE(0); 
	
	OCR1AH = 0;
	OCR1AL = 250;
	TIMSK1 = (1 << OCIE1A);					// enable timer 1 output compare A match interrupt
	TCCR1B = ((1 << CS11) | (1 << CS10));	// 1/64 prescaler on timer 1 input
	
	USB_Init(); 	// Initialize USB Subsystem
	sei(); 			// Enable global interrupts so that the USB stack can function	
}

bool ReseivePrepare()
{
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	if (!(Endpoint_IsOUTReceived()))
	  return false;

	return true;
}

void CDC_Task(void)
{
	if (ReseivePrepare() != true) return;

	uint8_t command = FetchNextCommandByte();

	switch(command)
	{			
		default:
			SendFormattingReason();
			break;
	}
}

void SendFormattingReason()
{
	sendReason();
	sendSN();
	sendTime();
}

void sendReason()
{
	uint8_t formatting_mark = eeprom_read_byte ((uint8_t *)formattingMarkEEPROMaddr);

	uint8_t reasonArr_size = 9;
	char reasonArr[9] = "Reason: ";
	reasonArr[8] = formatting_mark;

	uint8_t rByteArr[reasonArr_size];
	for (uint8_t i = 0; i < reasonArr_size; i++) rByteArr[i] = reasonArr[i];
	
	SendByteArray(rByteArr, reasonArr_size);
}

void sendSN()
{
	uint8_t sArr_size = 17;
	char sArr[17] = ". S/N: ";
	
	uint8_t sn_arr_EEPROM[sizeSerNum];
	eeprom_read_block(sn_arr_EEPROM, (uint8_t*)serialNumEEPROMaddr, sizeSerNum);
		
	for(uint8_t i = 0; i < sizeSerNum; i++)
	{
		if (sn_arr_EEPROM[i] != 0xFF)
			sArr[i + 7] = sn_arr_EEPROM[i];
		else
			sArr[i + 7] = ' ';
	}
	
	uint8_t snByteArr[sArr_size];
	for (uint8_t i = 0; i < sArr_size; i++) snByteArr[i] = sArr[i];

	
	SendByteArray(snByteArr, sArr_size);
}

void sendTime()
{
	uint8_t strTime_size = 19;
	char strTime[19] = ". Time: ";

	uint32_t time = eeprom_read_dword((uint32_t*)unixTimeStampEEPROMaddr);


	char time_CharArr[10];
	uint8_t time_size = 0;
	split_int(time, time_CharArr, (uint8_t*)&time_size );

	for(uint8_t i = 0; i < 10; i++)	strTime[i + 8] = time_CharArr[i];
	
	strTime[18] = '\r';
	
	uint8_t timeByteArr[strTime_size];
	for(uint8_t i = 0; i < strTime_size; i++)	{ timeByteArr[i] = strTime[i]; }

	SendByteArray(timeByteArr, strTime_size);
}

void split_int(uint32_t num, char * c, uint8_t * size)
{
	uint32_t n = num;
	uint32_t numQty = 0;
	
	// посчитать кол-во цифр в числе
	while (n != 0) { n /= 10; numQty++; }

	*size = numQty;

	for (uint8_t i = 0; i < numQty; i++)
	{
		c[i] = (char)((num % 10) + 48);
		num = num / 10;
	}

	char t;
	//инвертируем массив символов
	for (uint8_t i = 0; i < numQty / 2; i++)
	{
		t = c[i];
		c[i] = c[numQty - 1 - i];
		c[numQty - 1 - i] = t;
	}
}

void save_mcusr_inf()
{	
	if ( MCUSR & ( 1 << EXTRF ))	//внешний сброс
	{
		MCUSR &=  ~( 1 << EXTRF );	// сбросить флаг
		eeprom_write_byte_safe((uint8_t*)EXTRF_EEPROMaddr, 1);
	}
	else
		eeprom_write_byte_safe((uint8_t*)EXTRF_EEPROMaddr, 0);
	
	MCUSR = 0;							// очистить все флаги сброса
}

// Проверить, есть ли флаг форматирования и если есть, то форматировать.
bool FormatAndCheck_Flash()
{	
	bool chk = CheckFormat_Flash();
	if (chk == true) return true;
		
	while (CheckFormat_Flash() != true)
	{
		FlashFormatting();
	}

	return true;
}

bool CheckFormat_Flash()
{
	uint32_t page = 0;
	for (int i = 0; i < FLASHEND; i++)
	{
		page = i * 0x80;
		if (page == BOOT_START_ADDR)
			break;
		
		uint8_t byte = pgm_read_byte_near(i);

		if(byte != 0xff)
			return false;
	}
	return true;
}

void FlashFormatting()
{
	uint8_t sreg = SREG;
	cli();
	
	uint32_t page = 0;
	for (uint32_t i = 0; i < FLASH_PAGE_COUNT; i++)
	{
		page = i * 0x80;
		
		if (page == BOOT_START_ADDR)
		break;
		
		eeprom_busy_wait ();
		boot_page_erase_safe (page);
		boot_spm_busy_wait ();      // Wait until the memory is erased.
	}

	boot_rww_enable();

	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;
}


void EEPROM_erase_withOut_range(uint16_t start, uint16_t end)
{
	for (uint16_t i = 0; i < E2END + 1; i++)
	{
		if(start != 0 || end != 0)
		{
			if (i >= start && i <= end)
			continue;
		}
		
		uint8_t checkByte = eeprom_read_byte((uint8_t*)i);
		if (checkByte != 0xFF)
			eeprom_write_byte_safe((uint8_t*)i, 0xff);
	}
}

void blink()
{
	TX_LED_ON();
	_delay_ms(100);
	TX_LED_OFF();
	_delay_ms(100);
}

void SendByteArray(uint8_t * data, uint8_t size)
{	
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);
	
	
	for(uint8_t i = 0; i < size; i++) WriteNextResponseByte(data[i]);
		
	// Remember if the endpoint is completely full before clearing it
	bool IsEndpointFull = !(Endpoint_IsReadWriteAllowed());

	// Send the endpoint data to the host
	Endpoint_ClearIN();

	// If a full endpoint's worth of data was sent, we need to send an empty packet afterwards to signal end of transfer
	if (IsEndpointFull)
	{
		while (!(Endpoint_IsINReady()))
		{
			if (USB_DeviceState == DEVICE_STATE_Unattached)
			  return;
		}

		Endpoint_ClearIN();
	}

	// Wait until the data has been sent to the host
	while (!(Endpoint_IsINReady()))
	{
		if (USB_DeviceState == DEVICE_STATE_Unattached)
		  return;
	}

	// Select the OUT endpoint
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);

	// Acknowledge the command from the host
	Endpoint_ClearOUT();
}


static uint8_t FetchNextCommandByte(void)
{
	// Select the OUT endpoint so that the next data byte can be read
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);

	// If OUT endpoint empty, clear it and wait for the next packet from the host
	while (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearOUT();

		while (!(Endpoint_IsOUTReceived()))
		{
			if (USB_DeviceState == DEVICE_STATE_Unattached)
			  return 0;
		}
	}

	// Fetch the next byte from the OUT endpoint
	return Endpoint_Read_8();
}

static void WriteNextResponseByte(const uint8_t Response)
{
	// Select the IN endpoint so that the next data byte can be written
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);

	// If IN endpoint full, clear it and wait until ready for the next packet to the host
	if (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearIN();

		while (!(Endpoint_IsINReady()))
		{
			if (USB_DeviceState == DEVICE_STATE_Unattached)
			  return;
		}
	}

	// Write the next byte to the IN endpoint
	Endpoint_Write_8(Response);
}

/** Event handler for the USB_ConfigurationChanged event. This configures the device's endpoints ready
 *  to relay data to and from the attached USB host.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	// Setup CDC Notification, Rx and Tx Endpoints 
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT,
	                           ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, EP_TYPE_BULK,
	                           ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);

	Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, EP_TYPE_BULK,
	                           ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);
}

// Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
//  the device from the USB host before passing along unhandled control requests to the library for processing
//  internally.
//
void EVENT_USB_Device_ControlRequest(void)
{
	// Ignore any requests that aren't directed to the CDC interface
	if ((USB_ControlRequest.bmRequestType & (CONTROL_REQTYPE_TYPE | CONTROL_REQTYPE_RECIPIENT)) !=
	    (REQTYPE_CLASS | REQREC_INTERFACE))
	{
		return;
	}

	// Process CDC specific control requests 
	switch (USB_ControlRequest.bRequest)
	{
		case CDC_REQ_GetLineEncoding:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Write the line coding data to the control endpoint
				Endpoint_Write_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
				Endpoint_ClearOUT();
			}

			break;
		case CDC_REQ_SetLineEncoding:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				// Read the line coding data in from the host into the global struct 
				Endpoint_Read_Control_Stream_LE(&LineEncoding, sizeof(CDC_LineEncoding_t));
				Endpoint_ClearIN();
			}
			break;
	}
}

void ApplicationStart(void)
{
	cli();

	//Сброс TIMER1 и счетчика перед запуском скетча
	TIMSK1 = 0;
	TCCR1B = 0;
	
	// Перенос таблицы векторов прерываний в раздел приложения
	MCUCR = (1 << IVCE);
	MCUCR = 0;

	//перейти к началу пространства приложений
	__asm__ volatile("jmp 0x0000");
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	// Reset counter
	TCNT1H = 0;
	TCNT1L = 0;
}