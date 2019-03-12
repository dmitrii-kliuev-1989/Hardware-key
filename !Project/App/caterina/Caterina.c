#define  INCLUDE_FROM_CATERINA_C
#include "Caterina.h"

static CDC_LineEncoding_t LineEncoding = { .BaudRateBPS = 0,
                                           .CharFormat  = CDC_LINEENCODING_OneStopBit,
                                           .ParityType  = CDC_PARITY_None,
                                           .DataBits    = 8                            };

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Команды ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///////////////////////////// Человекопонятные ///////////////////////////////////
#define handshake_cmd                      			'Z'  // поздороваться
#define Send_SN_forMan_cmd 							's'	 // отправить человекопонятный S/N
#define enableCheckVoltageFlag_cmd          		'v'  // активировать проверку напряжений
#define MeasureVoltage_c_cmd						'c'	 // измерить зарядку
#define MeasureVoltage_d_cmd						'd'	 // измерить разрядку

//////////////////////////////// Числовые ////////////////////////////////////////

// Action. Обследование/беседа с проверяемым
#define action_cmd 			                		'U'  // провести беседу
#define SetActionMaxQuontity_cmd            		'H'  // Установить максимально количество бесед в сутки
#define GetActionMaxQuontity_cmd            		'G'  // Получить максимальное количество бесед в сутки
#define SetActionTimeCorrector_cmd          		'Q'  // Установить корректор времени для часового пояса
#define GetActionTimeCorrector_cmd          		'J'  // Получить корректор времени для часового пояса
#define GetRemainCurrentQtyAction_cmd       		'R'  // Получить текущее оставшееся количество бесед

// Время лицензии
#define set_endLicenseTime_cmd 						'T'  // Установить время окончания лицензии в EEPROM
#define get_endLicenseTime_cmd              		'E'  // Получить время окончания лицензии из EEPROM

// Серийный номер
#define writeSerNum_cmd                     		'W'  // Записать серийный номер в EEPROM
#define getSerNum_cmd                       		'S'  // Получить серийный номер из EEPROM

// Пароль
#define set_pasword_cmd                     		'P'  // Установить пароль
#define send_password_cmd                   		'A'  // Авторизация
#define use_default_key_cmd                 		'Y'  // Использовать ключ по умолчанию.

// Время
#define timing_cmd                          		'+'  // синхронизировать время МК с ПК
#define get_time_cmd                        		'?'  // получить время МК
#define GetPlusTimeMistake_cmd              		'F'  // получить положительную накрутку
#define SendMinusTimeMistake_cmd            		'M'  // Отправить на ПК сумму отрицательных накруток в секундах
#define GetSynchroTimeFromEEPROM_cmd        		'I'  // получить время синхронизации из EEPROM  
#define SendMaxPositive_cmd                 		'D'  // получить максимально возможную положительную накрутку
#define SendMaxNegative_cmd                 		'K'  // получить максимально возможную отрицательную накрутку

// Шифрование
#define GenerateKey_cmd                     		'4'  // получить сгенерированный случайный ключ

// Флэш
#define FORMAT_FLASH_cmd                    		'N'  // Отформатировать flash

// База данных
#define DB_Read_All_cmd                     		'5'  // Получить всю БД
#define DB_SetOneVal_cmd                    		'6'  // Записать 1 байт по ключу
#define DB_GetOneVal_cmd                    		'7'  // Прочитать 1 байт по ключу

// Посчитать по формуле
#define compute_f_wzh_cmd                   		'V'

// Контроль напряжений
#define setHallWay_cmd                      		'h'  	// записать коридор в память EEPROM 
#define getHallWay_cmd                      		'g'  	// считать коридор

// Дистанционное продление лицензии
#define setLFSRinitArr_cmd                  		'b'  	// Инициализировать LFSR

#define getIpAddress_cmd  							't'  	// получить ip адрес
#define getPort_cmd  								'w'  	// Получить порт

#define getLogic_cmd								'!'	 	// Получить логику

#define block_cmd  									'#'  	// Заблокировать ключ
#define setLogic_cmd								'$'		// Установить логику на фабрике


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Удаленные команды
#define remote_cmd  								'r'  // Отправить удаленную команду

// Работа с ip адресом и портом
#define setIpAddress_cmd							1	// Установить ip адрес
#define setPort_cmd  								2  	// Установить порт
#define unlock_cmd  								3  	// Разблокировать ключ
#define changeLogic_cmd  							4  	// Изменить логику. дни/обследования
#define prolongation_cmd  							5  	// Продлить ключ на количество дней/обследований
#define getActionsOrDays_cmd  						6  	// Получить количество дней/обследований
#define getBlockReason_cmd							7  	// Получить причину блокировки

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Ответы от МК //////////////////////////////////////
static uint8_t confirmation = 						0;	// подтверждение
static uint8_t deny = 								1;	// отказ. Не верный пароль
static uint8_t unknownCommand = 					2;	// неизвестная команда
static uint8_t maxQtyActionIsNotExists =			3;	// Максимальное количество бесед в сутки не установлено
static uint8_t noGMT_Corrector = 					4;	// Корректор времени для GMT не установлен
static uint8_t timeNotSync = 						5;	// Время не синхронизировано
static uint8_t badEndDay = 							6;	// Не удалось вычислить конец суток

static uint8_t maxActionQtyIsExists = 				8;	// Максимальное количество действий в сутки уже установлено
static uint8_t timeCorrectorExceedsMaximum = 		9;	// Устанавливаемый корректор времени превышыет допустимый максимум
static uint8_t timeCorrecorIsExists		= 			10;	// Корректор времени уже установлен. Ошибка повторной установки при имеющемся корректоре
static uint8_t timeCorrectorNotInstalled = 			11;	// Не удалось установить корректор при первичной инициализации
static uint8_t timeCorrectorIsNotExists = 			12;	// Корректор времени не установлен
static uint8_t endLicTimeIsExists = 				13;	// Время окончания лицензии уже установлено
static uint8_t endLicTimeIsNotExists = 				14;	// Время окончания лицензии не установлено
static uint8_t passwordIsExists = 					15;	// Пароль уже установлен
static uint8_t passwordIsNotSet = 					16;	// Не установлен пароль в EEPROM
static uint8_t serialNumIsExists = 					17;	// Серийный номер уже установлен
static uint8_t serialNumIsNotExists = 				18;	// Серийный номер не установлен


static uint8_t MaxPositiveIsNotExists = 			21;	// Максимально положительная накрутка не установлена
static uint8_t MaxNegativeIsNotExists = 			22; // Максимально отрицательная накрутка не установлена
static uint8_t tooMuchData = 						23; // Слишком много данных. При команде от ПК, количество данных оказалось больше их размера
static uint8_t dbOutOfRange = 						24; // Выход за пределы размера БД.
static uint8_t dbValIsNotExists = 					25; // Значение БД не существует. Ошибка при перезаписи элемента БД
static uint8_t badWZHsize = 						26; // Неверное количество данных для вычисления wzh
static uint8_t serialNumBadSize = 					27; // Слишком большой серийный номер. Максимум 10.
static uint8_t badHallWaySize = 					28;	// Неверное количество значений для коридоров
static uint8_t badSetHallWay =						29;	// Ошибка при записи коридора в EEPROM
static uint8_t checkVoltageFlagIsExists = 			30;	// Флаг проверки напряжений уже установлен. Установка коридоров запрещена
static uint8_t badLFSRInitArrSize = 				31; // Не верное количество параметров в массиве инициализации LSFR. Необходим массив g_LFSRinitArrSize байт
static uint8_t badIpAddrSize = 						32;	// Не верное количество байтов в ip адресе
static uint8_t ipAddrIsNotSet =						33; // ip адрес не задан
static uint8_t badPortSize = 						34;	// Передаваемый массив порта не может быть нулевым
static uint8_t portIsNotSet =						35;	// Порт не установлен
static uint8_t LFSRSettingsIsNotSet =				36; // Настройки LFSR не установлены
static uint8_t badLogicSize = 						37; // Не верный размер массива логики. Размер должен быть g_logicSize байт
static uint8_t undefinedLogic =						38; // Не известный код логики
static uint8_t logicIsNotSet =						39; // Логика не установлена
static uint8_t undefinedRemoteCommand = 			40;	// Неизвестная удаленная команда
static uint8_t LFSRsettingsAlredySet =				41;	// Инициализационные настройки LFSR уже заданы
static uint8_t endActionsInCurrDay =				42;	// В текущем дне исчерпаны все беседы
static uint8_t badProlongationArrSize =				43;	// Не верный размер массива продления лицензии
static uint8_t maxQtyActionsExceedsLimit =			44;	// Попытка установить количество действий(обследований) с превышением границы
static uint8_t prolongationQtyExceedsLimit =		45;	// Попытка продлить лицензию на количество дней/обследований с превышением границы
static uint8_t dongleIsBlocked =					46;	// Ключ заблокирован
static uint8_t maxQtyDaysExceedsLimit =				47;	// Попытка установить количество дней лицензии с превышением границы
static uint8_t badDaysLogicSize =					48;	// Не верное количество элементов в массиве смены логики на дни
static uint8_t badActionsLogicSize =				49;	// Неверное количество элементов в массиве смены логики на обследования
static uint8_t logicAlreadyExists =					50;	// Логика уже установлена
static uint8_t badLFSRsymbol =						51;	// В LFSR коде встретился не допустимый элемент. Элементы LFSR кода должны быть в диапазоне 0..9
static uint8_t badZeroUTS =							52;	// zeroUTS не установлено
static uint8_t logicAlreadyChangedInCurrDay =		53;	// Логика уже изменялась в текущих сутках
static uint8_t alreadyProlongationInCurrDay =		54;	// Продление лицензии уже производилось в текущих сутках
static uint8_t alreadyUnlockInCurrDay =				55;	// Разблокировка уже производилась в текущем дне
static uint8_t endLicTimeNeedNot =					56;	// Установка времени окончания лицензии не нужна т.к. логика обследования

static uint8_t timeoutError = 						0xFF;// ошибка зависания

//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Причины форматирования ///////////////////////////
//////////////////////////////////////////////////////////////////////////////////
static uint8_t bad_L_FUSE = 			65;	// Не верный L - конфигурационный байт
static uint8_t bad_H_FUSE = 			66;	// Не верный H - конфигурационный байт
static uint8_t bad_E_FUSE = 			67;	// Не верный E - конфигурационный байт
static uint8_t bad_LOCK_BITS = 			68;	// Не верный LOCK - блокировочный байт
static uint8_t badPassword = 			69;	// Не верно введен пароль N раз
//static uint8_t formatCommand = 		70;	// Команда форматирования от пользователя

static uint8_t badCMDhack = 			74; // Превышено число не верных команд
static uint8_t EXTRF_isSet = 			75;	// Аппаратный сброс МК. Замыкание ножки RST на GND

static uint8_t chargeHack = 			77;	// Подключено внешнее устройство при зарядке ножки PORTF.7
static uint8_t dischargeHack = 			78;	// Подключено внешнее устройство при разрядке ножки PORTF.7
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Причины блокировки ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
static uint8_t endLicense =				71;	// Истекло время лицензии для типа лицензии Days
static uint8_t timeHackPlus	=			72;	// Превышено число положительных накруток времени
static uint8_t timeHackMinus =			73;	// Превышено число отрицательных накруток времени

static uint8_t userBlock = 				79; // Блокировка от пользователя для отладки
static uint8_t unableToDecipherTheLFSRcommand = 80;	// Не удалось расшифровать команду LFSR
static uint8_t endActions = 			81;	// Исчерпано количество действий для типа лицнзии Actions
static uint8_t undefinedRemoteCMD = 	82;	// Неизвестная удаленная команда

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
static uint8_t NO_DATA[0];								// пустой массив для команд, которые ничего не должны возвращать

//////////////////////////// Счетчик несуществующих команд ///////////////////////
static uint8_t UnknownCMDcounter = 0;				// Счетчик
static uint8_t MaxUnknownCMD = 5;					// Максимальное количество не верных команд за всю работу ключа
static uint16_t UnknownCMDcounterEEPROMaddr = 40;	// Адрес счетчика не верных команд

////////////////////////////////// Работа с паролем //////////////////////////////
static bool IsAuthorized = false;			// Пользователь прошел авторизацию?
static uint8_t PasswordSize = 40;			// Размер пароля
static uint16_t PasswordEEPROMaddr = 100;	// Стартовый адрес расположения пароля

static uint8_t NumberOfLogins = 1;			// Количество попыток авторизации
static uint8_t MaxNumberOfLogins = 10;		// Максимальное количество авторизаций

//////////////////////////////// Подсчет и ограничение бесед ////////////////////////////////
static uint16_t ActionCounterEEPROMaddr = 80;			// Адрес счетчика бесед в EEPROM
static uint16_t ActionMaxQuontityEEPROMaddr = 82;		// Адрес максимального кол-ва бесед в EEPROM
static uint16_t GMT_CorrectorEEPROMaddr = 85;			// адресс корректора в EEPROM
static uint16_t ActionEndDayUTS_EEPROMaddr = 90;		// Адрес расположения конца суток в секундах
static uint16_t GMT_CorrectorMaximim = 43200;			// Максимальный корректор. Минимальный = 0.
static uint32_t DayInSec = 86400;						// количество секунд в сутках
bool ActionQtyExhausted = true;							// Количество действий в сутки исчерпано

/////////////////////////////// Конфигурационные и блокировочные биты ///////////////////////
uint8_t RELEASE_L_FUSE = 0xFF;
uint8_t RELEASE_H_FUSE = 0xD8;
uint8_t RELEASE_E_FUSE = 0xC8;
uint8_t RELEASE_LOCK_BITS = 0xFC;


///// Не форматируемые данные EEPROM
static uint16_t unixTimeStampEEPROMaddr = 10;	// адрес времени синхронизации
static uint16_t serialNumEEPROMaddr = 15; 		// адрес серийного номера в EEPROM
static uint16_t formattingMarkEEPROMaddr = 30;	// адрес метки форматирования

/////////////////////////////// Время ///////////////////////////////////////////////////////
/* 	Старт->инициализация
	1) Считаем в прерывании до 1000 в переменной timeCounter
	2) как только досчитали, то установить nextTick в true -> обнулить timeCounter
	3) в CDC_Task по условию if(nextTick) передать время на ПК, установить nextTick в false
	-> (1)
*/

