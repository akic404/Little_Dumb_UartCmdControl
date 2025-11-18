#include <string.h>

#include "uart_at.h"
#include "main.h"
/*本程序使用LD_前缀作为内部命名空间前缀*/

//>>>>>>>>>>串口缓存 start
#define LD_uart_buffer_size  20 //定义本库内部缓存区大小
uint16_t LD_uart_buffer_index = 0;

char LD_uart_buffer[LD_uart_buffer_size];
char LD_uart_buffer_copy[LD_uart_buffer_size];
//>>>>>>>>>>串口缓存 end


void LD_uart_idle_process(){
    memcpy(LD_uart_buffer_copy,LD_uart_buffer,LD_uart_buffer_size);

}

void LD_systick_1ms(){
    uint8_t uart_buffer_clean_count = -1

    // 判断 执行
    if(uart_buffer_clean_count = 0) {
        //TODO:清理两个缓存
    }

    //判断 计数/计时
    if(uart_buffer_clean_count>0){
        uart_buffer_clean_count--;
    }
}