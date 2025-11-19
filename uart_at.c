#include <string.h>

#include "uart_at.h"
#include "main.h"
/*本程序使用LD_前缀作为内部命名空间前缀*/

//>>>>>>>>>>串口缓存 start
#define LD_uart_buffer_size  20 //定义本库内部缓存区大小
uint16_t LD_uart_buffer_index = 0;

char LD_uart_buffer[LD_uart_buffer_size];
char LD_uart_buffer_copy[LD_uart_buffer_size];
//<<<<<<<<<<串口缓存 end

/*

*/
LD_CmdArrayNode cmd_array[] = { //指令关键词和指令函数结构体列表
    /*{"命令1","命令2",1/0是否带参数,函数指针}*/
	{"AT","CMD",1,cmd_fuc},    // 例如：AT+LED=ON → 调用 handle_led("ON")
	{"AT","VALUE",1,value_fuc},
    {"AT","SWITCH",0,switch_run},
    {"AT","LED",0,LED_toggle},// 例如：AT+SPEED=50 → 调用 handle_speed("50")
    {"INDEX","RESET",0,uart_index_reset_fuction},// 例如：AT+SPEED=50 → 调用 handle_speed("50")
/*################################################# motor pid*/
    {"lmotorpid","p",1,set_lmotorpid_p},
    {"lmotorpid","i",1,set_lmotorpid_i},
    {"lmotorpid","d",1,set_lmotorpid_d},
    {"rmotorpid","p",1,set_rmotorpid_p},
    {"rmotorpid","i",1,set_rmotorpid_i},
    {"rmotorpid","d",1,set_rmotorpid_d},
/*################################################# 转向 pid*/
    {"steeringpid","p",1,set_steeringpid_p},
    {"steeringpid","i",1,set_steeringpid_i},
    {"steeringpid","d",1,set_steeringpid_d},
/*################################################# 开始运行*/
    {"run","debug",1,set_run_debug},
/*################################################# 扫线*/
    {"AT","dir",0,allow_alldir_print}
	// 可以继续添加...
};

/*
    内部管理函数
*/
void LD_init(){
    LD_cmd_put_in_tree();
    //TODO:初始化未设计完成
}
void LD_cmd_put_in_tree(){
    uint16_t array_size = sizeof(cmd_array)/sizeof(LD_CmdArrayNode);
    //TODO:压入树功能未完成
}
void LD_uart_idle_interrupt_process(){
    memcpy(LD_uart_buffer_copy,LD_uart_buffer,LD_uart_buffer_size);
    LD_uart_cmd_process();
    //TODO:此处开始计时缓存清空倒计时，
}

void LD_systick_1ms(){
    /*
    -1:未启用，不会执行，不会计数
    0: 执行，执行后同一周期内计数器重置到-1未启用状态
    >0: 不会执行，每次调用--并计数
    */
    uint8_t uart_buffer_clean_count = -1
//FIXME:上面这个给他挪出去避免访问范围和寿命问题，可以放到结构体里，
    // 判断 执行
    if (uart_buffer_clean_count = 0)
    {
        //TODO:清理两个缓存
        memset(LD_uart_buffer,0,LD_uart_buffer_size);
        memset(LD_uart_buffer_copy,0,LD_uart_buffer_size);
        uart_buffer_clean_count = -1;
    }

    //判断 计数/计时
    if (uart_buffer_clean_count = 0)
    {
        uart_buffer_clean_count--;
    }
}

void LD_uart_cmd_process(){
    //TODO:先完成上面的树的部分再增加查询多叉树执行的部分
    //TODO:哦对顺带一提最好每次调用这个函数先复制一下cmd和action部分，因为我没做清理功能和这里的锁，我怕查树查到一半给我清空了，或者改为查完树再清空?
}