bool FirstTimeSync = false;						// Флаг самой первой синхронизации времени
bool TimeSynchronized = false;					// синхронизированно ли время?

static uint32_t g_unixTimeStamp = 0;				// На старте МК время берем из EEPROM

static uint8_t UTSarrSize = 5;					// длина массива времени
bool nextTick = false;							// переменная для обозначения тика в 1 сек.

static uint16_t mc_Sec = 997;		// Количество милисекунд в секунде микроконтроллера
static uint16_t timeCounter = 1;	// счетчик милисекунд.

uint8_t correctorValue = 2;			// Величина корректировки. То есть если = N, то к каждой Nй секунде добавляем 1 милисекунду
uint8_t timeCorrectorCounter = 1;	// Счетчик корректора времени

/////////////////////////////////////////////////////////////////////////////////////////////
// Работа с серийным номером
uint8_t sizeSerNum = 10;	// размер серийного номера

// Работа с временем жизни лицензии
static uint16_t LicenseTime_EEPROMaddr = 200;	//	Адрес окончания лицензии

//////////////// База данных /////////////////////////////////
uint16_t DataBaseEEPROMAddr = 250;
uint8_t g_dbSize = 32;

//////////////////////////////////////////////////////////////

const uint16_t PlusTimeMistakeEEPROMaddr = 150;
const uint16_t MinusTimeMistakeEEPROMaddr = 155;
static uint32_t g_MaxPositive = 0;	// максимальное количество положительных накруток
static uint32_t g_MaxNegative = 0;	// максимальное количество отрицательных накруток
static uint8_t g_MaxPositivePercent = 102;
static uint8_t g_MaxNegativePercent = 2;
uint16_t MaxPositiveEEPROMaddr = 70;
uint16_t MaxNegativeEEPROMaddr = 74;


///////////////////////////////////////////////////////////////
// Адреса причин перезагрузки в EEPROM
static uint16_t EXTRF_EEPROMaddr = 170;		// аппаратный ресет
bool g_isInit = false;						// флаг инициализации времени для проверки флагов

// Шифрование
uint8_t g_TimeKeySize = 32;

// дефолтный нужно ключ вычислять при подаче питания.
// Если пароль еще не установлен, то использовать заводской ключ.
// Slave ключ будет формироваться на основе времени.
uint8_t g_TimeKey[32] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0};	// присвоение заводского ключа
						   
bool keyIsDefault = false;

///////////////////////////////////////////////////////////////
// Переменные для контроля напряжений
uint8_t g_checkPin = 7; // A0
uint8_t g_iteration = 0;
uint8_t g_dly = 0;
uint8_t g_ADCcheckType = 1;
uint32_t g_ADCtimer = 0;		// счетчик времени в мс для контроля ножек
bool g_cmdStart = false;		// запущен ли блок исполнения команды

uint16_t g_checkVoltageFlagEEPROMaddr = 240; // адрес флага проверки напряжения
bool g_checkVoltageFlag = false;	// флаг проверки напряжения. true - проверяем, false - нет.

uint16_t g_HallWayEEPROMaddr = 320;		// адрес нулевого байта коридора
uint8_t g_HallWaySize = 32;				// количество всех значений в коридоре

// Зарядка
uint8_t g_chargeMin[8];
uint8_t g_chargeMax[8];

// Разрядка
uint8_t g_dischargeMin[8];
uint8_t g_dischargeMax[8];


///////////////////////////////////////////////////////////////
// Дистанционное продление лицензии
uint16_t g_LFSRinitArrEEPROMaddr = 360;
uint8_t g_LFSRinitArrSize = 11;

uint16_t g_LFSRkey = 1;
uint8_t g_LFSRshiftArr[] = { 0, 0, 0, 0, 0, 0, 0, 0 };	// Алгоритм работает при значениях сдвигов в диапазоне 1..30
uint8_t g_LFSR_codeSize = 8;

///////////////////////////////////////////////////////////////
// ip адрес и порт
uint16_t g_IpAddressEEPROMaddr = 375;
uint16_t g_PortEEPROMaddr = 379;
uint8_t g_ipAddrSize = 4;
uint8_t g_ipAddrSizeEnc = 64;

uint8_t g_portSize = 3;
uint8_t g_portSizeEnc = 24;


///////////////////////////////////////////////////////////////
// Установка/изменение логики дни/обследования
uint8_t g_logicSize = 8;
uint16_t g_logicEEPROMaddr = 385;

uint8_t g_currentLogic = 0;
uint8_t g_daysLogic = 1;
uint8_t g_actionsLogic = 2;

///////////////////////////////////////////////////////////////
// глобальный подсчет действий за период
uint16_t g_globalActionsCounterEEPROMaddr = 390;

///////////////////////////////////////////////////////////////
// Блокировка
uint16_t g_blockReasonEEPROMaddr = 400;
uint8_t g_blockReason = 0;								// причина блокировки

///////////////////////////////////////////////////////////////
static uint16_t g_firstUTS_EEPROMaddr = 415;	// адрес самой первой сихронизации времени
static uint16_t g_zeroUTS_EEPROMaddr = 420;		// время начала суток первой синхронизации (dd.mm.yyyy 00:00:00)

///////////////////////////////////////////////////////////////
// Продление лицензии
uint8_t g_prolongationArrSize = 16;
static uint16_t g_MaxActionOrDayQty = 1000;	// Максимально возможно количество дней/обследований на которое можно продлить или установить лицензию

///////////////////////////////////////////////////////////////
// Ограничение на выполнение удаленных команд внутри суток
static uint16_t g_changeLogicFlagEEPROMaddr = 427;
static uint16_t g_prolongationFlagEEPROMaddr = 428;
static uint16_t g_unlockFlagEEPROMaddr = 429;

/*###################################################################################################################*/
/*############################################ MAIN #################################################################*/
/*###################################################################################################################*/
/*###################################################################################################################*/
int main(void)
{
	Initialization();

	while (true)
	{
		g_cmdStart = true;
		
		CheckLicenseTime();
		CheckRSTflags();
		pc_mc_connection();	// Время выполнения должно быть менее 30 мс. Так же не должно превышать 10 сек. Протестировано на _delay_ms(10000).
		
		USB_USBTask();		// Задача USB должна вызываться не реже 30 мс в режиме устройства. Задача должна работать всегда.

		g_cmdStart = false;
		g_dly += g_ADCtimer;
		g_ADCtimer = 0;
		
		ADCcheck();
	}

	return 0;
}

bool CheckRequirement()
{
	// Для работы МК сначала требовать следующее:
	
	// 0) Требовать установку логики
	uint8_t logic = eeprom_read_byte((uint8_t*)g_logicEEPROMaddr);
	if(logic == 0xFF)
	{
		send_package_ENCRYPT(0, logicIsNotSet, NO_DATA);
		return false;
	}
	
	// 1) Требовать синхронизации времени.
	if (!TimeSynchronized)
	{
		send_package_ENCRYPT(0, timeNotSync, NO_DATA);
		return false;
	}
	
	// 2) Требовать установленного пароля в EEPROM
	uint8_t pswdSetByte = eeprom_read_byte((uint8_t *)PasswordEEPROMaddr);
	if(pswdSetByte == 0xFF)
	{
		send_package_ENCRYPT(0, passwordIsNotSet, NO_DATA);
		return false;
	}
	
	// 3) Требовать установленный серийный номер
	uint8_t num = eeprom_read_byte((uint8_t*) serialNumEEPROMaddr);
	if (num == 0xFF)
	{
		send_package_ENCRYPT(0, serialNumIsNotExists, NO_DATA);
		return false;
	}
	
	// 4) Требовать установленнго времени окончания лицензии
	if (logic == g_daysLogic)
	{
		uint32_t endLicTime = eeprom_read_dword((uint32_t*) LicenseTime_EEPROMaddr);
		if (endLicTime == 0xFFFFFFFF) 
		{
			send_package_ENCRYPT(0, endLicTimeIsNotExists, NO_DATA);
			return false;
		}
	}
	
	// 5) Требовать установленного максимального количества бесед
	uint16_t maxActionQty = eeprom_read_word((uint16_t*)ActionMaxQuontityEEPROMaddr);
	if (maxActionQty == 0xFFFF) 
	{
		send_package_ENCRYPT(0, maxQtyActionIsNotExists, NO_DATA);
		return false;
	}	
	
	// 6) Требовать установки корректора часового пояса
	uint32_t corrent_GMT_Corrector = eeprom_read_dword((uint32_t * )GMT_CorrectorEEPROMaddr);
	if(corrent_GMT_Corrector == 0xFFFFFFFF)
	{
		send_package_ENCRYPT(0, timeCorrectorIsNotExists, NO_DATA);
		return false;
	}
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!! Пока эти требования не выполнены, то давать, только handshake и установку величин, которые требуются !!!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Тогда и только тогда, когда они установлены, мы разрешаем работать
	return true;
}

