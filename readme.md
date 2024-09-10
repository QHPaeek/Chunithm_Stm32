基于stm32f411ceu6设计的chunithm控制器

本设计基于STM32cubeIDE与HAL库进行开发，采用FreeRTOS，旨在最大程度减少延迟，同时实现尽可能多的功能。

**关于使用：**

1.将\`F411_chunithm_15.6\Release\F411_chunithm_15.6.bin`使用STM32CubeProgrammer通过USB_DFU模式或者STlink刷入设备中。

2.重新插拔设备，~~将弹出的USB串行设备分配为COM1~~。由于affineIO实现了使用VIDPID自动查找串口号的功能，因此可以直接跳转下一步。

3.编辑Segatool.ini，使用affine.dll IO文件连接游戏。

4.直接启动游戏。ENJOY！

**关于软件：**

依赖[AL94_USB_Composite]([https://github.com/alambe94/I-CUBE-USBD-Composite)库实现USB多设备复合工作。请在CUBEIDE中正确安装这个库。

**关于IO：**

本手台使用原创的IO连接游戏，详情请见[AffineIO](https://github.com/QHPaeek/Affine_IO)

**关于硬件：**[chunithm_affine](https://oshwhub.com/remige/chunithm_affine)

请加入企鹅群531883107获取群文件中PCB设计文档，或者自行根据工程中引脚分布设计外部电路。

**关于传感器：**

本分支为CY8CMBR3116分支（主分支），同时还有尚未开发完毕的MPR121分支，后续可能还会兼容其他触摸传感器。

**关于主控：**

目前是使用的STM32F411CEU6开发板。

**已知的BUG：**

1.AL94_USB_Composite库在生成代码时并不会保护USER CODE区域中的用户代码，而本程序需要在USB接收回调函数中实现功能。因此请手动保护`\F411_chunithm_15.6\Middlewares\Third_Party\AL94_USB_Composite\COMPOSITE\App`目录下的`usbd_cdc_acm_if.c`以及`usbd_cdc_acm_if.h`两个文件。
