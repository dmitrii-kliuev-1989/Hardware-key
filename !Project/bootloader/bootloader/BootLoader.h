#ifndef _CDC_H_
#define _CDC_H_

	//  Includes:
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/eeprom.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <util/delay.h>
		
		#include "Descriptors.h"
		
		#include <LUFA/Drivers/USB/USB.h>
		
		#include "DongleCommon.h"
			
	// 	Macros:		
		#define FLASH_PAGE_COUNT FLASHEND / SPM_PAGESIZE
		
		#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
		#define LED_SETUP()		DDRC |= (1<<7); DDRB |= (1<<0); DDRD |= (1<<5);
		#define L_LED_OFF()		PORTC &= ~(1<<7)
		#define L_LED_ON()		PORTC |= (1<<7)
		#define L_LED_TOGGLE()	PORTC ^= (1<<7)
		
	// Безопасная запись памяти EEPROM
		#define eeprom_write_byte_safe(addr, val) 	{ eeprom_busy_wait(); eeprom_write_byte(addr, val); }
		#define eeprom_write_word_safe(addr, val)	{ eeprom_busy_wait(); eeprom_write_word(addr, val); }
		#define eeprom_write_dword_safe(addr, val) 	{ eeprom_busy_wait(); eeprom_write_dword(addr, val); }
		
		//		_____USB_____
		//		o           o
		//		o           o
		//		o     /\    o
		//		o    /  \   o
		//		o    \  /   o
		//		o     \/    o
		//		o           o
		//		o           o
		//		o           o
		//		o           o
		//		o  RX   TX  o
		//		o___________o
		
		#define TX_LED_OFF()	PORTD |= (1<<5)
		#define TX_LED_ON()		PORTD &= ~(1<<5)
		#define RX_LED_OFF()	PORTB |= (1<<0)
		#define RX_LED_ON()		PORTB &= ~(1<<0)

	// 	Function Prototypes:
		void initialization_func(void);		//	Инициализация
		void CDC_Task(void);
		void ApplicationStart(void);		// 	Старт прикладной программы
		bool FormatAndCheck_Flash(void);			// 	форматирование с проверкой байта причины в EEPROM и проверкой успешного форматирования
		void FlashFormatting(void);			// 	Форматирование flash
		bool CheckFormat_Flash(void);				// 	Проверка успешного форматирования flash
		
		void EEPROM_erase_withOut_range(uint16_t start, uint16_t end);	// Форматировать EEPROM исключая участок со start по end включительно
		
		void SendFormattingReason(void);	//	Отправить на ПК причину форматирования
		void split_int(uint32_t num, char * c, uint8_t * size);	// получить массив char из числа
		
		bool ReseivePrepare(void);			// 	Подготовка устройстсва к приёму данных
		void blink(void);					//	Моргнуть
		
		void SendByteArray(uint8_t * data, uint8_t size);	// отправить массив байт на ПК
		
		void SendPacket(uint8_t dataSize, uint8_t response, uint8_t * data);
		void EVENT_USB_Device_ConfigurationChanged(void);
		
		static uint8_t FetchNextCommandByte(void);
		static void    WriteNextResponseByte(const uint8_t Response);
		
		
		void save_mcusr_inf(void);	// информация о флагах сброса
		
		void sendReason(void);		// отправить причину
		void sendSN(void);			// отправить серийный номер
		void sendTime(void);		// отправить время последней синхронизации
		
#endif

