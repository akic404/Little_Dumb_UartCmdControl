# Little_Dumb_UartCmdControl
愚蠢但简单的 利用表驱动的串口命令系统
- ## 使用说明
    - 开启串口接收，并将`LD_uart_buffer`作为串口接收缓存区，长度可在头文件中设置
    - 开启串口空闲中断，将 ` void LD_uart_idle_process() ` 添加到空闲中断当中
    - 设计定时器并开启中断，每1ms调用一次`LD_systick_1ms`