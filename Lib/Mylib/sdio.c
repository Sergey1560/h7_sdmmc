#include "sdio.h"

volatile SDCard_TypeDef SDCard;
volatile SD_Status_TypeDef SDStatus;

volatile uint8_t transmit;       //Флаг запущенной передачи данных в SDIO
volatile uint8_t multiblock=0;   //Используется в прерывании SDIO, чтоб слать команду STOP
volatile uint32_t error_flag=0;

volatile uint32_t trials;
volatile uint8_t state=0;    //Для хранения состояния карты
volatile uint32_t response[4]; //Для хранения ответа от карты
volatile uint32_t sta_reg=0;

//volatile uint8_t ALGN4 buf_copy[TMP_BUF_SIZE];

void SD_parse_CSD(uint32_t* reg){
	uint32_t tmp;
	//Версия CSD регистра
	if(reg[0] & (11U << 30)){
		SDCard.CSDVer=2;
	}else{
		SDCard.CSDVer=1;
	};
	//Размер карты и количество блоков	
	tmp= (reg[2] >> 16) & 0xFFFF;
	tmp |= (reg[1] & 0x3F) << 16;
	SDCard.BlockCount=tmp*1000;
	SDCard.Capacity=(tmp+1)*512;
};


uint32_t SD_get_block_count(void){
	return SDCard.BlockCount;
};


uint8_t SD_Cmd(uint8_t cmd, uint32_t arg, uint16_t response_type, uint32_t *response){

	SDIO->ICR = SDIO_ICR_STATIC;
	SDIO->ARG = arg;
	SDIO->CMD = (uint32_t)(response_type | cmd );
	SDIO->CMD |= SDMMC_CMD_CPSMEN;

	while((SDIO->STA & SDIO_STA_CMD_FLAGS) == 0){__NOP();};

	if (response_type != SDIO_RESP_NONE) {
		response[0] =	SDIO->RESP1;
		response[1] =	SDIO->RESP2;
		response[2] =	SDIO->RESP3;
		response[3] =	SDIO->RESP4;
	}
	
	if (SDIO->STA & SDMMC_STA_CTIMEOUT) return 2;
	if (SDIO->STA & SDMMC_STA_CCRCFAIL) return 3;  
	return 0;
}