/*********************************************************************************************************************/
/**********************************************MAIN_FUNCTIONS*********************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

void pc_mc_connection(void)
{
	if (ReseivePrepare() != true) return;	// подготовка к приёму
	
	uint8_t firstByte = FetchNextCommandByte();
	if (firstByte == timeoutError) { SendOneByte(timeoutError); return; }
	
	switch(firstByte)
	{
		case handshake_cmd:
				SendOneByte(handshake_cmd);
			break;
			
		case Send_SN_forMan_cmd:
				Send_SN_forMan();
				return;
			break;
			
		case enableCheckVoltageFlag_cmd:
				SetCheckVoltageFlag();
				return;
			break;
			
		case MeasureVoltage_c_cmd:
				MeasureVoltage(1);
				return;
			break;
			
		case MeasureVoltage_d_cmd:
				MeasureVoltage(0);
				return;
			break;

		default:
			return;
	}

	// Команда не шифруется. 0-вой байт
	uint8_t command = FetchNextCommandByte();
	if (command == timeoutError) { SendOneByte(timeoutError); return; }
	
	uint8_t dsArrSize = 3;
	uint8_t dataSizeArray[dsArrSize];
	
	// Размер принимаемой последовательности не шифруется.
	// 1й, 2й, 3й байты - размер
	for(uint8_t i = 0; i < dsArrSize; i++)
	{
		dataSizeArray[i] = FetchNextCommandByte();
		if (dataSizeArray[i] == timeoutError) { SendOneByte(timeoutError); return;}
	}
	
	uint32_t dataSize = JoinArray(dataSizeArray, dsArrSize); // вычислить размер
	
	//////////////////////////////////////////////////////
	// Если по данной команде приходят шифрованные данные, то расшифровываем
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Расшифрование

	uint32_t EncryptDataSize = dataSize;
	uint8_t EncryptData[EncryptDataSize];
	
	if (EncryptDataSize != 0)
	{
		for(int i = 0; i < EncryptDataSize; i++)
			EncryptData[i] = FetchNextCommandByte();
	}

	// Игнорировать данные, которые по количеству выходят за рамки размера данных
	// Проверить. Есть ли в буфере еще входные данные
	uint8_t trashByte = FetchNextCommandByte();
	if(trashByte != 0xFF)
	{
		send_package_ENCRYPT(0, tooMuchData, NO_DATA);
		return;
	}

	uint8_t DecryptData[255];
	uint32_t DecryptDataSize = 0;
	for (uint16_t k = 0; k < 255; k++) DecryptData[k] = 0;	// ОБЯЗАТЕЛЬНО! Нужно обнулять этот массив т.к. из-за особонности avr Си он хранит предыдущий мусор.

	decryptArray(EncryptData, EncryptDataSize, DecryptData, (uint32_t*)&DecryptDataSize);
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	
	uint8_t data[DecryptDataSize];					//N byte == Read received data
	if (DecryptDataSize != 0)
	{
		for(int i = 0; i < DecryptDataSize; i++)
		{
			data[i] = DecryptData[i];
		}
	}

	if(command != setLFSRinitArr_cmd) // проверить настройки LFSR и если стоят заводские, то return
	{	
		bool lfsrSettings = true;

		if(g_LFSRkey == 1)
			lfsrSettings = false;
		
		for(uint8_t i = 0; i < 8; i++)
		{
			if(g_LFSRshiftArr[i] == 0)
			{
				lfsrSettings = false;
				break;
			}
		}

		if(!lfsrSettings)
		{
			send_package_ENCRYPT(0, LFSRSettingsIsNotSet, NO_DATA);
			return;
		}
	}

	if (command == use_default_key_cmd)	// сбросить ключ на дефолтный если пароль существует
	{
		if(GenerateAndUseDefaultKey())
			send_package_ENCRYPT(0, confirmation, NO_DATA);
		else
			send_package_ENCRYPT(0, passwordIsNotSet, NO_DATA);

		return;
	}
	
	// функции, которые установят требуемые параметры
	if(RequirementCMD(command, data, DecryptDataSize)) 	// если команда из этого списка, то 
		return;										// выходим, что бы не отсылать больше пакетов

	if(CheckRequirement()) 							// если требуемые установки сделаны, то разрешить главные команды
		MainWorkingCMD(command, data, DecryptDataSize);	// основные рабочие команды. Сюда попадаем, только если требуемые настройки установленны, а иначе не проходит проверка выше и на ПК отправляется ошибка
	else
		return;
}

/**********************************************************************************************************************/
/************************************************** TEST !!!!!! *******************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

void SetLFSRinitArr_test(uint8_t * LFSRinitArr, uint8_t initArrSize)
{
	if (initArrSize != g_LFSRinitArrSize)
	{
		send_package_ENCRYPT(0, badLFSRInitArrSize, NO_DATA);
		return;
	}

	uint8_t keyArr[3];
	for(uint8_t i = 0; i < 3; i++)
		keyArr[i] = LFSRinitArr[i];
	
	g_LFSRkey = JoinArray(keyArr, 3);
	
	for(uint8_t i = 3; i < g_LFSRinitArrSize; i++)
		g_LFSRshiftArr[i - 3] = LFSRinitArr[i];
	
	
	// ответ
	uint8_t LFSRinitArr_out[g_LFSRinitArrSize];
	
	uint8_t keyArr_out[3];
	SplitArray(g_LFSRkey, keyArr_out, 3);
	
	for(uint8_t i = 0; i < 3; i++)
		LFSRinitArr_out[i] = keyArr_out[i];
	
	for(uint8_t i = 0; i < g_LFSRinitArrSize; i++)
		LFSRinitArr_out[i + 3] = g_LFSRshiftArr[i];
	
	send_package_ENCRYPT(g_LFSRinitArrSize, confirmation, LFSRinitArr_out);
}

void GenerateLFSRcode_test(uint8_t * numArr)
{
	uint8_t num = numArr[0];
	
	uint8_t testCode[g_LFSR_codeSize];
	GenerateLFSRcode(testCode, num);
	send_package_ENCRYPT(g_LFSR_codeSize, confirmation, testCode);
}

void Get_LFSR_init_arr_test()
{
	uint8_t LFSRinitArr[g_LFSRinitArrSize];
	
	uint8_t keyArr[3];
	SplitArray(g_LFSRkey, keyArr, 3);
	
	for(uint8_t i = 0; i < 3; i++)
		LFSRinitArr[i] = keyArr[i];
	
	for(uint8_t i = 0; i < g_LFSRinitArrSize; i++)
		LFSRinitArr[i + 3] = g_LFSRshiftArr[i];
	
	send_package_ENCRYPT(g_LFSRinitArrSize, confirmation, LFSRinitArr);
}

/*********************************************************************************************************************/
/**************************** Команды доступные до и после инициализации *********************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
bool RequirementCMD(uint8_t cmd, uint8_t * pDataArray, uint8_t DataArraySize)
{
	switch(cmd)
	{
		case ',':	// тестовая иницализация
			SetLFSRinitArr_test(pDataArray, DataArraySize);
			break;
		
		case '.':	// тест зашифрования
			GenerateLFSRcode_test(pDataArray);
			break;
		
		case ';':	// запрос текущей инициализационного массива
			Get_LFSR_init_arr_test();
			break;
			
		case '*':	// запрос zeroUTS
			{
				uint32_t zeroUTS = eeprom_read_dword((uint32_t*)g_zeroUTS_EEPROMaddr);
				if(zeroUTS == 0xFFFFFFFF)
				{
					send_package_ENCRYPT(0, badZeroUTS, NO_DATA);	
					break;
				}	
				
				uint8_t zeroUTSarr[UTSarrSize];
				SplitArray(zeroUTS, zeroUTSarr, UTSarrSize);
				send_package_ENCRYPT(UTSarrSize, confirmation, zeroUTSarr);
			}
			break;
			
		case '@':	// запрос времени конца суток
			{
				uint32_t endDayTime = eeprom_read_dword((uint32_t*)ActionEndDayUTS_EEPROMaddr);
				uint8_t endDayTimeArr[UTSarrSize];
				SplitArray(endDayTime, endDayTimeArr, UTSarrSize);
				send_package_ENCRYPT(UTSarrSize, confirmation, endDayTimeArr);
			}
			break;
		
		case '&':	// запрос addKey
		{
			uint32_t zeroUTS = eeprom_read_dword((uint32_t*)g_zeroUTS_EEPROMaddr);
			uint32_t currentUTStoZero = g_unixTimeStamp - (g_unixTimeStamp % DayInSec);
			uint8_t addKey = (currentUTStoZero - zeroUTS) / DayInSec;
			uint8_t addKeyArr[1];
			addKeyArr[0] = addKey;
			
			send_package_ENCRYPT(1, confirmation, addKeyArr);
		}
		break;

		// Блокировка от пользователя тест
		case block_cmd:
				SetBlock(userBlock);
				send_package_ENCRYPT(0, confirmation, NO_DATA);
			break;
		
		// Логика
		case getLogic_cmd:
				GetLogic();
			break;
			
		case setLogic_cmd:
				SetLogic(pDataArray, DataArraySize);
			break;
		
		// Удаленные команды
		case remote_cmd:
			RemoteCMD(pDataArray, DataArraySize);
			break;
		
		// Запрос ip адреса и порта
		case getIpAddress_cmd:
			GetIpAddress();
			break;
		
		case getPort_cmd:
			GetPort();
			break;
		
		case setLFSRinitArr_cmd:
			SetLFSRinitArr(pDataArray, DataArraySize);
			break;

		// 0) Контроль напряжений
		case setHallWay_cmd:
			SetHallWay(DataArraySize, pDataArray);
			break;

		case getHallWay_cmd:
			GetHallWay();
			break;

		// 1) Синхронизация времени, запрос текущего
		case timing_cmd:
				TimeSynchronization(pDataArray, DataArraySize);
			break;

		case get_time_cmd:
				SendCurrTimeFromRegister();
			break;
			
		case GetSynchroTimeFromEEPROM_cmd:
				SendSynchroTimeFromEEPROM();
			break;
			
		case GetPlusTimeMistake_cmd:
				SendPlusTimeMistake();
			break;

		case SendMinusTimeMistake_cmd:
				SendMinusTimeMistake();
			break;
		
		case SendMaxPositive_cmd:
				SendMaxPositive();
			break;

		case SendMaxNegative_cmd:
				SendMaxNegative();
			break;
		
		// 2,3) Установка пароля. Авторизацию вынесли в главные ф-ции
		case set_pasword_cmd:
				SetPassword(pDataArray);
			break;
	
		// 4) Установка серийного номера
		case writeSerNum_cmd:
				write_serial_num(pDataArray, DataArraySize);
			break;
			
		case getSerNum_cmd:
				send_serial_num();
			break;
		
		// 5) Установка времени окончания лицензии
		case set_endLicenseTime_cmd:
				SetEndLicenseTimeToEEPROM(pDataArray);
			break;
			
		case get_endLicenseTime_cmd:
				SendEndLicenseTimeFromEEPROM();
			break;
			
			
		// 6) Установка максимального количества бесед в сутки //////////
		case SetActionMaxQuontity_cmd:
				SetActionMaxQuontity(pDataArray, DataArraySize);
			break;
			
		case GetActionMaxQuontity_cmd:
				GetActionMaxQuontity();
			break;
			
		case GetRemainCurrentQtyAction_cmd:
				SendRemainCurrentQuontityAction();
			break;
		
		// 7) Установка корректора часового пояса
		case SetActionTimeCorrector_cmd:
				SetGMT_Corrector(pDataArray);
			break;
		
		case GetActionTimeCorrector_cmd:
				SendGMT_Corrector();
			break;
		
		////////////////////////////////////
		case GenerateKey_cmd:
				GenerateKey();
			break;
			
		/*case FORMAT_FLASH_cmd: //Format memory
				SetFormattingMarkAndReboot(formatCommand);	// установить флаг форматирования перезагрузить МК для форматирования из загрузочной области
			break;*/
			
		default:
			// Если попали сюда, то никакой пакет на ПК не отправился и команда не из этого списка либо не существует
			// Если команда из рабочего списка (т.е. сейчас ничего не отправляли на ПК), то true и можно делать проверку
			// То есть если команда из инициализационного списка, то не надо делать проверку на установку требуемых параметров
			return false;	// true обозначает что команда не из этого списка
	}
	
	return true;
}


