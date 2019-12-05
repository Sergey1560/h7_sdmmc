#ifndef SDIO_INIT_H
#define SDIO_INIT_H
#include "stm32h7xx.h"
#include "common_defs.h"
#include <string.h>
#include "clocks.h"
#include "log.h"

#include "SEGGER_SYSVIEW.h"

//https://github.com/yigiter/Sample-STM32F4-codes/blob/master/SDIOLib/src/SDIO.c
//https://github.com/LonelyWolf/stm32/blob/master/stm32l4-sdio/src/sdcard.c


#define SDIO_4BIT_Mode		1
#define SDIO_HIGH_SPEED 	1

#define SDIO_GPIO_DATA_SPEED  S_VH
#define SDIO_GPIO_CMD_SPEED  S_VH
#define SDIO_GPIO_CLK_SPEED  S_VH


#define UM2SD         (0x00)  //Transfer Direction
#define SD2UM         (0x02)

#define SDIO_DATA_TIMEOUT     ((uint32_t)0x01000000)  
#define SDIO_CMD_TIMEOUT      ((uint32_t)0x01000000)

#define SDIO_DATA_R_TIMEOUT   ((uint32_t)((SystemCoreClock / (2) / 1000) * 250)) // Data read timeout is 250ms
#define SDIO_DATA_W_TIMEOUT   ((uint32_t)((SystemCoreClock / (2) / 1000) * 500)) // Date write timeout is 500ms

// SDIO CMD response type
#define SDIO_RESP_NONE         0x00                // No response
#define SDIO_RESP_SHORT        SDMMC_CMD_WAITRESP_0 // Short response
#define SDIO_RESP_LONG         SDMMC_CMD_WAITRESP   // Long response


// SD commands  index
#define SD_CMD0        ((uint8_t)0)
#define SD_CMD8        ((uint8_t)8)
#define SD_CMD55       ((uint8_t)55)
#define SD_ACMD41	   ((uint8_t)41)
#define SD_CMD2        ((uint8_t)2)
#define SD_CMD3        ((uint8_t)3)
#define SD_CMD6        ((uint8_t)6)
#define SD_CMD7        ((uint8_t)7)
#define SD_CMD9        ((uint8_t)9)
#define SD_CMD12        ((uint8_t)12)
#define SD_CMD13        ((uint8_t)13)
#define SD_CMD_SET_BLOCKLEN   ((uint8_t)16)
#define SD_CMD_SWITCH_FUNC    ((uint8_t)6U)
#define SD_CMD17        ((uint8_t)17)
#define SD_CMD18        ((uint8_t)18)
#define SD_CMD24        ((uint8_t)24)
#define SD_CMD25        ((uint8_t)25)
#define SDIO_ACMD41_CHECK            ((uint32_t)0x80000000)
#define SD_CHECK_PATTERN              ((uint32_t)0x000001AA) // Pattern for R6 response

#define SDIO_CLK_DIV_INIT      ((uint32_t)0x03C)  // SDIO clock 400kHz  (48MHz / (0x3c * 2) = 400kHz)

#define SDIO_ICR_STATIC     ((uint32_t)(SDMMC_ICR_CCRCFAILC | SDMMC_ICR_DCRCFAILC | SDMMC_ICR_CTIMEOUTC | \
                                        SDMMC_ICR_DTIMEOUTC | SDMMC_ICR_TXUNDERRC | SDMMC_ICR_RXOVERRC  | \
                                        SDMMC_ICR_CMDRENDC  | SDMMC_ICR_CMDSENTC  | SDMMC_ICR_DATAENDC  | \
                                         SDMMC_ICR_DBCKENDC ))

#define SDIO_DCTRL				(uint32_t)(9 << SDMMC_DCTRL_DBLOCKSIZE_Pos )
#define SDIO_STA_ERRORS			(uint32_t)(SDMMC_STA_RXOVERR | SDMMC_STA_TXUNDERR | SDMMC_STA_DTIMEOUT | SDMMC_STA_DCRCFAIL )
#define SDIO_STA_CMD_FLAGS		(uint32_t)(SDMMC_STA_CCRCFAIL|SDMMC_STA_CTIMEOUT|SDMMC_STA_CMDSENT|SDMMC_STA_CMDREND)

