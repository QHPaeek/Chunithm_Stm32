基于stm32f411ceu6设计的chunithm控制器

本设计基于STM32cubeIDE与HAL库进行开发，采用FreeRTOS，旨在最大程度减少延迟，同时实现尽可能多的功能。

**关于使用：**

1.将\`F411_chunithm_15.6\Release\F411_chunithm_15.6.bin`使用STM32CubeProgrammer通过USB_DFU模式或者STlink刷入设备中。

2.重新插拔设备，将弹出的USB串行设备分配为COM1

3.编辑Segatool.ini使其停止Hook串口。

`[slider]
enable=0`

4.直接启动游戏。ENJOY！

**关于软件：**

依赖[AL94_USB_Composite]([https://github.com/alambe94/I-CUBE-USBD-Composite)库实现USB多设备复合工作。请在CUBEIDE中正确安装这个库。

**关于硬件：**[chunithm_affine](https://oshwhub.com/remige/chunithm_affine)

本设计的硬件尚未完善，目前只是能运行的DEMO版本。后续将持续不断更新PCB设计以及外壳。

**关于传感器：**

本分支为CY8CMBR3116分支（主分支），同时还有尚未开发完毕的MPR121分支，后续还会兼容其他触摸传感器。

**关于主控：**

目前是使用的STM32F411CEU6开发板，通过邮票孔贴片在底板上。目前也有计划支持其他STM32 MCU，例如STM32H750VBT6。

**已知的BUG：**

1.AL94_USB_Composite库在生成代码时并不会保护USER CODE区域中的用户代码，而本程序需要在USB接收回调函数中实现功能。因此请手动保护`\F411_chunithm_15.6\Middlewares\Third_Party\AL94_USB_Composite\COMPOSITE\App`目录下的`usbd_cdc_acm_if.c`以及`usbd_cdc_acm_if.h`两个文件。

2.目前的串口逻辑还存在问题，进入游戏会提示ERROR 3100，但是报错后按F1进入TEST后再退出就可以正常使用。

3.灯光的颜色是反的。

4.左侧11个灯的刷新频率可能较低。（受USB传输包长限制）