/*********************************************************************************************************************/
/********************************************** Удаленные команды ****************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
void RemoteCMD(uint8_t * dataWithCMD, uint8_t dataSizeWithCMD)
{
	ClearRemoteFlags();
	
	uint8_t cmdArr[g_LFSR_codeSize];
	for(uint8_t i = 0; i < g_LFSR_codeSize; i++)
		cmdArr[i] = dataWithCMD[i];
	
	uint8_t cmd = LFSR_decrypt(cmdArr);	
	uint8_t dataSize = dataSizeWithCMD - g_LFSR_codeSize;
	uint8_t data[dataSize];
	
	for(uint8_t i = 0; i < dataSize; i++)
		data[i] = dataWithCMD[i + g_LFSR_codeSize];
	
	switch(cmd)
	{
		case setIpAddress_cmd:
				SetIpAddress(data, dataSize);
			break;

		case setPort_cmd:
				SetPort(data, dataSize);
			break;

		case unlock_cmd:
				Unlock();
			break;

		case changeLogic_cmd:
				ChangeLogic(data, dataSize);
			break;

		case prolongation_cmd:
			Prolongation(data, dataSize);
			break;

		case getActionsOrDays_cmd:
				GetGlobalActionsOrDaysQty();
			break;
			
		case getBlockReason_cmd:
				GetBlockReason();
			break;

		default:
		{
			send_package_ENCRYPT(0, undefinedRemoteCommand, NO_DATA);
			SetBlock(undefinedRemoteCMD);
		}
			break;
	}
}

void ClearRemoteFlags()
{
	uint32_t currentEndDayUTS = eeprom_read_dword((uint32_t*) ActionEndDayUTS_EEPROMaddr);
	
	if (currentEndDayUTS == 0xFFFFFFFF)
		currentEndDayUTS = ComputeNewEndDayUTS_and_SetInEEPROM();

	// сравниваем с текущим UTS с текущим окончанием суток
	if( g_unixTimeStamp > currentEndDayUTS)	// если уже новые сутки, то сбрасываем флаги удаленных команд
	{
		eeprom_write_byte_safe((uint8_t*) g_changeLogicFlagEEPROMaddr, 0);
		eeprom_write_byte_safe((uint8_t*) g_prolongationFlagEEPROMaddr, 0);
		eeprom_write_byte_safe((uint8_t*) g_unlockFlagEEPROMaddr, 0);
		
		ComputeNewEndDayUTS_and_SetInEEPROM();
	}
}

/*********************************************************************************************************************/
/*********************************** Команды доступные после инициализации *******************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
void MainWorkingCMD(uint8_t cmd, uint8_t* pDataArray, uint8_t DataArraySize)
{
	if(g_blockReason != 0)
	{
		send_package_ENCRYPT(0, dongleIsBlocked, NO_DATA);
		return;
	}
	
	switch (cmd)
	{
		case compute_f_wzh_cmd:
			compute_f_wzh(pDataArray, DataArraySize);
			break;
		
		case send_password_cmd:
				// Авторизация. Ожидаем пароль из PasswordSize байт
				if(CheckPassword(pDataArray))
				{
					IsAuthorized = true;
				}
				else
				{
					if (NumberOfLogins == MaxNumberOfLogins)
						SetFormattingMarkAndReboot(badPassword);
					
					IsAuthorized = false;
					send_package_ENCRYPT(0, deny, NO_DATA);
					NumberOfLogins++;
				}
				
				if(Action() && IsAuthorized)
					send_package_ENCRYPT(0, confirmation, NO_DATA);					
			break;
			
			
		case action_cmd:
				Action();
			break;

		case DB_Read_All_cmd:
				DB_ReadAll();
			break;
			
		case DB_SetOneVal_cmd:
				DB_SetOneVal(pDataArray, DataArraySize);
			break;
			
		case DB_GetOneVal_cmd:
				DB_GetOneVal(pDataArray, DataArraySize);
			break;

        default:
			// Подсчет не верных команд
			send_package_ENCRYPT(0, unknownCommand, NO_DATA);
			UnknownCMDcounter++;
			eeprom_write_byte_safe((uint8_t*) UnknownCMDcounterEEPROMaddr, UnknownCMDcounter);
			if (UnknownCMDcounter == MaxUnknownCMD - 1)
				SetFormattingMarkAndReboot(badCMDhack);

          break;
      }
}

void CheckLicenseTime()
{	
	if(g_currentLogic == g_daysLogic)
	{
		uint32_t endLicenseTime = eeprom_read_dword((uint32_t*)LicenseTime_EEPROMaddr);
		
		if (g_unixTimeStamp > endLicenseTime)
			SetBlock(endLicense);
	}
}

bool ReseivePrepare()
{
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	if (!(Endpoint_IsOUTReceived()))
	  return false;

	return true;
}

/*********************************************************************************************************************/
/**********************************************Проверка флагов перезагрузки*******************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
// Делать проверку флагов только после инициализации(времени в EEPROM)+перезагрузка
// После синхронизации устанавливать в флаг g_isInit
// считывать флаг после перезагрузки и если он установлен, то проверять флаги перезагрузки
void CheckRSTflags()
{
	// если время еще не синхронизировали и его нет в EEPROM, то проверку на флаги не проводить
	if (!g_isInit) return;
		
	uint8_t EXTRF_value = eeprom_read_byte((uint8_t*)EXTRF_EEPROMaddr);
	if (EXTRF_value == 1)
		SetFormattingMarkAndReboot(EXTRF_isSet);
}

void Send_SN_forMan()
{
	uint8_t strSNsize = 15;
	char strSN[15] = "S/N: ";
	
	uint8_t sn = eeprom_read_byte((uint8_t*) serialNumEEPROMaddr);
	if (sn == 0xFF)
	{
		char str_noSN[14] = "SN: undefined.";
		uint8_t no_SN[14];
		for(uint8_t i = 0; i < 14; i++)
			no_SN[i] = str_noSN[i];

		SendByteArray(14, no_SN);
		return;
	}

	uint8_t sn_arr_EEPROM[sizeSerNum];
	eeprom_read_block(sn_arr_EEPROM, (uint8_t*)serialNumEEPROMaddr, sizeSerNum);
	
	for(uint8_t i = 0; i < sizeSerNum; i++) strSN[i + 5] = sn_arr_EEPROM[i];

	uint8_t outArr[strSNsize];
	for(uint8_t i = 0; i < strSNsize; i++)	{ outArr[i] = strSN[i]; }

	SendByteArray(strSNsize, outArr);
}

void split_int(uint32_t num, uint8_t * c, uint8_t * size)
{
	uint32_t n = num;
	uint32_t numQty = 0;
	
	// посчитать кол-во цифр в числе
	while (n != 0) { n /= 10; numQty++; }

	*size = numQty;

	for (uint8_t i = 0; i < numQty; i++)
	{
		c[i] = (uint8_t)((num % 10));
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

/*********************************************************************************************************************/
/*************************************Замена ip адреса и порта********************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
void SetIpAddress(uint8_t * ipAddressEnc, uint8_t addrSize)
{
	if (addrSize != g_ipAddrSizeEnc)
	{
		send_package_ENCRYPT(0, badIpAddrSize, NO_DATA);
		return;
	}

	uint8_t ipAddressArrSeparated[8];
	LFSRdecryptArrSeparated(ipAddressEnc, ipAddressArrSeparated, 8);
	
	uint8_t ipAddress[4];
	uint8_t idx = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		if(i != 0)
			idx += 2;
		
		uint8_t tmp[2];
		tmp[0] = ipAddressArrSeparated[idx];
		tmp[1] = ipAddressArrSeparated[idx + 1];
		ipAddress[i] = JoinArray(tmp, 2);
	}
  
	eeprom_busy_wait();
	eeprom_write_block(ipAddress, (uint8_t*) g_IpAddressEEPROMaddr, g_ipAddrSize);
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

void GetIpAddress()
{
	uint8_t ipAddrFirstByte = eeprom_read_byte((uint8_t*)g_IpAddressEEPROMaddr);
	if(ipAddrFirstByte == 0xFF)
	{
		send_package_ENCRYPT(0, ipAddrIsNotSet, NO_DATA);
		return;
	}
	
	uint8_t ipAddress[g_ipAddrSize];
	eeprom_read_block(ipAddress, (uint8_t*) g_IpAddressEEPROMaddr, g_ipAddrSize);
	
	send_package_ENCRYPT(g_ipAddrSize, confirmation, ipAddress);
}

void SetPort(uint8_t * portArrEnc, uint8_t portSize_in)
{
	if (portSize_in != g_portSizeEnc)
	{
		send_package_ENCRYPT(0, badPortSize, NO_DATA);
		return;
	}
	
	uint8_t portArrSeparated[3];
	LFSRdecryptArrSeparated(portArrEnc, portArrSeparated, 3);
	
	eeprom_busy_wait();
	eeprom_write_block(portArrSeparated, (uint16_t*)g_PortEEPROMaddr, g_portSize);
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

void LFSRdecryptArrSeparated(uint8_t * encArr, uint8_t * separatedArr, uint8_t separatedArrSize)
{
	uint8_t x = 0;
	
	for(int i = 0; i < separatedArrSize; i++)
	{
		uint8_t tmp[g_LFSR_codeSize];

		if(i != 0) 
			x += g_LFSR_codeSize;

		for(uint8_t j = 0; j < g_LFSR_codeSize; j++)
			tmp[j] = encArr[j + x];

    uint8_t res = LFSR_decrypt(tmp);
    separatedArr[i] = res; 
  }
}

void GetPort()
{
	uint8_t portFirstByte = eeprom_read_byte((uint8_t*)g_PortEEPROMaddr);
	if(portFirstByte == 0xFF)
	{
		send_package_ENCRYPT(0, portIsNotSet, NO_DATA);
		return;
	}
	
	uint8_t portArr[g_portSize];
	eeprom_read_block(portArr, (uint16_t*)g_PortEEPROMaddr, g_portSize);
	
	send_package_ENCRYPT(g_portSize, confirmation, portArr);
}

/*********************************************************************************************************************/
/******************************** Блокировка и снятие блокировки *****************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
void SetBlock(uint8_t blockStatus_in)
{	
	eeprom_write_byte_safe((uint8_t*)g_blockReasonEEPROMaddr, blockStatus_in);
	g_blockReason = blockStatus_in;
}

void GetBlockReason()
{
	uint8_t l_blockReason = eeprom_read_byte((uint8_t*)g_blockReasonEEPROMaddr);
	if(l_blockReason == 0xFF) l_blockReason = 0;
	
	uint8_t blockReasonArr[1];
	blockReasonArr[0] = l_blockReason;
	
	uint8_t blockReasonArrEnc[g_LFSR_codeSize];
	LFSRencArr(blockReasonArr, 1, blockReasonArrEnc, g_LFSR_codeSize);

	send_package_ENCRYPT(g_LFSR_codeSize, confirmation, blockReasonArrEnc);
}

void Unlock()
{
	uint8_t unlockFlag = eeprom_read_byte((uint8_t*) g_unlockFlagEEPROMaddr);
	if(unlockFlag == 1)
	{
		send_package_ENCRYPT(0, alreadyUnlockInCurrDay, NO_DATA);
		return;
	}
	
	if (g_blockReason == timeHackPlus)
		eeprom_write_dword_safe((uint32_t*)PlusTimeMistakeEEPROMaddr, 0);
	
	if(g_blockReason == timeHackMinus)
		eeprom_write_dword_safe((uint32_t*)MinusTimeMistakeEEPROMaddr, 0);
	
	eeprom_write_byte_safe((uint8_t*)g_blockReasonEEPROMaddr, 0);
	g_blockReason = 0;
	
	// установить флаг выполнения разблокирования в текущих стуках
	eeprom_write_byte_safe((uint8_t*) g_unlockFlagEEPROMaddr, 1);
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

/*********************************************************************************************************************/
/******************************** Установка/Изменение логики (дни/обследования) **************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
void SetLogic(uint8_t * logicArr, uint8_t logicSize)
{
	if(logicSize != 1)
	{
		send_package_ENCRYPT(0, badLogicSize, NO_DATA);
		return;
	}

	uint8_t logic = logicArr[0];
		
	if (logic != g_daysLogic && logic != g_actionsLogic)
	{
		send_package_ENCRYPT(0, undefinedLogic, NO_DATA);
		return;
	}
	
	uint8_t checkLogic = eeprom_read_byte((uint8_t*)g_logicEEPROMaddr);
	if(checkLogic != 0xFF)
	{
		send_package_ENCRYPT(0, logicAlreadyExists, NO_DATA);
		return;
	}
	
	eeprom_write_byte_safe((uint8_t*)g_logicEEPROMaddr, logic);
	g_currentLogic = logic;
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}


void ChangeLogic(uint8_t * logicEnc_in, uint8_t logicSize)
{
	uint8_t changeLogicFlag = eeprom_read_byte((uint8_t*) g_changeLogicFlagEEPROMaddr);
	if(changeLogicFlag == 1)
	{
		send_package_ENCRYPT(0, logicAlreadyChangedInCurrDay, NO_DATA);
		return;
	}
	
	uint8_t logicArrEnc[g_LFSR_codeSize];
	for(uint8_t i = 0; i < g_LFSR_codeSize; i++) logicArrEnc[i] = logicEnc_in[i];
	uint8_t logic = LFSR_decrypt(logicArrEnc);
	
	
	if (logic != g_daysLogic && logic != g_actionsLogic)
	{
		send_package_ENCRYPT(0, undefinedLogic, NO_DATA);
		return;
	}
	
	if(logic == g_daysLogic)
	{
		if(logicSize != (13 * g_LFSR_codeSize))
		{
			send_package_ENCRYPT(0, badDaysLogicSize, NO_DATA);
			return;
		}
		
		uint8_t changeLogicDaysArrEnc[12 * g_LFSR_codeSize];
		for(uint8_t i = 0; i < (12 * g_LFSR_codeSize); i++) 
			changeLogicDaysArrEnc[i] = logicEnc_in[i + g_LFSR_codeSize];
		
		uint8_t changeLogicDaysArr[12];		// параметры логики дни без команды и без самой логики
		
		LFSRdecryptArrSeparated(changeLogicDaysArrEnc, changeLogicDaysArr, 12);
		
		// текущее время
		uint8_t USTarr[UTSarrSize];
		for(uint8_t i = 0; i < UTSarrSize; i++) USTarr[i] = changeLogicDaysArr[i];
		uint32_t uts = JoinArray(USTarr, UTSarrSize);
		
		// время окончания лицензии
		uint8_t licenseTimeArr[UTSarrSize];
		for(uint8_t i = 0; i < UTSarrSize; i++) licenseTimeArr[i] = changeLogicDaysArr[i + UTSarrSize];
		uint32_t licenseTime = JoinArray(licenseTimeArr, UTSarrSize);
		
		// количество анализов в сутки
		uint8_t maxActionQtyArr[2];
		for(uint8_t i = 0; i < 2; i++) maxActionQtyArr[i] = changeLogicDaysArr[i + UTSarrSize * 2];
		uint16_t maxActionQty = JoinArray(maxActionQtyArr, 2);
		
		
		// проверить что выдаем лицензию меньше чем граница
		uint32_t firstSyncTime = uts;
		uint16_t dayQty = (licenseTime - firstSyncTime + 10) / DayInSec;	// +10 сек. для разницы во времени действий запуск приложения и синхронизация
		if(dayQty > g_MaxActionOrDayQty)
		{
			send_package_ENCRYPT(0, maxQtyDaysExceedsLimit, NO_DATA);
			return;
		}
		
		if(maxActionQty > g_MaxActionOrDayQty)
		{
			send_package_ENCRYPT(0, maxQtyActionsExceedsLimit, NO_DATA);
			return;
		}

		g_unixTimeStamp = uts;															// текущее время
		eeprom_write_dword_safe((uint32_t*)g_firstUTS_EEPROMaddr, uts);					// время первой синхронизации
		eeprom_write_dword_safe((uint32_t*)unixTimeStampEEPROMaddr, uts);				// время синхронизации времени
		
		eeprom_write_dword_safe((uint32_t*)LicenseTime_EEPROMaddr, licenseTime);		// записать время окончания лицензии
		eeprom_write_word_safe((uint16_t*)ActionMaxQuontityEEPROMaddr, maxActionQty);	// записать максимальное количество действий в сутки
		
		eeprom_write_dword_safe((uint32_t*)PlusTimeMistakeEEPROMaddr, 0);				// Сбросить положительные накрутки
		eeprom_write_dword_safe((uint32_t*)MinusTimeMistakeEEPROMaddr, 0);				// Сбросить отрицательные накрутки
	}

	if(logic == g_actionsLogic)
	{
		// g_actionsLogic
		// Количество анализов
		
		if(logicSize != (3 * g_LFSR_codeSize))
		{
			send_package_ENCRYPT(0, badActionsLogicSize, NO_DATA);
			return;
		}
		
		uint8_t changeLogicActionsArrEnc[2 * g_LFSR_codeSize];
		for(uint8_t i = 0; i < (2 * g_LFSR_codeSize); i++) 
			changeLogicActionsArrEnc[i] = logicEnc_in[i + g_LFSR_codeSize];
		
		uint8_t actionQtyArr[2];		// параметры логики обследования без команды и без самой логики т.е. кол-во обследований
		LFSRdecryptArrSeparated(changeLogicActionsArrEnc, actionQtyArr, 2);
		
		// Количество анализов
		uint16_t actionQty = JoinArray(actionQtyArr, 2);
		
		if(actionQty > g_MaxActionOrDayQty)
		{
			send_package_ENCRYPT(0, maxQtyActionsExceedsLimit, NO_DATA);
			return;
		}
		
		eeprom_write_word_safe((uint16_t*)ActionMaxQuontityEEPROMaddr, actionQty);
	}

	eeprom_write_word_safe((uint16_t*)g_globalActionsCounterEEPROMaddr, 0);		// Сбросить глобальный счетчик действий
	eeprom_write_word_safe((uint16_t *)ActionCounterEEPROMaddr, 0);				// сбросить текущий счетчик действий

	eeprom_write_byte_safe((uint8_t*)g_logicEEPROMaddr, logic);
	g_currentLogic = logic;
	
	// установить флаг выполнения смены логики в текущих стуках
	eeprom_write_byte_safe((uint8_t*) g_changeLogicFlagEEPROMaddr, 1);
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

void GetLogic()
{
	uint8_t logic = eeprom_read_byte((uint8_t*)g_logicEEPROMaddr);
	if(logic == 0xFF)
	{
		send_package_ENCRYPT(0, logicIsNotSet, NO_DATA);
		return;
	}
	
	uint8_t logicArr[1];
	logicArr[0] = logic;
	send_package_ENCRYPT(1, confirmation, logicArr);
}

/*********************************************************************************************************************/
/****************************Продление ключа на [x] дней/обследований*************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
void Prolongation(uint8_t * prolongationQtyArrEnc, uint8_t prolongationQtyArrEncSize)
{
	uint8_t prolongationFlag = eeprom_read_byte((uint8_t*)g_prolongationFlagEEPROMaddr);
	if(prolongationFlag == 1)
	{
		send_package_ENCRYPT(0, alreadyProlongationInCurrDay, NO_DATA);
		return;
	}
	
	if(prolongationQtyArrEncSize != g_prolongationArrSize)
	{
		send_package_ENCRYPT(0, badProlongationArrSize, NO_DATA);
		return;
	}

	uint8_t prolongationArrSeparated[prolongationQtyArrEncSize];
	LFSRdecryptArrSeparated(prolongationQtyArrEnc, prolongationArrSeparated, 2);
	
	
	uint16_t prolongationQty = JoinArray(prolongationArrSeparated, 2);
	
	if (prolongationQty > g_MaxActionOrDayQty)
	{
		send_package_ENCRYPT(0, prolongationQtyExceedsLimit, NO_DATA);
		return;
	}
	
	if(g_currentLogic == g_daysLogic)
	{
		uint32_t licTime = eeprom_read_dword((uint32_t*)LicenseTime_EEPROMaddr);
		licTime = licTime + (prolongationQty * DayInSec);
		eeprom_write_dword_safe((uint32_t*)LicenseTime_EEPROMaddr, licTime);
	}
	
	if(g_currentLogic == g_actionsLogic)
	{
		uint16_t actionMaxQuontity = eeprom_read_word((uint16_t *)ActionMaxQuontityEEPROMaddr);
		if(actionMaxQuontity == 0xFFFF) actionMaxQuontity = 0;

		actionMaxQuontity += prolongationQty;
		eeprom_write_word_safe((uint16_t *)ActionMaxQuontityEEPROMaddr, actionMaxQuontity);
	}
	
	// установить флаг выполнения продления в текущих стуках
	eeprom_write_byte_safe((uint8_t*) g_prolongationFlagEEPROMaddr, 1);
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

/*********************************************************************************************************************/
/******************************************* LFSR ********************************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/
void GenerateLFSRcode(uint8_t * LFSRcode_out, uint8_t takt)
{
    uint8_t pkIndex = 0;

    for (uint8_t i = 0; i < 1; i++)
    {
        uint32_t num = LFSR(g_LFSRkey, i + takt);
		uint8_t numSize = 0;
		uint8_t numArr[10];
        split_int(num, numArr, (uint8_t*)&numSize);

        for (uint8_t k = 0; k < numSize; k++)
        {
            if (pkIndex > g_LFSR_codeSize - 1)
                break;

            LFSRcode_out[pkIndex] = numArr[k];
            pkIndex++;
        }
    }

    if (pkIndex < g_LFSR_codeSize)
        for (uint8_t i = pkIndex; i < g_LFSR_codeSize; i++)
            LFSRcode_out[i] = 0;
}

void SetLFSRinitArr(uint8_t * initArr, uint8_t initArrSize)
{
	if (initArrSize != g_LFSRinitArrSize)
	{
		send_package_ENCRYPT(0, badLFSRInitArrSize, NO_DATA);
		return;
	}
	
	uint8_t LFSRinitFirstByte = eeprom_read_byte((uint8_t*) g_LFSRinitArrEEPROMaddr);
	if(LFSRinitFirstByte != 0xFF)
	{
		send_package_ENCRYPT(0, LFSRsettingsAlredySet, NO_DATA);
		return;
	}
	
	eeprom_busy_wait();
	eeprom_write_block(initArr, (uint8_t*) g_LFSRinitArrEEPROMaddr, g_LFSRinitArrSize);
	
	LFSRinit();
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

void LFSRinit()
{
	uint8_t LFSRinitFirstByte = eeprom_read_byte((uint8_t*) g_LFSRinitArrEEPROMaddr);
	if(LFSRinitFirstByte == 0xFF)
		return;
	
	uint8_t LFSRinitArr[g_LFSRinitArrSize];
	eeprom_read_block(LFSRinitArr, (uint8_t*)g_LFSRinitArrEEPROMaddr, g_LFSRinitArrSize);
	
	uint8_t keyArr[3];
	for(uint8_t i = 0; i < 3; i++)
		keyArr[i] = LFSRinitArr[i];
	
	g_LFSRkey = JoinArray(keyArr, 3);
	
	for(uint8_t i = 3; i < g_LFSRinitArrSize; i++)
		g_LFSRshiftArr[i - 3] = LFSRinitArr[i];
}

uint8_t LFSR_decrypt(uint8_t * encArr)
{
  for (uint8_t i = 0; i < 128; i++)
  {
    uint8_t keyArr[g_LFSR_codeSize];
	GenerateLFSRcode(keyArr, i);

    bool isEqually = true;
    for (int k = 0; k < g_LFSR_codeSize; k++)
    {
		if( !(encArr[k] >= 0 && encArr[k] <= 9) )
		{
			send_package_ENCRYPT(0, badLFSRsymbol, NO_DATA);
			return 0;
		}
		
		if (keyArr[k] != encArr[k])
		{
			isEqually = false;
			break;
		}
    }

    if (isEqually) return i;
  }
  
  SetBlock(unableToDecipherTheLFSRcommand);
  
  return 0;
}

long int LFSR(int key, int takt) 		// функция возвращает значение LFSR при данном ключе и на заданном такте
{
	// Прибавим количество дней к ключу
	uint32_t zeroUTS = eeprom_read_dword((uint32_t*)g_zeroUTS_EEPROMaddr);
	uint32_t currentUTStoZero = g_unixTimeStamp - (g_unixTimeStamp % DayInSec);
	uint32_t addKey = (currentUTStoZero - zeroUTS) / DayInSec;
	
	long int ShiftRegister = key + addKey;
	for (int i = 0; i < takt; i++)
	{
		ShiftRegister = ((
		(( ShiftRegister >> g_LFSRshiftArr[0])
		^ (ShiftRegister >> g_LFSRshiftArr[1])
		^ (ShiftRegister >> g_LFSRshiftArr[2])
		^ (ShiftRegister >> g_LFSRshiftArr[3])
		^ (ShiftRegister >> g_LFSRshiftArr[4])
		^ (ShiftRegister >> g_LFSRshiftArr[5])
		^ (ShiftRegister >> g_LFSRshiftArr[6])
		^ (ShiftRegister >> g_LFSRshiftArr[7])
		^ (ShiftRegister))
		& 0x00000001)
		<< 30) | (ShiftRegister >> 1);
	}
	
	if(takt == 0)
		ShiftRegister = 0;
	
	return ShiftRegister;
}

/*********************************************************************************************************************/
/******************************************** Контроль напряжений ****************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

void SetCheckVoltageFlag()
{
	eeprom_write_byte_safe((uint8_t *)g_checkVoltageFlagEEPROMaddr, 1);
	uint8_t check = eeprom_read_byte((uint8_t *)g_checkVoltageFlagEEPROMaddr);
	
	uint8_t responce[1];
	if(check != 0xFF)
		responce[0] = 48;
	else
		responce[0] = 49;

	SendByteArray(1, responce);
}


void SetHallWay(uint8_t hSize, uint8_t * hallWay)
{
	if (hSize != g_HallWaySize)
	{
		send_package_ENCRYPT(0, badHallWaySize, NO_DATA);
		return;
	}
	
	if (g_checkVoltageFlag) // если разрешили проверку напряжений, то установка уже невозможна.
	{
		send_package_ENCRYPT(0, checkVoltageFlagIsExists, NO_DATA);
		return;
	}
	
	for(uint8_t i = 0; i < g_HallWaySize; i++)
	{
		eeprom_write_byte_safe((uint8_t*)g_HallWayEEPROMaddr + i, hallWay[i]);
		
		uint8_t checkByte = eeprom_read_byte((uint8_t*)g_HallWayEEPROMaddr + i);
		if(checkByte != hallWay[i])
		{
			send_package_ENCRYPT(0, badSetHallWay, NO_DATA);
			return;
		}
	}
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

void GetHallWay()
{
	uint8_t hallWay[g_HallWaySize];
	eeprom_read_block(hallWay, (uint8_t*)g_HallWayEEPROMaddr, g_HallWaySize);
	send_package_ENCRYPT(g_HallWaySize, confirmation, hallWay);
}

void HallWayInint()
{
	eeprom_read_block(g_chargeMin, (uint8_t*)g_HallWayEEPROMaddr, 8);
	eeprom_read_block(g_chargeMax, (uint8_t*)g_HallWayEEPROMaddr + 8, 8);
	eeprom_read_block(g_dischargeMin, (uint8_t*)g_HallWayEEPROMaddr + 16, 8);
	eeprom_read_block(g_dischargeMax, (uint8_t*)g_HallWayEEPROMaddr + 24, 8);
}

void ADCcheck()
{	
	if (!g_isInit || !g_checkVoltageFlag) return;	// если ключ не проинициализирован и мы не разрешали проверку, то выход.
	
	if (g_dly >= 100)
	{
		if (g_dly >= 110)
		g_iteration = 0;
    
		if (g_iteration == 8)
		{
			g_iteration = 0;
		}
    
		if (g_iteration == 0)
		{		
			g_ADCcheckType = rand() % 2;
			MyPinMode(&PORTF, &DDRF, g_checkPin, OUTPUT);

			// 1 - charge, 0 - discharge
			if (g_ADCcheckType == 1)   MyDigitalWrite(&PORTF, g_checkPin, LOW);   // зарядка от 0 до плавающего потенциала
			if (g_ADCcheckType == 0)   MyDigitalWrite(&PORTF, g_checkPin, HIGH);  // зарядка от 0 до плавающего потенциала
  
			_delay_ms(100);
			MyPinMode(&PORTF, &DDRF, g_checkPin, INPUT);
		}
	
		ADCcompare();
		g_iteration++;
		g_dly = 0;
	}

  _delay_ms(1);
  g_dly++;  
}

void ADCcompare()
{
    int currentVoltage = MyAnalogRead(g_checkPin) / 4;

    if (g_ADCcheckType == 1)
      if (currentVoltage < g_chargeMin[g_iteration] || currentVoltage > g_chargeMax[g_iteration])
        SetFormattingMarkAndReboot(chargeHack);

    if (g_ADCcheckType == 0)
      if (currentVoltage < g_dischargeMin[g_iteration] || currentVoltage > g_dischargeMax[g_iteration])
        SetFormattingMarkAndReboot(dischargeHack);
}

void MeasureVoltage(uint8_t ADCcheckType)
{
	if (g_checkVoltageFlag) // если проверка напряжения включена, то замеры не делаем
		return;
		
	uint8_t delayVal = 100;
	
	MyPinMode(&PORTF, &DDRF, g_checkPin, OUTPUT);

	if (ADCcheckType == 1)   MyDigitalWrite(&PORTF, g_checkPin, LOW);   // зарядка от 0 до плавающего потенциала
	if (ADCcheckType == 0)   MyDigitalWrite(&PORTF, g_checkPin, HIGH);  // зарядка от 0 до плавающего потенциала

	_delay_ms(delayVal);
	MyPinMode(&PORTF, &DDRF, g_checkPin, INPUT);


	uint8_t mass[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < 8; i++)
	{
		mass[i] = MyAnalogRead(g_checkPin) / 4;
		_delay_ms(delayVal);
	}
	
	SendByteArray(8, mass);
}

void adc_init()
{
	cli();
	ADMUX = ADC_VREF_TYPE;   /* set AVREF and right result */
	   /* ADCSRA  ADC Control and Status Register  
	   bit7 ADEN 1 => ADC Enable theconverter 
	   bit6 ADSC 1 => start each conversion in single mode
	   bit5 ADFR 1 => ADC Auto Trigger Enable  
	   bit4 ADIF bit set at conversion end cleared by ISR or writng 1 to ADIF 
	   bit3 ADIE I bit in SRE = 1 & ADIE =1 -> ADC Interrpt Enable
	   bit2 ADSP2 ADC Prescaler Select Bits
	   bit1 ADSP1
	   bit0 ADSP0 */
	ADCSRA = 0x86;
   /* SFIOR Special Function IO register
   bit7 ADTS2 ADC Auto Trigger Source Selector
   bit6 ADTS1 000 = free running mode
   bit5 ADTS0
   bit4 -----  reserved
   bit3 ACME 1 => Analog Comparator Mux Enable when ADC off 
   bit2 PUD 1 => Pull Up resistor Disabled
   bit1 PSR2 1 => Timer Counter 2 Reset
   bit0 PSR10 1 => Timer Counter s 0 & 1 Reset */
      /*SFIOR&=0xEF;  */  /* should be SFIOR = 0x03 */ 
   /* SFIOR is set to 0 above so SFIOR &= is always 0 */
   sei();
}

