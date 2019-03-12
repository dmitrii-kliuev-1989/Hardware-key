#ifndef _CDC_H_
#define _CDC_H_

	/* Includes: */
		#include <avr/io.h>
		#include <stdlib.h>
		#include <stdio.h>
		#include <math.h>		
		#include <avr/wdt.h>
		#include <avr/eeprom.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <util/delay.h>
		#include <avr/boot.h>
		#include <inttypes.h>
		#include <avr/pgmspace.h>
		#include <util/atomic.h>
		#include "Descriptors.h"
		#include <LUFA/Drivers/USB/USB.h>
		
	//#define boot_memformat __attribute__ ((section (".memformat")))
	
		#define TEMPERATURE_OFFSET -7
		
		#define BOOTSTARTADDR 0x3800 * 2

	/*Random stuff*/
		#define MAX_DICE 255
	
	/* Macros: */		
		#define ABS(x) ((x) < 0 ? -(x) : (x))

		#define FLASH_PAGE_COUNT FLASHEND / SPM_PAGESIZE
		
		#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
		#define LED_SETUP()		DDRC |= (1<<7); DDRB |= (1<<0); DDRD |= (1<<5);
		#define TX_LED_OFF()	PORTD |= (1<<5)
		#define TX_LED_ON()		PORTD &= ~(1<<5)
		#define RX_LED_OFF()	PORTB |= (1<<0)
		#define RX_LED_ON()		PORTB &= ~(1<<0)

		// Безопасная запись памяти EEPROM
		#define eeprom_write_byte_safe(addr, val) 	{ eeprom_busy_wait(); eeprom_write_byte(addr, val); }
		#define eeprom_write_word_safe(addr, val)	{ eeprom_busy_wait(); eeprom_write_word(addr, val); }
		#define eeprom_write_dword_safe(addr, val) 	{ eeprom_busy_wait(); eeprom_write_dword(addr, val); }

		// Для контроля ножек
		#define HIGH 0x1
		#define LOW  0x0

		#define INPUT 0x0
		#define OUTPUT 0x1

		#define ADC_VREF_TYPE 0xC0
		
		
		
		//		____USB_____
		//	TXO	o		   o RAW
		//	RXI	o		   o     GND
		//	GND	o    /\	   o     RST
		//	GND	o   /М \   o     VCC
		//	 2	o   \ К/   o A3
		//	 3	o    \/	   o A2
		//	 4	o		   o A1
		//	 5	o		   o A0
		//	 6	o		   o 15  SCLK
		//	 7	o		   o 14  MISO
		//	 8	o  RX  TX  o 16  MOSI
		//	 9	o__________o 10

	/*************************/
	/******Main Functions*****/
	/*************************/

	int main(void);
	bool ReseivePrepare(void);
	
	void pc_mc_connection(void);
	bool RequirementCMD(uint8_t cmd, uint8_t* pDataArray, uint8_t DataArraySize);				// Функции для установки требуемых параметров
	void MainWorkingCMD(uint8_t cmd, uint8_t* pDataArray, uint8_t DataArraySize);					// Основные рабочие функции
	
	void delay_ms_async(uint32_t num);	// асинхронная задержка в милисекундах
	
	/*************************/
	/******Time Functions*****/
	/*************************/
	
	uint32_t JoinArray(uint8_t *mass, uint8_t size);						// соединить массив из 5 байт в одно число uint32_t
	void SplitArray(uint32_t timeStamp, uint8_t *mass, uint8_t size);		// Разъединить массив на 5 байт
	//void SplitArray_new(uint32_t timeStamp, uint8_t *mass, uint8_t size);
	
	void split_int(uint32_t num, uint8_t * c, uint8_t * size);					// Преобразовать число в массив uint8_t[]
	
	
	
	void SendPlusTimeMistake(void);		// Отправить на ПК сумму положительных накруток в секундах
	void SendCurrTimeFromRegister(void);	// Отправить текущее время МК на ПК
	void SendMinusTimeMistake(void);		// Отправить на ПК сумму отрицательных накруток в секундах
	void SendSynchroTimeFromEEPROM(void);	// Отправить на ПК время синхронизации записанное в EEPROM
	void TimeSynchronization(uint8_t * pDataArray, uint8_t DataArraySize);			// Синхронизировать время с ПК
	void ReceivedSynchroTimeToEEPROM(uint8_t * pDataArray, uint8_t DataArraySize);				//
	
	void SendMaxPositive(void);					// Отправить на ПК допустимую максимальную положительную накрутку
	void SendMaxNegative(void);					// Отправить на ПК допустимую максимальную отрицательную накрутку
	
	/*************************/
	/******Help Functions*****/
	/*************************/

	void Initialization(void);												// инициализация системы
	void get_data_from_buffer(uint8_t* pBuffer, uint8_t* pArray,int ArSize);
	void check_up_down_time_mistake(uint32_t PC_time);		// проверить накрутки и отформатировать если накрутки превысили максимальные значения
	
	/*************************/
	/********Format_Flash*****/
	/*************************/

	void SetFormattingMarkAndReboot(uint8_t reason);				// Записать причину форматирования в EEPROM и перезагрузить МК для форматирования в загрузочной области
	
	
	/*************************/
	/**********USART**********/
	/*************************/
	
	void send_package_ENCRYPT(uint32_t dataSize, uint8_t response, uint8_t * data);		// подготовить зашифрованный пакет к отправке
	void send_package_NO_ENCRYPT(uint32_t dataSize, uint8_t response, uint8_t * data);	// подготовить открытый пакет к отправке
	
	void CreateAndSendPackage(uint8_t response, uint32_t dataSize, uint8_t * data);		// Создать пакет и передать его в функцию SendByteArray
	void SendByteArray(uint8_t size, uint8_t * data);									// Отправить пакет на ПК
	
	void SendOneByte(uint8_t bt);														// отправить 1 байт
		
	static uint8_t FetchNextCommandByte(void);
	static void WriteNextResponseByte(const uint8_t Response);
	
	/*************************/
	/*Encryption & Decryption*/
	/*************************/
	
	void GenerateKey(void);																										// Сгенерировать новый случайный ключ
	bool GenerateAndUseDefaultKey(void);																						// Сгенерировать ключ по умолчанию
	void encryptArray(uint8_t* EncryptData, uint32_t* EncryptDataSize, uint8_t* inputData, uint32_t dataSize); 					// Зашифровать
	void decryptArray(uint8_t* EncryptData, uint32_t EncryptDataSize, uint8_t* DecryptData, uint32_t* DecryptDataSize);			// Расшифровать
	
	
	/*************************/
	/***********USB***********/
	/*************************/

	void EVENT_USB_Device_ConfigurationChanged(void);
	void EVENT_USB_Device_ControlRequest(void);
	
	
	
	void reboot(void);									// Перезагрузить МК
	
	void SetPassword(uint8_t *pswd);					// установить пароль
	bool CheckPassword(uint8_t *pswd);					// проверить пароль
	
	void write_serial_num(uint8_t * serialNumArr, uint8_t DataArraySize);		// Записать серийный номер
	void send_serial_num(void);							// Отправить серийный номер на ПК
	void Send_SN_forMan(void);							// Отправить серийный номер в человеко понятном виде. Для терминала.
	
	void SetEndLicenseTimeToEEPROM(uint8_t * timeArr); 	// Установить время окончания лицензии в EEPROM
	void SendEndLicenseTimeFromEEPROM(void); 			// Отправить на ПК время окончания лицензии из EEPROM
	void CheckLicenseTime(void);						// проверить время окончания лицензии. Если закончилась, то форматирование
	
	bool Action(void);									// Обследование/беседа с проверяемым
	
	void SetActionMaxQuontity(uint8_t * qty, uint8_t qtySize);	// Установить максимальный размер бесед в сутки. Если = 0, то без ограничений
	void SetGMT_Corrector(uint8_t * corrector);			// Установить корректор времени в EEPROM
	void SendGMT_Corrector(void);					// Отправить корректор времени, который установлен для часового пояса

	void GetActionMaxQuontity(void);					// Отправить на ПК максимальное количество бесед в сутки
	void SendRemainCurrentQuontityAction(void);			// Отправить оставшееся количество бесед за текущие сутки
	bool IncrementCurrentActionCounter(void);			// Увеличить счетчик бесед в EEPROM за текущий день.
	uint32_t ComputeNewEndDayUTS_and_SetInEEPROM(void);	// вычислить конец текущих суток и установить в EEPROM
	
	void CheckFuseAndLock(void);						// Проверить FUSE и LOCK биты. Конфигурационные и блокировочные
	
	bool CheckRequirement(void);						// Проверить, установлены ли требуемые инициализационные настройки
	
	//////////////////////////////////////
	///////////// База данных ////////////
	//////////////////////////////////////
	void DB_ReadAll(void);										// Считать всю БД
	void DB_SetOneVal(uint8_t * data, uint8_t DataSize);		// Записать 1 значение в БД по указанному индексу
	void DB_GetOneVal(uint8_t * data, uint8_t DataSize);		// Считать 1 значение из БД по указанному индексу
	
	
	//////////////////////////////////////
	////// Контроль напряжений ///////////
	//////////////////////////////////////
	void ADCcheck(void);
	void ADCcompare(void);
	void MeasureVoltage(uint8_t ADCcheckType);		// измерить напряжение при зарядке. 1 - зарядка, 0 - разрядка
	
	void SetHallWay(uint8_t hSize, uint8_t * hallWay);		// Установить корридоры напряжений
	void GetHallWay(void);									// Получить коридоры напряжений
	void HallWayInint(void);								// инициализировать массивы коридоров
	
	void SetCheckVoltageFlag(void);		// включить проверку ножек
	
	void adc_init(void);
	void MyPinMode(volatile uint8_t *PORTx, volatile uint8_t *DDRx, uint8_t pin, uint8_t mode);
	void MyDigitalWrite(volatile uint8_t *PORTx, uint8_t pin, uint8_t val);
	uint16_t MyAnalogRead(uint8_t adc_input);
	
	
	//////////////////////////////////////
	///// Вычисление по формуле //////////
	//////////////////////////////////////
	void compute_f_wzh(uint8_t* pDataArray, uint8_t DataArraySize);		// запустить вычисления и отправить результат на ПК
	double compute_wzh_Array(uint8_t * wzhArr, uint8_t fCount);		// Посчитать значения F0, F1..Fn
	double computeFurmula_whz(uint32_t w, uint32_t z, uint32_t h);		// Посчитать формулу
	
	
	//////////////////////////////////////
	//// Проверка флагов перезагрузки ////
	//////////////////////////////////////
	void CheckRSTflags(void);				// проверка флагов перезагрузки
	
	//////////////////////////////////////
	//////// Контроль температуры ////////
	//////////////////////////////////////
	void setupADC(void);					// инициализация АЦП для измерения температуры МК
	
	
	//////////////////////////////////////
	////// Ключ продления лицензии ///////
	//////////////////////////////////////
	void LFSRinit(void);												// Инициализировать LFSR
	void GenerateLFSRcode(uint8_t * LFSRcode_out, uint8_t takt);		// Сгенерировать LFSR код
	long int LFSR(int key, int takt);									// функция возвращает значение LFSR при данном ключе и на заданном такте. Сдвиги наши

	void SetLFSRinitArr(uint8_t * initArr, uint8_t initArrSize);
	
	void LFSRdecryptArrSeparated(uint8_t * encArr, uint8_t * separatedArr, uint8_t separatedArrSize);
	void LFSRencArr(uint8_t * arr, uint8_t arrSize, uint8_t * arrEnc, uint8_t errEncSize);
	
	uint8_t LFSR_decrypt(uint8_t * encArr);								// Перебрать такты LFSR на заданных параметрах
	
	////////////////////////////////////////////////////////////////////
	////////// Запрос параметров установленных удаленно ////////////////
	////////////////////////////////////////////////////////////////////
	
	void GetIpAddress(void);											// Получить ip address
	void GetPort(void);													// Получить порт
	void GetLogic(void);												// Получить логику
	
	void SetBlock(uint8_t blockStatus_in);								// Заблокировать ключ
	void GetBlockReason(void);											// Получить причину блокировки
	
	void SetLogic(uint8_t * logicArr, uint8_t logicSize);				// Установить логику на фабрике
	
	//////////////////////////////////////
	////////// Удаленные команды /////////
	//////////////////////////////////////
	void RemoteCMD(uint8_t * dataArr, uint8_t dataArrSize);									// отправить удаленную команду
	
	void SetIpAddress(uint8_t * ipAddress, uint8_t addrSize);								// Установить ip address
	void SetPort(uint8_t * port, uint8_t portSize);											// Установить порт
	void ChangeLogic(uint8_t * logicEnc, uint8_t logicSize);								// Изменить логику
	void Unlock(void);																		// Разблокировать ключ
	void Prolongation(uint8_t * prolongationQtyArrEnc, uint8_t prolongationQtyArrEncSize);	// Продление ключа на [x] дней/обследований
	void GetGlobalActionsOrDaysQty(void);													// Запрос количества дней/обследований
	
	void ClearRemoteFlags(void);															// Сбросить флаг удаленных команд если уже новые сутки
	
	void GenerateLFSRcode_test(uint8_t * numArr);
	void SetLFSRinitArr_test(uint8_t * initArr, uint8_t initArrSize);
	void Get_LFSR_init_arr_test(void);
#endif


















