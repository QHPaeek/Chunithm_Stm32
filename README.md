# Chunithm_Stm32
STM32(F411CEU6) projetct to run as chunithm controller  
这是一个未完成的DEMO  
使用STM32CubeIDE进行开发。  
目标：三颗mpr121驱动32key触摸，每个模块单独分配一路I2C硬件总线，开启IRO中断，使用DMA，并且每一路I2C都作为FreeRtos的一个任务进行并行读取，以做到尽可能降低延迟。  
当前进度：  
1.LL库硬件I2C的MPR121驱动(已完成)  
2.配置RTOS进程(未完成)  
3.改写ilufang为arduino撰写的chunithm串口驱动(未完成)  