uint16_t MyAnalogRead(uint8_t adc_input)
{
	ADMUX = adc_input | ADC_VREF_TYPE; /* select channel */
	_delay_us(10); /* let signals settle */
	ADCSRA |= 0x40; /* Start the AD conversion  */
  
	while ((ADCSRA & 0x10) == 0) { ; } // Wait for the AD conversion to complete
	ADCSRA |= 0x10;  /*clear ADIF ADC Interupt Flag  */

	uint8_t low;
	uint8_t high;
	low  = ADCL;
	high = ADCH;
	return (high << 8) | low;
}

void MyPinMode(volatile uint8_t *PORTx, volatile uint8_t *DDRx, uint8_t pin, uint8_t mode)
{
	uint8_t bit = 1 << pin;

	uint8_t oldSREG = SREG;
	cli();

	if (mode == INPUT)
	{
		*DDRx &= ~bit;
		*PORTx &= ~bit;
	}

	if (mode == OUTPUT)
		*DDRx |= bit;

	SREG = oldSREG;
}

void MyDigitalWrite(volatile uint8_t *PORTx, uint8_t pin, uint8_t val)
{
	uint8_t bit = 1 << pin;

	uint8_t oldSREG = SREG;
	cli();
	if (val == LOW)
	{
		*PORTx &= ~bit;
	}
	else
	{
		*PORTx |= bit;
	}
	SREG = oldSREG;
}

