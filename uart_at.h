#ifndef __UART_AT_H__
#define __UART_AT_H__

#if defined(__cplusplus)
extern "C" {
#endif





typedef struct{
    const char* val;
    LD_CmdTreeNode** childs;
    int childs_size;
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














#if defined(__cplusplus)
}
#endif

#endif /* _UART_AT_H */