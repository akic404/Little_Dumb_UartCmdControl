#ifndef __UART_AT_H__
#define __UART_AT_H__
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif





typedef struct{
    const char* val;
    LD_CmdTreeNode** childs;
    int number_of_childs;
	void (*callback)(char*);     //回调函数，TODO:后期改为可变参数，并增加参数数量验证
}LD_CmdTreeNode;


typedef struct {//指令表中的 指令关键词和指令函数结构体
	const char* main;        //命令名称
    const char* sub;     //指令动作
	unsigned int has_args;      // 1=需要参数，0=不需要(但是实际上都会传入？)
	void (*callback)(char*);     //对应的函数指针
} LD_CmdArrayNode;

typedef struct {//接受到的串口数据结构体
    //CMD:{main,sub,param}
	char* main;
	char* sub;
	char* param;
}LD_CmdUartReceive;






void LD_init();//库初始化函数，开机时需要运行一次
void LD_cmdtreenode_init(LD_CmdTreeNode* current);//初始化单个树节点
void LD_cmd_put_in_tree();//指令表压入树,dps逻
void LD_uart_idle_interrupt_process();//中断空闲处理，每次接收到完整一条命令时执行
void LD_systick_1ms();//1ms定时服务，用于定时清理缓存区，现在没启用


#if defined(__cplusplus)
}
#endif

#endif /* _UART_AT_H */