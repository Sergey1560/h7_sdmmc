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
uint32_t sd_speed=0;

BYTE work[FF_MAX_SS];


int main(void){

	INFO("System start");

	RCC_init();
	//MPU_Config();	

	result = f_mount((FATFS *)&FATFS_Obj, "0", 1);
	if(result != FR_OK){
		ERROR("f_mount fail: %d",result);
		while(1);
		};


	result = f_unlink("test_file.txt");

	result = f_open((FIL*)&file, "test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if(result != FR_OK) {
		ERROR("f_open fail: %d",result);
		while(1);
	}

	clk_count=dwt_get_tick();
	data_size=0;
	/* WRITE_COUNT раз пишет в файл кусками по 70 Кб */
	for(uint32_t i=0; i<WRITE_COUNT; i++){
		memset(data,0x30+i,DATA_SIZE);
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
	result=f_close((FIL*)&file);
	if(result != FR_OK) {
		ERROR("f_close fail: %d",result);
		return result;
	}
	
	sd_speed = data_size*1000/dwt_get_diff_sec(clk_count);
	INFO("Write to file done in %d bytes in %d ms, speed %ld byte/sec", data_size, dwt_get_diff_sec(clk_count),sd_speed);
	/* Чтение обратно, проверка записанного */

	result = f_open((FIL*)&file, "test_file.txt", FA_READ);
	if(result != FR_OK) {
		ERROR("f_open fail: %d",result);
		while(1);
	}

	data_size=0;
	clk_count=dwt_get_tick();
	for(uint32_t i=0; i<WRITE_COUNT; i++){
		memset(data,0,DATA_SIZE);
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
			if(data[k] != 0x30+i){
				ERROR("Pattern %d error, offset %d",i,k);
			}
		}
	};
	result=f_close((FIL*)&file);
	if(result != FR_OK) {
		ERROR("f_close fail: %d",result);
		return result;
	}
	
	sd_speed = data_size*1000/dwt_get_diff_sec(clk_count);
	INFO("Read from file done in %d ms, speed %ld byte/sec", dwt_get_diff_sec(clk_count),sd_speed);




	while(1){
	};
}