// SD card description
typedef struct {
	uint32_t    Capacity;        // Card capacity (MBytes for SDHC/SDXC, bytes otherwise)
	uint32_t    BlockCount;      // SD card blocks count
	uint32_t    BlockSize;       // SD card block size (bytes), determined in SD_ReadCSD()
	uint32_t    MaxBusClkFreq;   // Maximum card bus frequency (MHz)
	uint32_t    RCA;             // SD card RCA address (only for SDIO)
	uint32_t    PSN;             // SD card serial number
	uint32_t    CSD[4];          // SD card CSD register (card structure data)
	uint32_t    CID[4];          // SD card CID register (card identification number)
	uint16_t    OID;             // SD card OEM/Application ID
	uint16_t    MDT;             // SD card manufacturing date
	uint8_t     Type;            // Card type (detected by SD_Init())
	uint8_t     CSDVer;          // SD card CSD register version
	uint8_t     MID;             // SD card manufacturer ID
	uint8_t     PNM[5];          // SD card product name (5-character ASCII string)
	uint8_t     PRV;             // SD card product revision (two BCD digits: '6.2' will be 01100010b)
	uint8_t     SCR[8];          // SD card SCR register (SD card configuration)
} SDCard_TypeDef;


/*
Регистр статуса.
Возвращается в R1
*/
typedef struct {
	uint8_t			out_of_range;     //Аргумент команды вышел за пределы допустимого диапазона для этой карты.
	uint8_t			address_error;		//Ошибочно выровненный адрес, который не соответствует длине блока, который использовался в команде.
	uint8_t			block_len_error;	//Длина передаваемого блока не допустима для этой карты, или количество передаваемых байт не соответствует длине блока.
	uint8_t			erase_seq_error;	//Произошла ошибка в последовательности команд стирания.
	uint8_t			erase_param;			//Произошел недопустимый выбор записываемых блоков для стирания.
	uint8_t			wp_violation;			//Устанавливается, когда хост попытался выполнить запись в защищенный блок либо на временно защищенную от записи, либо на постоянно защищенную от записи карту.
	uint8_t			card_is_locked;		//Если установлен, то сигнализирует, что карта заблокирована хостом.
	uint8_t			lock_unlock_failed; //Устанавливается, когда была детектирована ошибка пароля в команде блокировки/разблокировки карты.
	uint8_t			com_crc_error;		//Ошибка CRC предыдущей команды.
	uint8_t			illegal_command;  //Команда недопустима для текущего состояния карты.
	uint8_t			card_ecc_failed;  //Была применена внутренняя ECC, но произошла ошибка для корректных данных.
	uint8_t			cc_error;					//Ошибка внутреннего контроллера карты.
	uint8_t			error;						//Во время выполнения операции произошла общая или неизвестная ошибка.
	uint8_t			csd_overwrite;    //Произошла одна из следующих ошибок: - Секция только для чтения CSD не соответствует содержимому карты.  - Попытка реверса копирования (копирование в место источника), или ошибка защиты от записи.
	uint8_t			wp_erase_skip;    //Устанавливается, когда была очищена только часть адресного пространства - из-за наличия защищенных от записи блоков, или очищалась карта, временно или постоянно защищенная от записи.
	uint8_t			card_ecc_disabled;//Была выполнена команда без внутреннего ECC.
	uint8_t			erase_reset;      //Была очищена последовательность стирания перед выполнением, потому что была принята команда выхода из последовательности стирания.
	uint8_t			current_state;    //Состояние карты, когда принимается команда. Если выполнение команды приводит к изменению состояния карты, это увидит хост в ответ на следующую команду. Эти 4 бита интерпретируются как двоичное число со значением в диапазоне от 0 до 15.
	/*
																	0: idle
																	1: ready
																	2: ident
																	3: stby
																	4: tran
																	5: data
																	6: rcv
																	7: prg
																	8: dis
																	9..14:зарезервировано
																	15: зарезервировано для режима I/O
	*/
	uint8_t			ready_for_data;  //Соответствует сигнализации по шине, что буфер пуст.
	uint8_t			app_cmd;         //Карта ожидает ACMD, или показывается, что команда была интерпретирована как ACMD.
	uint8_t			ake_seq_error;   //Ошибка в последовательности аутентификации.
} SD_Status_TypeDef;


void SDIO_gpio_init(void);
uint8_t SD_Init(void);
uint8_t SD_Cmd(uint8_t cmd, uint32_t arg, uint16_t response_type, uint32_t *response);
uint32_t SD_transfer(uint8_t *buf, uint32_t blk, uint32_t cnt, uint32_t dir);

void SD_check_status(SD_Status_TypeDef* SDStatus,uint32_t* reg);
void SD_parse_CSD(uint32_t* reg);
uint32_t SD_get_block_count(void);
uint8_t sd_get_cardsize(void);
uint8_t SD_HighSpeed(void);
uint8_t SD_CmdSwitch(uint32_t argument, uint8_t *resp);

#endif
