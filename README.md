# STM32H7 SDMMC

Пример работы с модулем SDMMC и FATFs.  
Примеры того как работать с картой (инициализация, выбор режимов) я брал отсюда:
<https://github.com/yigiter/Sample-STM32F4-codes/blob/master/SDIOLib/src/SDIO.c>  
<https://github.com/LonelyWolf/stm32/blob/master/stm32l4-sdio/src/sdcard.c>  

Частота SD 50Mhz:
Кэш включен, оптимизация -O2:  
Write file: 18278400 bytes in 5484 ms, speed 3333 Kbyte/sec
Read file: 18278400 bytes in 1354 ms, speed 13499 Kbyte/sec
RAW Read: 18284544 bytes in 934 ms, speed 19576 Kbyte/sec

Кэш выключен, оптимизация -O2:  
Write file: 18278400 bytes in 5437 ms, speed 3361 Kbyte/sec
Read file: 18278400 bytes in 3751 ms, speed 4872 Kbyte/sec
RAW Read: 18284544 bytes in 924 ms, speed 19788 Kbyte/sec

Кэш включен, оптимизация -O0:
Write file: 18278400 bytes in 5559 ms, speed 3288 Kbyte/sec
Read file: 18278400 bytes in 1973 ms, speed 9264 Kbyte/sec
RAW Read: 18284544 bytes in 1026 ms, speed 17821 Kbyte/sec

Кэш отключен, оптимизация -O0:
Write file: 18278400 bytes in 5470 ms, speed 3341 Kbyte/sec
Read file: 18278400 bytes in 5124 ms, speed 3567 Kbyte/sec
RAW Read: 18284544 bytes in 923 ms, speed 19809 Kbyte/sec


Стандартная частота CLK карты 25Mhz в обычном режиме и
50 Mhz в режиме HighSpeed. Но работает и с большей частотой.
Частота SD Clk 100Mhz, кэш, -O2:
Write file: 18278400 bytes in 5239 ms, speed 3488 Kbyte/sec
Read file: 18278400 bytes in 982 ms, speed 18613 Kbyte/sec
RAW Read: 18284544 bytes in 561 ms, speed 32592 Kbyte/sec