uint32_t SD_transfer(uint8_t *buf, uint32_t blk, uint32_t cnt, uint32_t dir){
	uint32_t alignedAddr;
	uint8_t cmd=0;

	trials=SDIO_DATA_TIMEOUT;
	while (transmit && trials--) {};
	if(!trials) {
		return 1;
		}

	state=0;
	while(state != 4){ //Дождаться когда карта будет в режиме tran (4)
		SD_Cmd(SD_CMD13, SDCard.RCA ,SDIO_RESP_SHORT,(uint32_t*)response); 
		SD_check_status((SD_Status_TypeDef*)&SDStatus,(uint32_t*)&response[0]);
		state=SDStatus.current_state;

		if((state == 5) || (state == 6)) SD_Cmd(SD_CMD12, 0, SDIO_RESP_SHORT,(uint32_t*)response);
	};

	multiblock = (cnt == 1) ? 0 : 1;
	if (dir==UM2SD){ //Запись
				cmd=(cnt == 1)? SD_CMD24 : SD_CMD25;
			} 
	else if (dir==SD2UM){ //Чтение
				cmd=(cnt == 1)? SD_CMD17 : SD_CMD18;
			}
	else{
		ERROR("Unknown dir %d",dir);
		return 210;
	};

	//Настройка DMA
	if(((uint32_t)buf % 4) != 0){
		ERROR("Buffer not alligned ");
		return 200;
	}	
	transmit=1;
	error_flag=0;
	
	SDMMC1->IDMABASE0 = (uint32_t)buf;

	SDIO->DTIMER=(uint32_t)SDIO_DATA_R_TIMEOUT;
	SDIO->DLEN=cnt*512;    //Количество байт (блок 512 байт)
	SDIO->DCTRL= SDIO_DCTRL | (dir & SDMMC_DCTRL_DTDIR);  //Direction. 0=Controller to card, 1=Card to Controller
	SDIO->MASK=0;
	
	#ifdef ENABLE_DCACHE 
	/*
	Буфер в структуре FATFs не выровнен на 32 байта (для SCB_CleanDCache). При
	операции SCB_InvalidateDCache данные в строке кэша рядом с началом и концом
	буфера могут быть испорчены. Для этого сначала сброс этих строк в память, чтобы
	потом Invalidate был с правильными данными
	https://community.st.com/s/question/0D50X00009XkWmR/sd-card-memory-corruption-due-to-overly-aggressive-cache-maintenance
	*/
	alignedAddr = (uint32_t)buf & ~0x1F;
/*
	if (dir==UM2SD){ //Запись, сбросить из кэша весь буфер 
		SCB_CleanDCache_by_Addr((uint32_t *)alignedAddr,cnt*512+((uint32_t)buf - alignedAddr));
	}else{ //Чтение, сбросить из кэша первую и последнюю строку
		SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr, 32);
		SCB_CleanDCache_by_Addr((uint32_t*)(alignedAddr + cnt*512), 32);
	};
*/	
		SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr, 32);
		SCB_CleanDCache_by_Addr((uint32_t*)(alignedAddr + cnt*512), 32);
		if (dir==UM2SD){ //Запись, сбросить из кэша весь буфер 
			SCB_CleanDCache_by_Addr((uint32_t *)alignedAddr,cnt*512+((uint32_t)buf - alignedAddr));
		};

	#endif

	SD_Cmd(cmd, blk, SDIO_RESP_SHORT, (uint32_t*)response);
	
	SDIO->ICR=SDIO_ICR_STATIC;
	SDMMC1->IDMACTRL |= 1;
	SDIO->DCTRL|=1; //DPSM is enabled

	while((SDIO->STA & (SDMMC_STA_DATAEND|SDIO_STA_ERRORS)) == 0){__NOP();};

	if(SDIO->STA & SDIO_STA_ERRORS){
		error_flag=SDIO->STA;
		transmit=0;
		SDIO->ICR = SDIO_ICR_STATIC;
		return error_flag;
	}

	#ifdef ENABLE_DCACHE 
	if(dir==SD2UM) { //Read
		SCB_InvalidateDCache_by_Addr((uint32_t *)alignedAddr,cnt*512 + ((uint32_t)buf - alignedAddr));
	};
	#endif

	if(multiblock > 0) {
		SD_Cmd(SD_CMD12, 0, SDIO_RESP_SHORT, (uint32_t*)response);
	};
	transmit=0;		
	return 0;	
};



uint8_t sd_get_cardsize(void){
	if( ((SDCard.Capacity/1000) > 3000) && ((SDCard.Capacity/1000) < 5000) ) return 4;
	if( ((SDCard.Capacity/1000) > 7000) && ((SDCard.Capacity/1000) < 9000) ) return 8;
	if( ((SDCard.Capacity/1000) > 14000) && ((SDCard.Capacity/1000) < 17000) ) return 16;
	if( ((SDCard.Capacity/1000) > 31000) && ((SDCard.Capacity/1000) < 33000) ) return 32;
	return 0;
};

