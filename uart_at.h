#ifndef UART_AT_H
#define UART_AT_H

#if defined(__cplusplus)
extern "C" {
#endif








typedef struct {//指令表中的 指令关键词和指令函数结构体
	const char* cmd_str;        //命令名称
    const char* action_str;     //指令动作
	unsigned int has_args;      // 1=需要参数，0=不需要(但是实际上都会传入？)
	void (*handler)(char*);     //
} CmdEntry;

typedef struct {//接受到的串口数据结构体
	char* p_cmd;
	char* p_action;
	char* p_value;
}cmd_struct;














#if defined(__cplusplus)
}
#endif

#endif /* _UART_AT_H */