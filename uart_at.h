#ifndef UART_AT_H
#define UART_AT_H

#if defined(__cplusplus)
extern "C" {
#endif





typedef struct{
    const char* val;
    treenode_cmd** childs;
    int childs_size;
	void (*handler)(char*);     //回调函数，TODO:后期改为可变参数，并增加参数数量验证
}treenode_cmd;


typedef struct {//指令表中的 指令关键词和指令函数结构体
	const char* cmd_str;        //命令名称
    const char* action_str;     //指令动作
	unsigned int has_args;      // 1=需要参数，0=不需要(但是实际上都会传入？)
	void (*handler)(char*);     //
} CmdNode;

typedef struct {//接受到的串口数据结构体
	char* p_cmd;
	char* p_action;
	char* p_value;
}CmdStruct;














#if defined(__cplusplus)
}
#endif

#endif /* _UART_AT_H */