uint8_t SD_Init(void) {
	volatile uint32_t trials = 0x0000FFFF;
	uint32_t tempreg;  
	uint8_t result = 0;
	
	//Тактирование от PLL1 по умолчанию 
	SDIO_gpio_init();
	//SDIO & DMA
	RCC->AHB3ENR |= RCC_AHB3ENR_SDMMC1EN;

	RCC->AHB3RSTR |= RCC_AHB3RSTR_SDMMC1RST;
	for(uint8_t i=0; i<0x10; i++) {__NOP();};
	RCC->AHB3RSTR &= ~RCC_AHB3RSTR_SDMMC1RST;

	SDIO->CLKCR = SDIO_CLK_DIV_INIT; 
	SDIO->POWER |= SDMMC_POWER_PWRCTRL;
	
	//Ожидание 74 цикла
	for(uint32_t i=0; i<0x1000; i++) {__NOP();};
		
	result = SD_Cmd(SD_CMD0,0x00,SDIO_RESP_NONE,(uint32_t*)response);  //NORESP
	if (result != 0){
		ERROR("CMD0: %d",result);
		return 1;
	};
	
	result = SD_Cmd(SD_CMD8,SD_CHECK_PATTERN,SDIO_RESP_SHORT,(uint32_t*)response);  //R7
	if (result != 0) {
		ERROR("CMD8: %d",result);
		return 8;
	};
	if (response[0] != SD_CHECK_PATTERN) {
		ERROR("CMD8 pattern check");	
		return 8;
	};

	trials = 0x0000000F;
	while (--trials) {
			SD_Cmd(SD_CMD55, 0 ,SDIO_RESP_SHORT,(uint32_t*)response); // CMD55 with RCA 0   R1
			SD_check_status((SD_Status_TypeDef*)&SDStatus,(uint32_t*)&response[0]);
			SD_Cmd(SD_ACMD41,(1<<20|1<<30),SDIO_RESP_SHORT,(uint32_t*)response);
			if (response[0] & SDIO_ACMD41_CHECK) break;
		}
	if (!trials) {
		ERROR("CMD41 check");	
		return 41; 
	};

	result = SD_Cmd(SD_CMD2,0x00,SDIO_RESP_LONG,(uint32_t*)response); //CMD2 CID R2
	if (result != 0) {
		ERROR("CMD2: %d",result);
		return 2;
	};
		
	SDCard.CID[0]=response[0];
	SDCard.CID[1]=response[1];
	SDCard.CID[2]=response[2];
	SDCard.CID[3]=response[3];
	
	result = SD_Cmd(SD_CMD3,0x00,SDIO_RESP_SHORT,(uint32_t*)response); //CMD3 RCA R6
	if (result != 0){
		ERROR("CMD3: %d\n",result);
		return 3;		
	};
	SDCard.RCA=( response[0] & (0xFFFF0000) );

	result = SD_Cmd(SD_CMD9,SDCard.RCA,SDIO_RESP_LONG,(uint32_t*)response); //CMD9 СSD  R2
	if (result != 0) {
		ERROR("CMD9: %d\n",result);
		return 9;
	}
	
	SDCard.CSD[0]=response[0];
	SDCard.CSD[1]=response[1];
	SDCard.CSD[2]=response[2];
	SDCard.CSD[3]=response[3];
	
	SD_parse_CSD((uint32_t*)SDCard.CSD);	
		
	result = SD_Cmd(SD_CMD7,SDCard.RCA,SDIO_RESP_SHORT,(uint32_t*)response); //CMD7 tran   R1b
	SD_check_status((SD_Status_TypeDef*)&SDStatus,(uint32_t*)&response[0]);
	if (result != 0) {
		ERROR("CMD7: %d\n",result);
		return 7;
		};		

	state=0;
	//Дождаться когда карта будет в режиме tran (4)
	while(state != 4){
		SD_Cmd(SD_CMD13, SDCard.RCA ,SDIO_RESP_SHORT,(uint32_t*)response); 
		SD_check_status((SD_Status_TypeDef*)&SDStatus,(uint32_t*)&response[0]);
		state=SDStatus.current_state;
	};
	

  #if(SDIO_4BIT_Mode == 1)
		result = SD_Cmd(SD_CMD55, SDCard.RCA ,SDIO_RESP_SHORT,(uint32_t*)response); //CMD55 with RCA
		SD_check_status((SD_Status_TypeDef*)&SDStatus,(uint32_t*)&response[0]);
		if (result != 0){
			ERROR("CMD55: %d",result);
			return 55;
		}
	
		result = SD_Cmd(6, 0x02, SDIO_RESP_SHORT,(uint32_t*)response);      //Шлем ACMD6 c аргументом 0x02, установив 4-битный режим
		if (result != 0) {
			ERROR("CMD6: %d",result);
			return 6;
			};
		if (response[0] != 0x920) {
			ERROR("CMD6 response");
			return 5;
			};    //Убеждаемся, что карта находится в готовности работать с трансфером

		tempreg=((0x01)<<SDMMC_CLKCR_WIDBUS_Pos); 
		SDIO->CLKCR=tempreg;	

		#if (SDIO_HIGH_SPEED != 0)
			SD_HighSpeed();
		#endif
#else
		tempreg=0;  
		tempreg=SDIO_CLKCR_CLKEN; 
		SDIO->CLKCR=tempreg;	
#endif
	
	INFO("SD init done");
	return 0;
};


