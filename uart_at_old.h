#ifndef _UART_AT_H
#define _UART_AT_H

#define uart_buffer_size 128//定义串口缓冲区大小


typedef struct {//接受到的串口数据结构体
	char* p_cmd;
	char* p_action;
	char* p_value;
}cmd_struct;

typedef struct {//指令表中的 指令关键词和指令函数结构体
	const char* cmd_str;
    const char* action_str; //指令动作
	unsigned int has_args; // 1=需要参数，0=不需要
	void (*handler)(char*);
} CmdEntry;

//===指令表函数
void cmd_fuc(char* value);
void value_fuc(char* value);
void switch_run(char* value);
void LED_toggle(char* value);
void uart_index_reset_fuction(char* value);
//===AT指令相关函数
//void uart_at_init(void);
void auto_save_in_buffer(char one_char);
void cmd_finish_receive(char *uart_buffer);
void exec_command(char* cmd, char* action, char* args);
void uart_reset_index(void);

void set_lmotorpid_p(char* value);
void set_lmotorpid_i(char* value);
void set_lmotorpid_d(char* value);
void set_rmotorpid_p(char* value);
void set_rmotorpid_i(char* value);
void set_rmotorpid_d(char* value);

void set_steeringpid_p(char* value);
void set_steeringpid_i(char* value);
void set_steeringpid_d(char* value);

void set_run_debug(char* value);
void allow_alldir_print(char* value);
#endif