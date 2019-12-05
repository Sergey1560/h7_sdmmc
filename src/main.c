#include "main.h"

FRESULT result;
FIL file;
DIR cdir;    
FILINFO fileInfo;
uint32_t nBytes;
FATFS FATFS_Obj;

uint8_t ALGN4 data[DATA_SIZE]; 
uint32_t clk_count;

uint32_t data_size=0;
float sd_speed;

int main(void){
	INFO("System start");
	RCC_init();
	SEGGER_SYSVIEW_Conf();

	SEGGER_SYSVIEW_PrintfHost("Mount");
	result = f_mount((FATFS *)&FATFS_Obj, "0", 1);
	if(result != FR_OK){
		ERROR("f_mount fail: %d",result);
		while(1);
		};

	SEGGER_SYSVIEW_PrintfHost("Open write");
	result = f_open((FIL*)&file, "test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if(result != FR_OK) {
		ERROR("f_open fail: %d",result);
		while(1);
	}

	clk_count=dwt_get_tick();
	data_size=0;

	INFO("Data start: %X end: %X",data,data+DATA_SIZE);

	/* WRITE_COUNT раз пишет в файл кусками по 70 Кб */

	SEGGER_SYSVIEW_PrintfHost("Write");
	for(uint32_t i=0; i<WRITE_COUNT; i++){
		memset(data,i,DATA_SIZE);
		result=f_write((FIL*)&file, (uint8_t *)data, DATA_SIZE, (UINT *)&nBytes);	
		if(result != FR_OK) {
			ERROR("f_write fail: %d",result);
			while(1);
		}
		if(nBytes != DATA_SIZE){
			ERROR("write count fail, data: %d writed %d",DATA_SIZE,nBytes);
			while(1);
		}
		data_size+=nBytes;
		result=f_sync((FIL*)&file);
		if(result != FR_OK) {
			ERROR("f_sync fail: %d",result);
			return result;
		}
	};
	SEGGER_SYSVIEW_PrintfHost("Close write");
	result=f_close((FIL*)&file);
	if(result != FR_OK) {
		ERROR("f_close fail: %d",result);
		return result;
	}
	
	sd_speed = (float)data_size/(float)dwt_get_diff_sec(clk_count);
	INFO("Write file: %d bytes in %d ms, speed %d Kbyte/sec", data_size, dwt_get_diff_sec(clk_count),(uint32_t)sd_speed);

	/* Чтение обратно, проверка записанного */
	SEGGER_SYSVIEW_PrintfHost("Open read");
	result = f_open((FIL*)&file, "test_file.txt", FA_READ);
	if(result != FR_OK) {
		ERROR("f_open fail: %d",result);
		while(1);
	}

	data_size=0;
	clk_count=dwt_get_tick();
	SEGGER_SYSVIEW_PrintfHost("Read");
	for(uint32_t i=0; i<WRITE_COUNT; i++){
		memset(data,0xEA,DATA_SIZE);
		result=f_read((FIL*)&file, (uint8_t *)data, DATA_SIZE, (UINT *)&nBytes);	
		if(result != FR_OK) {
			ERROR("f_read fail: %d",result);
			while(1);
		}
		if(nBytes != DATA_SIZE){
			ERROR("read count fail, data: %d read %d",DATA_SIZE,nBytes);
			while(1);
		}
		data_size+=nBytes;
		for(uint32_t k=0; k<DATA_SIZE; k++){
			if(data[k] != i){
				ERROR("Pattern %d error, offset %d,  adr: %X",i,k,&data[k]);
			}
		}
	};
	SEGGER_SYSVIEW_PrintfHost("Close read");
	result=f_close((FIL*)&file);
	if(result != FR_OK) {
		ERROR("f_close fail: %d",result);
		return result;
	}
	
	sd_speed = (float)data_size/(float)dwt_get_diff_sec(clk_count);
	INFO("Read file: %d bytes in %d ms, speed %d Kbyte/sec", data_size, dwt_get_diff_sec(clk_count),(uint32_t)sd_speed);


	data_size=0;
	clk_count=dwt_get_tick();
	SEGGER_SYSVIEW_PrintfHost("Raw start");
	for(uint32_t read_data=0; data_size < WRITE_COUNT*DATA_SIZE; read_data++){
		result = SD_transfer(data,read_data*64,64,SD2UM);
		if(result){
			ERROR("RAW Read Error");
		}
		data_size+=64*512;
	}
	SEGGER_SYSVIEW_PrintfHost("Raw end");
	sd_speed = (float)data_size/(float)dwt_get_diff_sec(clk_count);
	INFO("RAW Read: %d bytes in %d ms, speed %d Kbyte/sec", data_size, dwt_get_diff_sec(clk_count),(uint32_t)sd_speed);



	while(1){
	};
}