void SD_check_status(SD_Status_TypeDef* SDStatus,uint32_t* reg){
	SDStatus->ake_seq_error     = (*reg & (1 << 3)) ? 1 : 0;
	SDStatus->app_cmd           = (*reg & (1 << 5)) ? 1 : 0;
	SDStatus->ready_for_data    = (*reg & (1 << 8)) ? 1 : 0;
	SDStatus->current_state     = (uint8_t)((*reg & (0x0F << 9)) >> 9);
	SDStatus->erase_reset       = (*reg & (1 << 13)) ? 1 : 0;
	SDStatus->card_ecc_disabled = (*reg & (1 << 14)) ? 1 : 0;
	SDStatus->wp_erase_skip     = (*reg & (1 << 15)) ? 1 : 0;
	SDStatus->csd_overwrite     = (*reg & (1 << 16)) ? 1 : 0;
	SDStatus->error             = (*reg & (1 << 19)) ? 1 : 0;
	SDStatus->cc_error          = (*reg & (1 << 20)) ? 1 : 0;
	SDStatus->card_ecc_failed   = (*reg & (1 << 21)) ? 1 : 0;
	SDStatus->illegal_command   = (*reg & (1 << 22)) ? 1 : 0;
	SDStatus->com_crc_error     = (*reg & (1 << 23)) ? 1 : 0;
	SDStatus->lock_unlock_failed= (*reg & (1 << 24)) ? 1 : 0;
	SDStatus->card_is_locked    = (*reg & (1 << 25)) ? 1 : 0;
	SDStatus->wp_violation      = (*reg & (1 << 26)) ? 1 : 0;
	SDStatus->erase_param       = (*reg & (1 << 27)) ? 1 : 0;
	SDStatus->erase_seq_error   = (*reg & (1 << 28)) ? 1 : 0;
	SDStatus->block_len_error   = (*reg & (1 << 29)) ? 1 : 0;
	SDStatus->address_error     = (*reg & (1 << 30)) ? 1 : 0;
	SDStatus->out_of_range      = (*reg & (1U << 31)) ? 1 : 0;
};



void SDIO_gpio_init(void){
	/*
	SDIO:
	AF12
	D0 - PC8
	D1 - PC9
	D2 - PC10
	D3 - PC11
	CK - PC12
	CMD- PD2
*/
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN|RCC_AHB4ENR_GPIOCEN;
	/* MODER:
	00: Input (reset state)
	01: General purpose output mode
	10: Alternate function mode
	11: Analog mode
	*/
	//AF
	GPIOC->MODER &= ~(GPIO_MODER_MODER8|GPIO_MODER_MODER9|GPIO_MODER_MODER10|GPIO_MODER_MODER11|GPIO_MODER_MODER12);
	GPIOC->MODER |= GPIO_MODER_MODER8_1|GPIO_MODER_MODER9_1|GPIO_MODER_MODER10_1|GPIO_MODER_MODER11_1|GPIO_MODER_MODER12_1;;
	
	GPIOC->OSPEEDR |= (SDIO_GPIO_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR8_Pos) |\
					  (SDIO_GPIO_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR9_Pos) |\
					  (SDIO_GPIO_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR10_Pos)|\
					  (SDIO_GPIO_DATA_SPEED << GPIO_OSPEEDER_OSPEEDR11_Pos)|\
					  (SDIO_GPIO_CLK_SPEED << GPIO_OSPEEDER_OSPEEDR12_Pos);
	
	//Внешние подтяжки, внутренние не нужны
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR9|GPIO_PUPDR_PUPDR10|GPIO_PUPDR_PUPDR11|GPIO_PUPDR_PUPDR12);
	
	//AF12
	GPIOC->AFR[1] = (12 << GPIO_AFRH_AFRH8_Pos) | \
					(12 << GPIO_AFRH_AFRH9_Pos) | \
					(12 << GPIO_AFRH_AFRH10_Pos)| \
					(12 << GPIO_AFRH_AFRH11_Pos)| \
					(12 << GPIO_AFRH_AFRH12_Pos);

	//PD2 CMD 
	GPIOD->MODER &= ~GPIO_MODER_MODER2;
	GPIOD->MODER |= GPIO_MODER_MODER2_1;
	GPIOD->OSPEEDR |= (SDIO_GPIO_CMD_SPEED << GPIO_OSPEEDER_OSPEEDR2_Pos);
	GPIOD->AFR[0] |= (12 << GPIO_AFRL_AFRL2_Pos);
	
}