/*********************************************************************************************************************/
/***************************************** Контроль температуры ******************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

void setupADC()
{
  cli();  //Отключаем глобальные прерывания

  ADMUX = 0;
  ADMUX |= (1 << REFS1);
  ADMUX |= (1 << REFS0);
  ADMUX |= (0 << MUX4);
  ADMUX |= (0 << MUX3);
  ADMUX |= (1 << MUX2);
  ADMUX |= (1 << MUX1);
  ADMUX |= (1 << MUX0);

  ADCSRA = 0;
  ADCSRA |= (1 << ADEN);
  ADCSRA |= (1 << ADPS2);

  ADCSRB = 0;
  ADCSRB |= (1 << MUX5);

  sei();  //Разрешаем глобальные прерывания
}

/*********************************************************************************************************************/
/*********************************************TIMER_FUNCTIONS*********************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

uint32_t JoinArray(uint8_t * mass, uint8_t size)
{
	uint32_t joinTS = 0;
	uint32_t multiplier = 1;
	int i;
	int x;
	
	for (x = 1; x < size; x++)
      multiplier *= 100;
	
	for (i = 0; i < size; i++)
	{
		if (i != 0) multiplier /= 100;
		joinTS += mass[i] * multiplier;
	}

	return joinTS;
}

void SplitArray(uint32_t timeStamp, uint8_t * mass, uint8_t size)
{
	uint32_t divisor = 1;
	uint32_t subtrahend;
	uint32_t tmpTime;
	uint32_t time = timeStamp;
	
	for (int i = 1; i < size; i++)
		divisor *= 100;

	for (uint8_t i = 0; i < size; i++)
	{
		if (i != 0)
		{
			subtrahend = mass[i - 1] * divisor;
			time -= subtrahend;
			divisor /= 100;
		}

		tmpTime = time / divisor;
		mass[i] = tmpTime;
	}
}

////////////////////// Время /////////////////////////////////////////////////////////////////////////
void TimeSynchronization(uint8_t * pDataArray, uint8_t DataArraySize)
{
	//Received current Time to register
	//Receive data
	timeCounter = 1;	// сброс счетчика т.к. синхронизация
	cli();

	uint32_t pc_time = JoinArray(pDataArray, UTSarrSize);
	
	
	if (FirstTimeSync)	// если самая первая синхронизация за всю работу ключа
	{
		g_unixTimeStamp = pc_time;
		FirstTimeSync = false;
		
		eeprom_write_dword_safe((uint32_t*)g_firstUTS_EEPROMaddr, pc_time);
		
		// записать время начала суток первой синхронизации (dd.mm.yyyy 00:00:00)
		uint32_t zeroUTS = pc_time - (pc_time % DayInSec);		
		eeprom_write_dword_safe((uint32_t*)g_zeroUTS_EEPROMaddr, zeroUTS);
	}

	//inside this function we save received time, if it's less then current
	if(g_currentLogic == g_daysLogic)
		check_up_down_time_mistake(pc_time);
	
	g_unixTimeStamp = pc_time; // Save received time in SRAM reg


	// Запись времени в EEPROM раз в 3 часа. Что-бы не исчерпать количество перезаписи
	uint32_t UTS_eeprom = eeprom_read_dword((uint32_t*)unixTimeStampEEPROMaddr);	

	if (UTS_eeprom == 0xFFFFFFFF)
		eeprom_write_dword_safe(((uint32_t*)unixTimeStampEEPROMaddr), g_unixTimeStamp);

	if ((g_unixTimeStamp - UTS_eeprom) >= 10800)
		eeprom_write_dword_safe((uint32_t*)unixTimeStampEEPROMaddr, g_unixTimeStamp);

	sei();
	
	TimeSynchronized = true;
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CheckFuseAndLock()
{
	uint8_t L_FUSE;
	uint8_t H_FUSE;
	uint8_t E_FUSE;
	uint8_t LOCK_BITS;
	
	L_FUSE = 	boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
	H_FUSE = 	boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
	E_FUSE = 	boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
	LOCK_BITS = boot_lock_fuse_bits_get(GET_LOCK_BITS);
	
	if(L_FUSE != RELEASE_L_FUSE) 		SetFormattingMarkAndReboot(bad_L_FUSE);
	if(H_FUSE != RELEASE_H_FUSE) 		SetFormattingMarkAndReboot(bad_H_FUSE);
	if(E_FUSE != RELEASE_E_FUSE) 		SetFormattingMarkAndReboot(bad_E_FUSE);
	if(LOCK_BITS != RELEASE_LOCK_BITS)	SetFormattingMarkAndReboot(bad_LOCK_BITS);
}

///////////////////////////////// Беседы и ограничение ////////////////////////////////////////////
bool Action()
{
	bool res = IncrementCurrentActionCounter();
	
	// глобальный подсчет действий
	if(res)
	{
		uint16_t counter = eeprom_read_word((uint16_t*)g_globalActionsCounterEEPROMaddr);
		if(counter == 0xFFFF)
			counter = 1;
		else
			counter++;
		
		eeprom_write_word_safe((uint16_t*)g_globalActionsCounterEEPROMaddr, counter);
	}
	
	return res;
}

void GetGlobalActionsOrDaysQty()
{
	if(g_currentLogic == g_daysLogic)
	{
		uint32_t firstSyncTime = eeprom_read_dword((uint32_t*)g_firstUTS_EEPROMaddr);
		
		uint16_t dayQty = 0;
		
		if(g_unixTimeStamp > firstSyncTime)
			dayQty = (g_unixTimeStamp - firstSyncTime) / DayInSec;
		else
			dayQty = (firstSyncTime - g_unixTimeStamp) / DayInSec;


		uint8_t dayQtyArr[2];
		SplitArray(dayQty, dayQtyArr, 2);

		uint8_t dayQtyArrEnc[2 * g_LFSR_codeSize];
		LFSRencArr(dayQtyArr, 2, dayQtyArrEnc, (2 * g_LFSR_codeSize));
		send_package_ENCRYPT((2 * g_LFSR_codeSize), confirmation, dayQtyArrEnc);
	}
	
	if(g_currentLogic == g_actionsLogic)
	{
		uint16_t counter = eeprom_read_word((uint16_t*)g_globalActionsCounterEEPROMaddr);
		if(counter == 0xFFFF) counter = 0;
		uint8_t counterArr[2];
		SplitArray(counter, counterArr, 2);

		uint8_t counterArrEnc[2 * g_LFSR_codeSize];
		LFSRencArr(counterArr, 2, counterArrEnc, (2 * g_LFSR_codeSize));

		send_package_ENCRYPT((2 * g_LFSR_codeSize), confirmation, counterArrEnc);
	}
}

void LFSRencArr(uint8_t * arr, uint8_t arrSize, uint8_t * arrEnc, uint8_t errEncSize)
{
	uint8_t x = 0;
	for(uint8_t i = 0; i < arrSize; i++) 
	{
		uint8_t currCode[g_LFSR_codeSize];
		
		GenerateLFSRcode(currCode, arr[i]);
		
		if(i != 0) 
			x += g_LFSR_codeSize;
		
		for (int k = 0; k < g_LFSR_codeSize; k++)
			arrEnc[k + x] = currCode[k];
  }
}

bool IncrementCurrentActionCounter()
{
	uint16_t ActionMaxQuontity = eeprom_read_word((uint16_t *)ActionMaxQuontityEEPROMaddr);
	// Если максимальное количество = 0, то безлимитно
	if (ActionMaxQuontity == 0)
		return true;
	
	// Проверить. Если находимся в новых сутках, то 
	// 1) обнулить счётчик
	// 2) определить и записать границу следующих суток в EEPROM
	// Иначе ничего не делать.
	
	uint32_t EndDayUTS = eeprom_read_dword((uint32_t*) ActionEndDayUTS_EEPROMaddr);	// читаем текущее время конца суток	
	
	// если время конца суток не установлено, то вычисляем и устанавливаем в EEPROM
	if (EndDayUTS == 0xFFFFFFFF)
		EndDayUTS = ComputeNewEndDayUTS_and_SetInEEPROM();
	
	if (EndDayUTS == 0)
		return false;
	
	if(g_currentLogic == g_daysLogic)
	{
		// сравниваем с текущим UTS
		if( g_unixTimeStamp > EndDayUTS)	// если уже новые сутки
		{
			// обнуляем счетчик в EEPROM
			eeprom_write_word_safe((uint16_t *)ActionCounterEEPROMaddr, 0);
			EndDayUTS = ComputeNewEndDayUTS_and_SetInEEPROM();
		}
	}

	uint16_t actionCounter = eeprom_read_word((uint16_t *)ActionCounterEEPROMaddr);
	uint16_t NewActionCounter = 0;
	
	// Если исчерпали количество бесед
	if (actionCounter == ActionMaxQuontity)
	{
		ActionQtyExhausted = true;

		if(g_currentLogic == g_daysLogic)
			send_package_ENCRYPT(0, endActionsInCurrDay, NO_DATA);
		
		
		if(g_currentLogic == g_actionsLogic)
		{
			SetBlock(endActions);
			send_package_ENCRYPT(0, dongleIsBlocked, NO_DATA);
		}

		return false;
	}
	else
	{
		ActionQtyExhausted = false;
	}

	// если за всю работу ключа только начали считать беседы
	if(actionCounter == 0xFFFF)
	{
		NewActionCounter = 1;
	}
	else
		NewActionCounter = actionCounter + 1;
	
	eeprom_write_word_safe((uint16_t*)ActionCounterEEPROMaddr, NewActionCounter);
	
	actionCounter = eeprom_read_word((uint16_t*)ActionCounterEEPROMaddr);

	if (actionCounter == NewActionCounter)
		return true;
	else
		return false;
}

uint32_t ComputeNewEndDayUTS_and_SetInEEPROM()
{
	uint16_t unixTimeStamp_Mod_DayInSec = 0;
	uint16_t delta = 0;
	uint32_t GMT_Corrector = 0;
	uint32_t newEndDayUTS = 0;
		
	GMT_Corrector = eeprom_read_dword((uint32_t*) GMT_CorrectorEEPROMaddr);
	if(GMT_Corrector == 0xFFFFFFFF)
	{
		send_package_ENCRYPT(0, noGMT_Corrector, NO_DATA);	// корректор не установлен
		return 0;
	}
	
	if (g_unixTimeStamp < DayInSec)
	{
		send_package_ENCRYPT(0, timeNotSync, NO_DATA);	// время не синхронизировано
		return 0;
	}

	unixTimeStamp_Mod_DayInSec = g_unixTimeStamp % DayInSec;		// получили кол-во прошедших секунд за текущие сутки
	delta = DayInSec - unixTimeStamp_Mod_DayInSec - GMT_Corrector;	// получить дельту, сколько нужно прибавить к текущему дню, что бы получить полночь
	newEndDayUTS = g_unixTimeStamp + delta;
	
	if (newEndDayUTS == 0)
	{
		send_package_ENCRYPT(0, badEndDay, NO_DATA);
		return false;
	}

	// записать в EEPROM только если новое вычисленное значение конца суток больше того что хранится в EEPROM
	uint32_t currentEndDayUTS = eeprom_read_dword((uint32_t*) ActionEndDayUTS_EEPROMaddr);	// читаем текущее время конца суток
	
	if(currentEndDayUTS == 0xFFFFFFFF || newEndDayUTS > currentEndDayUTS)
		eeprom_write_dword_safe((uint32_t*)ActionEndDayUTS_EEPROMaddr, newEndDayUTS);
	
	return newEndDayUTS;
}

void SetActionMaxQuontity(uint8_t * qty, uint8_t qtySize)
{
	uint16_t maxQty = eeprom_read_word((uint16_t*)ActionMaxQuontityEEPROMaddr);
	if (maxQty != 0xFFFF)
	{
		send_package_ENCRYPT(0, maxActionQtyIsExists, NO_DATA);
		return;
	}
	
	uint16_t newMaxQty = JoinArray(qty, qtySize);
	
	if (newMaxQty > g_MaxActionOrDayQty)
	{
		send_package_ENCRYPT(0, maxQtyActionsExceedsLimit, NO_DATA);
		return;
	}

	eeprom_write_word_safe((uint16_t*)ActionMaxQuontityEEPROMaddr, newMaxQty);
	
	maxQty = eeprom_read_word((uint16_t*)ActionMaxQuontityEEPROMaddr);
	if (maxQty != 0xFFFF)
		send_package_ENCRYPT(0, confirmation, NO_DATA);
	else
		send_package_ENCRYPT(0, maxQtyActionIsNotExists, NO_DATA);
}

void GetActionMaxQuontity()
{
	uint16_t maxQty = eeprom_read_word((uint16_t *)ActionMaxQuontityEEPROMaddr);
	
	if(maxQty != 0xFFFF)
	{
		uint8_t currentQtyArr[2];
		SplitArray(maxQty, currentQtyArr, 2);
		send_package_ENCRYPT(2, confirmation, currentQtyArr);
	}
	else
		send_package_ENCRYPT(0, maxQtyActionIsNotExists, NO_DATA);
}

void SetGMT_Corrector(uint8_t * corrector)
{	
	uint32_t GMT_Corrector = 0;
	GMT_Corrector = JoinArray(corrector, 3);
	if (GMT_Corrector > GMT_CorrectorMaximim)
	{
		send_package_ENCRYPT(0, timeCorrectorExceedsMaximum, NO_DATA);
		return;
	}
	
	GMT_Corrector = eeprom_read_dword((uint32_t*) GMT_CorrectorEEPROMaddr);
	// Установить корректор единожды
	if(GMT_Corrector != 0xFFFFFFFF)
	{
		send_package_ENCRYPT(0, timeCorrecorIsExists, NO_DATA);
		return;
	}
	
	GMT_Corrector = JoinArray(corrector, 3);
	eeprom_write_dword_safe((uint32_t * )GMT_CorrectorEEPROMaddr, GMT_Corrector);
			
	GMT_Corrector = eeprom_read_dword((uint32_t * ) GMT_CorrectorEEPROMaddr);
	if(GMT_Corrector != 0xFFFFFFFF)
		send_package_ENCRYPT(0, confirmation, NO_DATA);
	else
		send_package_ENCRYPT(0, timeCorrectorNotInstalled, NO_DATA);
}

void SendGMT_Corrector()
{
	uint32_t GMT_Corrector = eeprom_read_dword((uint32_t * )GMT_CorrectorEEPROMaddr);

	if(GMT_Corrector != 0xFFFFFFFF)
	{
		uint8_t GMT_CorrectorArray[3];
		SplitArray(GMT_Corrector, GMT_CorrectorArray, 3);
		send_package_ENCRYPT(3, confirmation, GMT_CorrectorArray);
	}
	else
		send_package_ENCRYPT(0, timeCorrectorIsNotExists, NO_DATA);
}

void SendRemainCurrentQuontityAction()
{
	uint16_t actionCounter = 0;
	uint16_t ActionMaxQuontity = eeprom_read_word((uint16_t *)ActionMaxQuontityEEPROMaddr);

	if(ActionMaxQuontity == 0)
	{
		uint8_t unlimitedActionArr[1];
		unlimitedActionArr[0] = 1;
		send_package_ENCRYPT(1, confirmation, unlimitedActionArr);
		return;
	}
	
	if (eeprom_read_word((uint16_t *)ActionCounterEEPROMaddr) != 0xFFFF)
		actionCounter = eeprom_read_word((uint16_t*)ActionCounterEEPROMaddr);
	
	// Вычтем из максимального числа бесед текущще
	uint16_t RemainQty = ActionMaxQuontity - actionCounter;
	uint8_t RemainQtyArr[2];
	SplitArray(RemainQty, RemainQtyArr, 2);
	send_package_ENCRYPT(2, confirmation, RemainQtyArr);
}

////////////////////////////////// Время лицензии /////////////////////////////////////////////////
void SetEndLicenseTimeToEEPROM(uint8_t * timeArr) // Установить время окончания лицензии в EEPROM
{
	if(g_currentLogic != g_daysLogic)
	{
		send_package_ENCRYPT(0, endLicTimeNeedNot, NO_DATA);
		return;
	}
	
	uint32_t licTime = eeprom_read_dword((uint32_t*) LicenseTime_EEPROMaddr);

	if (licTime != 0xFFFFFFFF)
	{
		send_package_ENCRYPT(0, endLicTimeIsExists, NO_DATA);  // устанавливать время окончания лицензии можно только 1 раз
		return;
	}
	
	if (!TimeSynchronized)
	{
		send_package_ENCRYPT(0, timeNotSync, NO_DATA);
		return;
	}
	
	// проверить что выдаем лицензию меньше чем на 1000 дней
	licTime = JoinArray(timeArr, UTSarrSize);
	uint32_t firstSyncTime = eeprom_read_dword((uint32_t*)g_firstUTS_EEPROMaddr);
	uint16_t dayQty = (licTime - firstSyncTime + 10) / DayInSec;	// +10 сек. для разницы во времени действий запуск приложения и синхронизация
	
	if(dayQty > g_MaxActionOrDayQty)
	{
		send_package_ENCRYPT(0, maxQtyDaysExceedsLimit, NO_DATA);
		return;
	}
	
	// Записать время окончания лицензии	
	eeprom_write_dword_safe	((uint32_t*) LicenseTime_EEPROMaddr, licTime);
	
	uint32_t checkLicTime = eeprom_read_dword((uint32_t*) LicenseTime_EEPROMaddr);
	if(checkLicTime != licTime)
	{
		send_package_ENCRYPT(0, endLicTimeIsNotExists, NO_DATA);
		return;
	}
	
	uint32_t licDuration = licTime - g_unixTimeStamp;
	
	// Записать максимальное количество положительной накрутки
	g_MaxPositive = (licDuration / 100) * g_MaxPositivePercent;
	eeprom_write_dword_safe((uint32_t*)MaxPositiveEEPROMaddr, g_MaxPositive);
	
	uint32_t checkMaxPositive = eeprom_read_dword((uint32_t*)MaxPositiveEEPROMaddr);
	if(checkMaxPositive != g_MaxPositive)
	{
		send_package_ENCRYPT(0, MaxPositiveIsNotExists, NO_DATA);
		return;
	}

	// Записать максимальное количество отрицательной накрутки
	g_MaxNegative = (licDuration / 100) * g_MaxNegativePercent;
	eeprom_write_dword_safe((uint32_t*)MaxNegativeEEPROMaddr, g_MaxNegative);
	
	uint32_t checkMaxNegative = eeprom_read_dword((uint32_t*)MaxNegativeEEPROMaddr);
	if(checkMaxNegative != g_MaxNegative)
	{
		send_package_ENCRYPT(0, MaxNegativeIsNotExists, NO_DATA);
		return;
	}
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
}

void SendEndLicenseTimeFromEEPROM() 		// Отправить на пк время окончания лицензии
{	
	uint8_t endLicenseTimeArray[UTSarrSize];
	uint32_t endLicTime = eeprom_read_dword((uint32_t*) LicenseTime_EEPROMaddr);
	
	
	if (endLicTime != 0xFFFFFFFF)
	{
		SplitArray(endLicTime, endLicenseTimeArray, UTSarrSize);
		send_package_ENCRYPT(UTSarrSize, confirmation, endLicenseTimeArray);
	}
	else
		send_package_ENCRYPT(0, endLicTimeIsNotExists, NO_DATA);
}

////////////////////////////// Пароль /////////////////////////////////////////////////////////////////
void SetPassword(uint8_t *pswd)
{	
	uint8_t pswdSetByte = eeprom_read_byte((uint8_t *)PasswordEEPROMaddr);
	if(pswdSetByte != 0xFF) // Пароль можно установить только 1 раз
	{
		send_package_ENCRYPT(0, passwordIsExists, NO_DATA);
		return;
	}
	
	for(int i = 0; i < PasswordSize; i++)
		eeprom_write_byte_safe((uint8_t *)i + PasswordEEPROMaddr, pswd[i]);
	
	if(CheckPassword(pswd))
	{
		send_package_ENCRYPT(0, confirmation, NO_DATA);
		GenerateAndUseDefaultKey(); // Сгенерировать ключ по умолчанию если пароль установлен
	}
	else
		send_package_ENCRYPT(0, passwordIsNotSet, NO_DATA);
}

bool CheckPassword(uint8_t *pswd)
{
	for(int i = 0; i < PasswordSize; i++)
	{
		uint8_t pswdByte = eeprom_read_byte ((uint8_t *)i + PasswordEEPROMaddr);
		if (pswdByte != pswd[i])
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
void SendCurrTimeFromRegister()
{	
	uint8_t UTSarr[5];
	SplitArray(g_unixTimeStamp, UTSarr, UTSarrSize);
	send_package_ENCRYPT(UTSarrSize, confirmation, UTSarr);
}

void SendPlusTimeMistake()
{
	uint8_t PlusTimeMistakeArr[UTSarrSize];
	
	uint32_t plusTimeMistake = eeprom_read_dword((uint32_t*)PlusTimeMistakeEEPROMaddr);
	if (plusTimeMistake == 0xFFFFFFFF) plusTimeMistake = 0;
	
	SplitArray(plusTimeMistake, PlusTimeMistakeArr, UTSarrSize);
	send_package_ENCRYPT(UTSarrSize, confirmation, PlusTimeMistakeArr);
}

void SendMinusTimeMistake()
{
	uint8_t MinusTimeMistakeArr[UTSarrSize];
	uint32_t minusTimeMistake = eeprom_read_dword((uint32_t*)MinusTimeMistakeEEPROMaddr);
	if (minusTimeMistake == 0xFFFFFFFF) minusTimeMistake = 0;
	
	SplitArray(minusTimeMistake, MinusTimeMistakeArr, UTSarrSize);
	send_package_ENCRYPT(UTSarrSize, confirmation, MinusTimeMistakeArr);
}

void SendSynchroTimeFromEEPROM()
{
	uint32_t UTS_eeprom = eeprom_read_dword((uint32_t*)unixTimeStampEEPROMaddr);
	uint8_t UTS_eeprom_arr[UTSarrSize];
	SplitArray(UTS_eeprom, UTS_eeprom_arr, UTSarrSize);
	send_package_ENCRYPT(UTSarrSize, confirmation, UTS_eeprom_arr);
}

void SendMaxPositive()
{
	uint32_t l_maxPositive = eeprom_read_dword((uint32_t*) MaxPositiveEEPROMaddr);
	if (l_maxPositive == 0xFFFFFFFF)
	{
		send_package_ENCRYPT(0, MaxPositiveIsNotExists, NO_DATA);
		return;
	}
	
	uint8_t l_maxPositiveArr[UTSarrSize];
	SplitArray(l_maxPositive, l_maxPositiveArr, UTSarrSize);
	send_package_ENCRYPT(UTSarrSize, confirmation, l_maxPositiveArr);
}

void SendMaxNegative()
{
	uint32_t l_maxNegative = eeprom_read_dword((uint32_t*) MaxNegativeEEPROMaddr);
	if(l_maxNegative == 0xFFFFFFFF)
	{
		send_package_ENCRYPT(0, MaxNegativeIsNotExists, NO_DATA);
		return;
	}
	
	uint8_t l_maxNegativeArr[UTSarrSize];
	SplitArray(l_maxNegative, l_maxNegativeArr, UTSarrSize);
	send_package_ENCRYPT(UTSarrSize, confirmation, l_maxNegativeArr);
}

//////////////////////////////////////////////////////////////////////////
///////////////////////// База данных ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void DB_ReadAll()
{
	uint8_t dataBase[g_dbSize];
	eeprom_read_block(dataBase, (uint8_t*)DataBaseEEPROMAddr, g_dbSize);
	send_package_ENCRYPT(g_dbSize, confirmation, dataBase);
}

void DB_SetOneVal(uint8_t * data, uint8_t DataSize)
{
	uint8_t idx = data[0];
	uint8_t val = data[1];

	if (idx < 0 || idx > g_dbSize - 1)
	{
		send_package_ENCRYPT(0, dbOutOfRange, NO_DATA);
		return;
	}

	eeprom_write_byte_safe((uint8_t*)DataBaseEEPROMAddr + idx, val);
	uint8_t checkVal = eeprom_read_byte((uint8_t*)DataBaseEEPROMAddr + idx);
	if (checkVal == val)
		send_package_ENCRYPT(0, confirmation, NO_DATA);
	else
		send_package_ENCRYPT(0, dbValIsNotExists, NO_DATA);
}

void DB_GetOneVal(uint8_t * data, uint8_t DataSize)
{
	uint8_t idx = data[0];
	
	if (idx > g_dbSize - 1)
	{
		send_package_ENCRYPT(0, dbOutOfRange, NO_DATA);
		return;
	}

	uint8_t val = eeprom_read_byte((uint8_t*)DataBaseEEPROMAddr + idx);
	if (val == 0xFF)
	{
		send_package_ENCRYPT(0, dbValIsNotExists, NO_DATA);
		return;
	}
	
	uint8_t valArr[1];
	valArr[0] = val;
	send_package_ENCRYPT(1, confirmation, valArr);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////// Вычисление по формуле ////////////////////////////
//////////////////////////////////////////////////////////////////////////
void compute_f_wzh(uint8_t* wzh_array, uint8_t wzh_arraySize)
{
	if (wzh_arraySize%9 != 0)
	{
		send_package_ENCRYPT(0, badWZHsize, NO_DATA);
		return;
	}
	
	uint8_t fCount	= wzh_arraySize / 9;
	double wzh_result = compute_wzh_Array(wzh_array, fCount);
	
	double x = 0;
	double y = 0;
	double n = 0;
	x = wzh_result;
	y = modf(x, &n); // y - дробная, n - целая

	uint32_t int_wzh = n * 100000;
	
	uint8_t wzh_resultArray[5];
	SplitArray(int_wzh, wzh_resultArray, 5);
	
	send_package_ENCRYPT(5, confirmation, wzh_resultArray);
}

double compute_wzh_Array(uint8_t * wzhArr, uint8_t fCount)
{
	double result = 0;
	for (uint8_t fc = 0; fc < fCount; fc++)
	{
		uint32_t w = 0;
		uint32_t z = 0;
		uint32_t h = 0;

		uint8_t massW[3];
		massW[0] = wzhArr[0 + fc * 9];
		massW[1] = wzhArr[1 + fc * 9];
		massW[2] = wzhArr[2 + fc * 9];
		w = JoinArray(massW, 3);

		uint8_t massZ[3];
		massZ[0] = wzhArr[3 + fc * 9];
		massZ[1] = wzhArr[4 + fc * 9];
		massZ[2] = wzhArr[5 + fc * 9];
		z = JoinArray(massZ, 3);

		uint8_t massH[3];
		massH[0] = wzhArr[6 + fc * 9];
		massH[1] = wzhArr[7 + fc * 9];
		massH[2] = wzhArr[8 + fc * 9];
		h = JoinArray(massH, 3);

		result += computeFurmula_whz(w, z, h);
	}
	
	return result;
}

double computeFurmula_whz(uint32_t w, uint32_t z, uint32_t h)
{
	double dbl_w = w;
	double dbl_z = z;
	double dbl_h = h;

	double res = 0;

	res = dbl_w * (dbl_z / dbl_h);

	return res;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void reboot()
{
	wdt_enable(WDTO_250MS);
    for (;;);
}

/*********************************************************************************************************************/
/**********************************************HELP_FUNCTIONS*********************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

void Initialization()
{
	/**************************************************************************************/
	/**********************************Start initialization********************************/
	/**************************************************************************************/
	
	// Reset some registers
		MCUSR = 0;							//reset all reset flags
		wdt_disable();						//disable watchdog timer
		clock_prescale_set(clock_div_1);	//disable clock division
	
	// Start led signal
		LED_SETUP();
		RX_LED_OFF();
		TX_LED_OFF();
		CPU_PRESCALE(0);

	// Timer initialization
		OCR1AH = 0;
		OCR1AL = 250;

	// Comparing with A Interruption
		TIMSK1 = (1 << OCIE1A);
		
	// Set 1/64 prescale
		TCCR1B = ((1 << CS11) | (1 << CS10));
	
	// Enable global interruption flag
		sei();
		
	
	g_unixTimeStamp = eeprom_read_dword((uint32_t*)unixTimeStampEEPROMaddr);
	if (g_unixTimeStamp == 0xFFFFFFFF)
	{
		FirstTimeSync = true;
		
		g_isInit = false;
	}
	else
		g_isInit = true;


	CheckFuseAndLock();
	
	g_MaxPositive = eeprom_read_dword((uint32_t*) MaxPositiveEEPROMaddr);
	g_MaxNegative = eeprom_read_dword((uint32_t*) MaxNegativeEEPROMaddr);
	
	UnknownCMDcounter = eeprom_read_byte((uint8_t*) UnknownCMDcounterEEPROMaddr);
	if (UnknownCMDcounter == 0xFF) UnknownCMDcounter = 0;
	
	// инициализировать БД нулями если там 0xFF
	for(uint8_t i = 0; i < g_dbSize; i++)
	{
		uint8_t checkDbByte = eeprom_read_byte((uint8_t*) DataBaseEEPROMAddr + i);
		if (checkDbByte == 0xFF)
			eeprom_write_byte_safe((uint8_t*) DataBaseEEPROMAddr + i, 0);
	}
	
	GenerateAndUseDefaultKey(); // Сгенерировать и использовать ключ по умолчанию если пароль установлен
	
	uint8_t checkFlag = eeprom_read_byte((uint8_t*)g_checkVoltageFlagEEPROMaddr);
	if (checkFlag != 0xFF)
		g_checkVoltageFlag = true;
	
	adc_init();					// инициализация для контроля ножек
	MyAnalogRead(g_checkPin);	// обязательно нужно считать текущее напряжение первый раз
	HallWayInint();
	
	g_blockReason = eeprom_read_byte((uint8_t*)g_blockReasonEEPROMaddr);	// считать причину блокировки
	if (g_blockReason == 0xFF) g_blockReason = 0;
	
	LFSRinit();
	
	g_currentLogic = eeprom_read_byte((uint8_t*)g_logicEEPROMaddr);	// Текущая логика
	if(g_currentLogic == 0xFF) g_currentLogic = 0;
}

