
#include "uart_at.h"

//#include "stdio.h"
#include "string.h"

//=================================定义命令对应的函数========================================
// 指令表（可动态扩展）
CmdEntry cmd_table[] = { //指令关键词和指令函数结构体列表
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
//=================
/*模板
void 函数名(char* value) {//无论实际是需要参数，都会传入一个字符串
    if(value ......){//（如有需求）先检测参数合法性

      }
    else{//参数合法，执行

    }
}
*/
void cmd_fuc(char* value) {
	LQ_Printf("cmd is %s\n", value);
}
void value_fuc(char* value) {
	LQ_Printf("value is %s\n", value);
}
void switch_run(char* value) {

    LQ_Printf("switch is %s\n", value);
}
void LED_toggle(char* value){
    LED_Ctrl(LED0, RVS);
    LQ_Printf("LED_BLINK\n");
}
void uart_index_reset_fuction(char* value){
    uart_reset_index();
    //↑函数里有提示信息打印了
}

void set_lmotorpid_p(char* value){
    switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_lmotor_pid.pid_p = atof(value);
    }
    LQ_Printf("car_lmotor_pid->p = %f\n",car_lmotor_pid.pid_p);
}
void set_lmotorpid_i(char* value){
     switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_lmotor_pid.pid_i = atof(value);
    }
    LQ_Printf("car_lmotor_pid->i = %f\n",car_lmotor_pid.pid_i);
}
void set_lmotorpid_d(char* value){
    switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_lmotor_pid.pid_d = atof(value);
    }
    LQ_Printf("car_lmotor_pid->d = %f\n",car_lmotor_pid.pid_d);
}

void set_rmotorpid_p(char* value){
    switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_rmotor_pid.pid_p = atof(value);
    }
    LQ_Printf("car_rmotor_pid->p = %f\n",car_rmotor_pid.pid_p);
}
void set_rmotorpid_i(char* value){
     switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_rmotor_pid.pid_i = atof(value);
    }
    LQ_Printf("car_rmotor_pid->i = %f\n",car_rmotor_pid.pid_i);
}
void set_rmotorpid_d(char* value){
    switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_rmotor_pid.pid_d = atof(value);
    }
    LQ_Printf("car_rmotor_pid->d = %f\n",car_rmotor_pid.pid_d);
}

void set_steeringpid_p(char* value){
    switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_steering_pid.pid_p = atof(value);
    }
    LQ_Printf("car_steering_pid->p = %f\n",car_steering_pid.pid_p);
}
void set_steeringpid_i(char* value){
     switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_steering_pid.pid_i = atof(value);
    }
    LQ_Printf("car_steering_pid->i = %f\n",car_steering_pid.pid_i);
}
void set_steeringpid_d(char* value){
    switch(*value){
        case '?':
            LQ_Printf("\n");
        default:
        car_steering_pid.pid_d = atof(value);
    }
    LQ_Printf("car_steering_pid->d = %f\n",car_steering_pid.pid_d);
}

void set_run_debug(char* value){
    switch(*value){
        case '1':
            print_list.run_debug_flag = 1;break;
        case '0':
            print_list.run_debug_flag = 0;break;
        // case '?':
        //     LQ_Printf("")
    }
}

void allow_alldir_print(char* value){
    //print_list.all_dir = 1;
    print_list.dir_zip = 1;
}
//============================================================================================


cmd_struct cmd_data = { //创建 字符串 分割 结构体 实例
	.p_cmd = "NaN",
	.p_action = "NaN",
	.p_value = "NaN",
};

static char uart_buffer[uart_buffer_size];
static char uart_buffer_copy[uart_buffer_size];
static unsigned int uart_buffer_index = 0; //缓冲区索引



// void uart_at_init() { //串口AT指令初始化函数

// 	static char uart_buffer[uart_buffer_size];
// 	static char uart_buffer_copy[uart_buffer_size];

// }

void auto_save_in_buffer(char one_char) { //自动保存到缓冲区


    if ((uart_buffer_index >= uart_buffer_size - 1))//如果缓冲区已满
    {
        uart_buffer[uart_buffer_index] = '\0'; //出现结尾或者超出大小
        uart_buffer_index = 0; // 重置索引
        //不处理后续字符
    }
    else if (one_char == '\n') {
        uart_buffer[uart_buffer_index] = '\0'; //出现结尾或者超出大小
        uart_buffer_index = 0; // 重置索引
        cmd_finish_receive(uart_buffer); //不处理后续字符
    } //如果是换行符或回车符
    else if (one_char != '\r') {//如果不是回车符
        uart_buffer[uart_buffer_index++] = one_char; //保存字符到缓冲区
    }
}


void cmd_finish_receive(char *uart_buffer) {//命令接收完成函数
    strncpy(uart_buffer_copy, uart_buffer, uart_buffer_size); //复制到临时缓冲区
    uart_buffer_copy[uart_buffer_size - 1] = '\0'; //确保字符串结尾
    uart_buffer_index = 0; // 重置索引

		LQ_Printf("\nreceived: %s\n", uart_buffer_copy);

		cmd_data.p_cmd = strtok(uart_buffer_copy, "+"); //获取第一个字符串作为命令
		cmd_data.p_action = strtok(NULL, "="); //获取第二个字符串作为动作
		cmd_data.p_value = strtok(NULL, "\0"); //获取第三个字符串作为值

		//LQ_Printf("cmd %s\n", cmd_data.p_cmd);
		//LQ_Printf("action %s\n", cmd_data.p_action);
		//LQ_Printf("value %s\n", cmd_data.p_value);

		exec_command(cmd_data.p_cmd, cmd_data.p_action, cmd_data.p_value);//执行命令函数
}
void exec_command(char* cmd, char* action, char* args) {//参数处理函数
	for (int i = 0; i < sizeof(cmd_table) / sizeof(CmdEntry); i++) {/*列表长度/单独结构体长度*/
		if (strcmp(cmd, cmd_table[i].cmd_str) == 0/*对比命令1，命令2*/
            &&
            strcmp(action, cmd_table[i].action_str) == 0) {
			if (cmd_table[i].has_args && (args == NULL || strlen(args) == 0)) {
				printf("错误:缺少参数");
			}
            else {
				cmd_table[i].handler(args);
			}
			return;
		}
	}
	printf("错误:未知命令");
}

void uart_reset_index(){
    uart_buffer_index = 0;
    LQ_Printf("\n!!! OUT OF TIME , CLEAN THE UART BUFFER\n");
}




