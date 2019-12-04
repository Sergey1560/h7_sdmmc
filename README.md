# STM32H7 SDMMC

Пример работы с модулем SDMMC и FATFs.  
Примеры того как работать с картой (инициализация, выбор режимов) я брал отсюда:
<https://github.com/yigiter/Sample-STM32F4-codes/blob/master/SDIOLib/src/SDIO.c>  
<https://github.com/LonelyWolf/stm32/blob/master/stm32l4-sdio/src/sdcard.c>  

С включенным кэшированием и оптимизацией -O2 результат:  
INFO main:76   [9144]:Write to file done in 18278400 bytes in 5336 ms, speed 3425 Kbyte/sec  
INFO main:112  [10599]:Read from file done, 18278400 bytes in 1454 ms, speed 12571 Kbyte/sec  