void check_up_down_time_mistake(uint32_t PC_time)
{
	if (PC_time > g_unixTimeStamp) 
	{
		uint32_t sumPlusTimeMistake = eeprom_read_dword((uint32_t*)PlusTimeMistakeEEPROMaddr); // Считать сумму положительных накруток из EEPROM
		if(sumPlusTimeMistake == 0xFFFFFFFF) sumPlusTimeMistake = 0;
		
		uint32_t plusTimeMistake = PC_time - g_unixTimeStamp;	// посчтитали текущую положительную накрутку
		sumPlusTimeMistake += plusTimeMistake;
		
		// Запишем в EEPROM
		eeprom_write_dword_safe((uint32_t*)PlusTimeMistakeEEPROMaddr, sumPlusTimeMistake);
		
		if (sumPlusTimeMistake > g_MaxPositive)
			SetBlock(timeHackPlus);
	}
	else
	{
		uint32_t sumMinusTimeMistake = eeprom_read_dword((uint32_t*)MinusTimeMistakeEEPROMaddr);	//  считать сумму отрицательных накруток из EEPROM
		if (sumMinusTimeMistake == 0xFFFFFFFF) sumMinusTimeMistake = 0;
		
		uint32_t minusTimeMistake = g_unixTimeStamp - PC_time;	// посчитали текущую отрицательную накрутку
		sumMinusTimeMistake += minusTimeMistake;
		
		// запишем в EEPROM
		eeprom_write_dword_safe((uint32_t*)MinusTimeMistakeEEPROMaddr, sumMinusTimeMistake);
		
		if (sumMinusTimeMistake > g_MaxNegative)
			SetBlock(timeHackMinus);
	}
}