// Issue SWITCH_FUNC command (CMD6)
// input:
//   argument - 32-bit argument for the command, refer to SD specification for description
//   resp - pointer to the buffer for response (should be 64 bytes long)
// return: SDResult
// note: command response is a contents of the CCCR register (512bit or 64bytes)
uint8_t SD_CmdSwitch(uint32_t argument, uint8_t *resp) {
	uint32_t *ptr = (uint32_t *)resp;
	uint8_t res = 0;

	// SD specification says that response size is always 512bits,
	// thus there is no need to set block size before issuing CMD6
	// Clear the data flags
	SDIO->ICR = SDIO_ICR_STATIC;

	SDIO->DTIMER = SDIO_DATA_R_TIMEOUT;
	SDIO->DLEN = 64; 	// Data length in bytes
	// Data transfer:
	//   transfer mode: block
	//   direction: to card
	//   DMA: enabled
	//   block size: 2^6 = 64 bytes
	//   DPSM: enabled
	SDIO->DCTRL = SDMMC_DCTRL_DTDIR|(6 << SDMMC_DCTRL_DBLOCKSIZE_Pos) | SDMMC_DCTRL_DTEN;

	// Send SWITCH_FUNCTION command
	// Argument:
	//   [31]: MODE: 1 for switch, 0 for check
	//   [30:24]: reserved, all should be '0'
	//   [23:20]: GRP6 - reserved
	//   [19:16]: GRP5 - reserved
	//   [15:12]: GRP4 - power limit
	//   [11:08]: GRP3 - driver strength
	//   [07:04]: GRP2 - command system
	//   [03:00]: GRP1 - access mode (a.k.a. bus speed mode)
	//   Values for groups 6..2:
	//     0xF: no influence
	//     0x0: default
	res=SD_Cmd(SD_CMD_SWITCH_FUNC, argument, SDIO_RESP_SHORT, (uint32_t*)response); // CMD6
	if (res != 0) {	
		ERROR("CMD6 error %d",res);
		return res; 
		}

	// Read the CCCR register value
	while (!(SDIO->STA & (SDMMC_STA_RXOVERR | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT | SDMMC_STA_DBCKEND| SDMMC_STA_DATAEND))) {
		// The receive FIFO is half full, there are at least 8 words in it
		if (SDIO->STA & SDMMC_STA_RXFIFOHF) {
			*ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO;
		}
	}

	// Check for errors
	if (SDIO->STA & SDIO_XFER_ERROR_FLAGS) return	1;
	
	// Read the data remnant from the SDIO FIFO (should not be, but just in case)
	//while (!(SDIO->STA & SDMMC_STA_RXFIFOE)) *ptr++ = SDIO->FIFO;
	
	// Clear the static SDIO flags
	SDIO->ICR = SDIO_ICR_STATIC;

	return res;
}

uint8_t SD_HighSpeed(void) {
	uint8_t CCCR[64];
	uint8_t cmd_res = 0;

	// Check if the card supports HS mode
	cmd_res = SD_CmdSwitch(
				(0x0U << 31) | // MODE: check
				(0xFU << 20) | // GRP6: no influence
				(0xFU << 16) | // GRP5: no influence
				(0xFU << 12) | // GRP4: no influence
				(0xFU <<  8) | // GRP3: no influence
				(0xFU <<  4) | // GRP2: default
				(0x1U <<  0),  // GRP1: high speed
				CCCR
			);
	if (cmd_res != 0) {return cmd_res;}

	// Check SHS bit from CCCR
	if ( (CCCR[63 - (400 / 8)] & 0x01) != 0x01) {
		return 2;
	}

	// Ask the card to switch to HS mode
	cmd_res = SD_CmdSwitch(
				(0x1U << 31) | // MODE: switch
				(0xFU << 20) | // GRP6: no influence
				(0xFU << 16) | // GRP5: no influence
				(0xFU << 12) | // GRP4: no influence
				(0xFU <<  8) | // GRP3: no influence
				(0xFU <<  4) | // GRP2: default
				(0x1U <<  0),  // GRP1: high speed
				CCCR
			);
	if (cmd_res != 0) { return 3;}

	// Note: the SD specification says "card shall switch speed mode within 8 clocks
	// after the end bit of the corresponding response"
	// Apparently, this is the reason why some SD cards demand a delay between
	// the request for mode switching and the possibility to continue communication
	
	return 0;
}