/*********************************************************************************************************************/
/******************************************* Format_Flash ************************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

// reason - причина форматирования
void SetFormattingMarkAndReboot(uint8_t reason)
{
	uint8_t formatting_mark = 0;
	while(formatting_mark == 0)	// НА потом. Добавить отдельно Форматирование после конца загрузчика если не удалось 1000 раз поставить флаг форматирования.
	{
		eeprom_write_byte_safe((uint8_t*) formattingMarkEEPROMaddr, reason);
		formatting_mark = eeprom_read_byte ((uint8_t*) formattingMarkEEPROMaddr);
	}
	
	send_package_ENCRYPT(0, confirmation, NO_DATA);
	
	reboot();
}

/****************************************************************************************************/
/************************First Writing of Serial number into MC**************************************/
/****************************************************************************************************/

void write_serial_num(uint8_t * serialNumArr, uint8_t DataArraySize)
{	
	if(DataArraySize != sizeSerNum)
	{
		send_package_ENCRYPT(0, serialNumBadSize, NO_DATA);
		return;
	}
	
	uint8_t sn_firstByte = eeprom_read_byte((uint8_t*) serialNumEEPROMaddr);
	if (sn_firstByte == 0xFF)
	{
		eeprom_busy_wait();
		eeprom_write_block(serialNumArr, (uint16_t*) serialNumEEPROMaddr, sizeSerNum);
	
		send_package_ENCRYPT(0, confirmation, NO_DATA);
	}
	else
		send_package_ENCRYPT(0, serialNumIsExists, NO_DATA);	// ошибка т.к. серийный номер можно устанавливать только 1 раз
}

/****************************************************************************************************/
/***********************************NEW SWITCH-CASE CMD**********************************************/
/****************************************************************************************************/

void send_serial_num()
{	
	uint8_t sn_firstByte = eeprom_read_byte((uint8_t*) serialNumEEPROMaddr);
	if (sn_firstByte != 0xFF)
	{
		uint8_t sn_arr_EEPROM[sizeSerNum];
		eeprom_read_block(sn_arr_EEPROM, (uint8_t*)serialNumEEPROMaddr, sizeSerNum);
		send_package_ENCRYPT(sizeSerNum, confirmation, sn_arr_EEPROM);
	}
	else
		send_package_ENCRYPT(0, serialNumIsNotExists, NO_DATA);
}

/*********************************************************************************************************************/
/***************************************************USART*************************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

void send_package_ENCRYPT(uint32_t dataSize, uint8_t response, uint8_t * data)
{
	// Ответ и размер не шифруем.
	// Зашифровать данные.
	
	uint8_t EncryptData[255];
	uint32_t EncryptDataSize = 0;
	for (uint16_t k = 0; k < 255; k++) EncryptData[k] = 0;	// ОБЯЗАТЕЛЬНО! Нужно обнулять этот массив т.к. из-за особонности avr Си он хранит предыдущий мусор.
	
	encryptArray(EncryptData, (uint32_t*)&EncryptDataSize, data, dataSize);
	
	// обнуление размера зашифрованной последовательности. Т.к. из-за особенности ф-ции зашифрования при нулевом массиве данных возвращается размер 1. Что является ошибкой.
	if (dataSize == 0) EncryptDataSize = 0;
	
	CreateAndSendPackage(response, EncryptDataSize, EncryptData);
}

void send_package_NO_ENCRYPT(uint32_t dataSize, uint8_t response, uint8_t * data)
{	
	CreateAndSendPackage(response, dataSize, data);
}

void CreateAndSendPackage(uint8_t response, uint32_t dataSize, uint8_t * data)
{
	uint8_t packageSize = dataSize + 4;	// 4 - это команда и размер
	uint8_t package[dataSize + 4];
	package[0] = response;
	
	uint8_t sizeArr = 3;
	uint8_t dsArr[sizeArr];
	SplitArray(dataSize, dsArr, sizeArr);
	
	// прикрепить размер к пакету
	for(uint8_t i = 0; i < sizeArr; i++) package[i + 1] = dsArr[i];
	
	// прикрепить данные. Если они есть
	if (dataSize != 0)
		for (uint8_t i = 0; i < dataSize; i++) package[i + 4] = data[i];
	
	SendByteArray(packageSize, package);
}

void SendOneByte(uint8_t bt)
{
	uint8_t bArr[1];
	bArr[0] = bt;
	SendByteArray(1, bArr);
}

void SendByteArray(uint8_t size, uint8_t * data)
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
	uint32_t timeOutCounter = 0;
	
	// Select the OUT endpoint so that the next data byte can be read
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	
	// If OUT endpoint empty, clear it and wait for the next packet from the host
	while (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearOUT();

		while (!(Endpoint_IsOUTReceived()))
		{
			if (timeOutCounter > 10000) return timeoutError;
			
			timeOutCounter++;
				
			if (USB_DeviceState == DEVICE_STATE_Unattached)
			  return 0;
		}
	}

	// Fetch the next byte from the OUT endpoint
	return Endpoint_Read_8();
}

// TODO: добавлять мусор перед первым шифрованным байтом

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


/*********************************************************************************************************************/
/*******************************************Time interruption*********************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	// Reset counter
	TCNT1H = 0;
	TCNT1L = 0;
	
	if (timeCounter == mc_Sec)
	{
		nextTick = true;
		timeCounter = 1;
		g_unixTimeStamp++;
		
		if(correctorValue != 0)
		{
			if (timeCorrectorCounter == correctorValue) 
			{
				timeCounter++;
				timeCorrectorCounter = 1;
			}
			else
				timeCorrectorCounter++;
		}
	}
	
	timeCounter++;
	
	if (g_cmdStart)
		g_ADCtimer++;
}

/*********************************************************************************************************************/
/*************************************Encryption & Decryption data****************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/

void GenerateKey()
{
	uint8_t newTimeKey[g_TimeKeySize];
	for(uint8_t i = 0; i < g_TimeKeySize; i++) newTimeKey[i] = 0;	// очистка

	for (int i = 0; i < g_TimeKeySize; i++)
	{
		uint8_t buf = rand() / (RAND_MAX / MAX_DICE + 1);
		if(buf > 50)
			buf = 1;
		else 
			buf = 0;

		newTimeKey[i] = buf;
	}
	
	send_package_ENCRYPT(g_TimeKeySize, confirmation, newTimeKey);
	
	for (uint8_t i = 0; i < g_TimeKeySize; i++)	// применить нвый ключ для для МК
		g_TimeKey[i] = newTimeKey[i];
}

bool GenerateAndUseDefaultKey()
{
	// Формирование ключа по умолчанию из первых 20 элементов пароля
	// Формируем только если пароль установлен
	
	uint8_t pswdSetByte = eeprom_read_byte((uint8_t *)PasswordEEPROMaddr);
	if(pswdSetByte != 0xFF)
	{
		uint8_t password[PasswordSize];
		uint8_t newKey[g_TimeKeySize];
		eeprom_read_block(password, (uint8_t*)PasswordEEPROMaddr, PasswordSize);
		
		for (uint8_t i = 0; i < g_TimeKeySize; i++)
			newKey[i] = password[i] & 1;
		
		for (uint8_t i = 0; i < g_TimeKeySize; i++)	// применить нвый ключ для для МК
			g_TimeKey[i] = newKey[i];
			
		return true;
	}
	else
		return false;
}

void encryptArray(uint8_t* EncryptData, uint32_t* EncryptDataSize, uint8_t* inputData, uint32_t dataSize)
{
	uint8_t parser = 7; // Bit parser
	
	uint32_t EncryptDataSize_l = 0;

	uint8_t tNumb = 0;
	uint8_t uNumb = 0;
	uint8_t tens = 0;
	uint8_t units = 0;

	// Shifted tens & units
	uint8_t stens = 0;
	uint8_t sunits = 0;

	uint8_t tensKey[16];
	uint8_t unitsKey[16];

	for (int i = 0; i < 16; i++) tensKey[i] = g_TimeKey[i]; 		// выделить ключ для десятков
	for (int i = 0; i < 16; i++) unitsKey[i] = g_TimeKey[i + 16]; 	// выделить ключ для единиц

	for (uint16_t k = 0; k < dataSize; k++)
	{
		// взять сотню
		tens = (uint8_t)(inputData[k] / 16);   // взять десяток
		units = (uint8_t)(inputData[k] % 16);  // взять единицу
		
		// Tens dance
		if (tensKey[tens] == 1)
		{  // 5 bits
			stens = (uint8_t)((tens << 1) + 1);//add 1 to tens
			tNumb = 5;
		}
		else
		{
			//4b bits
			stens = tens;
			tNumb = 4;
		}


		// Units dance
		if (unitsKey[units] == 1)
		{  // 5 bits
			sunits = (uint8_t)(units << 1); // сдвиг десятков на единицу влево. Добавляется 0 в младший разряд. 11 << 1 = 110
			uNumb = 5;
		}
		else
		{
			// 4b bits
			sunits = units;
			uNumb = 4;
		}

		// Десятки
		for (uint8_t i = (uint8_t)(tNumb - 1); i >= 0; i--)
		{
			// break: variable overflow
			if (i > tNumb) break;

			if (parser > 7)
			{
				EncryptDataSize_l++;
				parser = 7;
			}
			
			uint8_t shiftTempl = (uint8_t)(1 << i);
			uint8_t stens_and_shiftTempl = (uint8_t)(stens & shiftTempl);
			uint8_t Sh_stens_and_shiftTempl = (uint8_t)(stens_and_shiftTempl >> i);
			uint8_t predRes = (uint8_t)(Sh_stens_and_shiftTempl << parser);
			uint8_t res = (uint8_t)(EncryptData[EncryptDataSize_l] | predRes);
			EncryptData[EncryptDataSize_l] = res;

			parser--;
		}

		// Единицы
		for (uint8_t i = (uint8_t)(uNumb - 1); i >= 0; i--)
		{
			if (i > uNumb) break;

			//templ = 1;
			if (parser > 7)
			{
				EncryptDataSize_l++;
				parser = 7;
			}
			EncryptData[EncryptDataSize_l] = (uint8_t)(EncryptData[EncryptDataSize_l] | (((sunits & (1 << i)) >> i) << parser));
			parser--;
		}
	}

	*EncryptDataSize = EncryptDataSize_l + 1;
}

void decryptArray(uint8_t* EncryptData, uint32_t EncryptDataSize, uint8_t* DecryptData, uint32_t* DecryptDataSize)
{	
    uint8_t selector = 128;//1000 0000
    uint8_t DecrDigit = 0;
    uint8_t ShiftFlag = 0;//false
    uint8_t DecrTens = 0;
    uint8_t DecrUnit = 0;

    uint8_t ByteNumber = 0;
    uint8_t SizeCount = 0;
    uint32_t outByteNumb = 0;

	uint8_t tensKey[16];
	uint8_t unitsKey[16];

	for (int i = 0; i < 16; i++) tensKey[i] = g_TimeKey[i]; 		// выделить ключ для десятков
	for (int i = 0; i < 16; i++) unitsKey[i] = g_TimeKey[i + 16]; 	// выделить ключ для единиц
	
    //Encrypt data byte array parser
	while (ByteNumber < EncryptDataSize)
    { //Input Byte Bits Parser
	
      for (uint8_t i = 0; i < 8; i++)
      {
        //Check situation: end of byte with decryption parser
        if(ShiftFlag == 255 && i == 0)
        {
          i++;
          ShiftFlag = 0;
        } else {
          ShiftFlag = 0;
        }
        //bit conversions
        uint8_t ShiftedSelector = selector >> i;
        ///Select bit
        uint8_t* pSelectedByte = (  EncryptData +  ByteNumber  );
        uint8_t SelectedBit = *pSelectedByte & ShiftedSelector;
        //Selected bit now is the first on the right
        SelectedBit = SelectedBit >> (7 - i);
        //Move bit to the correct position
        SelectedBit = SelectedBit << (7 - SizeCount);
        //Write new bit into decrypted byte
        DecrDigit = (DecrDigit | SelectedBit);

        //Check table & counter" tenses
        if(SizeCount == 3)//Tens stuff
        {
          //Select tens
          DecrTens = (DecrDigit & 240);//1111 0000
          DecrTens = (DecrTens >> 4);//0000 1111
          //Check shifts: tenses
		  if(tensKey[DecrTens] == 1)
		  {
            //Shift bit position
            if(i < 8)
            {
              i++;
              //Check edge bit shift
              if (i == 8){
                ShiftFlag = 255;
              }
            }
          }
        }
        //Check table & counter: units
        if(SizeCount == 7)
        {
          //Select unit
          DecrUnit = (DecrDigit & 15);//0000 1111
          //Check shifts: units
		  if(unitsKey[DecrUnit] == 1)
          {
            if(i < 8)
            {
              i++;
              if(i == 8){
                ShiftFlag = 255;
              }
            }
          }//end shift

          //Get output Byte
          uint8_t* pSelectedOutByte = (uint8_t*)(  DecryptData + outByteNumb  );
          *pSelectedOutByte = ((DecrTens * 16) + DecrUnit);
          //Remove variables value
          DecrDigit = 0;
          DecrTens = 0;
          DecrUnit = 0;
          outByteNumb++;
        }//end size count

        //Check the output byte length
        if(SizeCount < 8){
          SizeCount++;
          //Check byte overflow
          if(SizeCount == 8){
            SizeCount = 0;
          }
        }
      }//end for
       
	  
	  ByteNumber++;
	  
    }//end while
	
    *DecryptDataSize = outByteNumb;
}

/*********************************************************************************************************************/
/***********************************************USB_FUNCTIONS*********************************************************/
/*********************************************************************************************************************/
/*********************************************************************************************************************/


// Event handler for the USB_ConfigurationChanged event. This configures the device's endpoints ready
// to relay data to and from the attached USB host.

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