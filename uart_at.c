#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "uart_at.h"
#include <stdio.h>
// #include "main.h"
/*本程序使用LD_前缀作为内部命名空间前缀*/

//>>>>>>>>>>串口缓存 start
#define LD_uart_buffer_size  20 //定义本库内部缓存区大小
uint16_t LD_uart_buffer_index = 0;

char LD_uart_buffer[LD_uart_buffer_size];
char LD_uart_buffer_copy[LD_uart_buffer_size];
//<<<<<<<<<<串口缓存 end

LD_CmdTreeNode root;//创建命令表根节点

/*/////////////////////////////////////////
    命令表
*/
LD_CmdArrayNode LD_cmd_array[] = { //指令关键词和指令函数结构体列表
    /*{"命令1","命令2",1/0是否带参数,函数指针}*/
	{"AT","CMD",1,NULL},    // 例如：AT+LED=ON → 调用 handle_led("ON")
	{"AT","VALUE",1,NULL},
    {"AT","SWITCH",0,NULL},
    {"AT","LED",0,NULL},// 例如：AT+SPEED=50 → 调用 handle_speed("50")
    {"INDEX","RESET",0,NULL},// 例如：AT+SPEED=50 → 调用 handle_speed("50")
	// 可以继续添加...
};

/*/////////////////////////////////////////
    内部管理函数
*/
void LD_init(){//库初始化函数，开机时需要运行一次
    LD_cmdtreenode_init(&root);
    LD_cmd_put_in_tree();
    //TODO:初始化未设计完成
}

void LD_cmdtreenode_init(LD_CmdTreeNode* current){//初始化单个树节点
    current->val = NULL ;
    current->childs = calloc(10,sizeof(LD_CmdTreeNode*));//初始容量为10
    current->number_of_childs = 0 ;
}

void LD_cmd_put_in_tree(){//指令表压入树

    uint16_t cmd_array_node_size = sizeof(LD_cmd_array)/sizeof(LD_CmdArrayNode);//计算并存储有多少条命令

    uint8_t* used_main_index = calloc(cmd_array_node_size,sizeof(uint8_t));//建立此数组用于记录每条命令中的main部分是否已经被压入树
    if(used_main_index==NULL)LD_malloc_error_handler();//检测是否分配失败

    uint16_t number_of_childs = 10;//记录分配的空间大小,注意每次处理到一个新节点的childs时都要重置到10

    //>>>>> 添加main层节点start >>>>>
    for(int i = 0;i<cmd_array_node_size;i++){
       //判断当前cmd的main是否已被压入树中
        if(used_main_index[i]){
            //如果在'used_main_index'已经记录为已使用则跳过当前这条命令
            continue;
        }else{//如果未使用则进行压入

            //获取记录当前main词
            const char* current_main =LD_cmd_array[i].main;

            //判断现有数据量是否到达阈值决定是否扩容根节点的childs空间
            //判断方式：若加上一条命令，已占用空间是否超过总空间的0.8
            //若超过：空间*2重新分配
            if ((float)number_of_childs*0.8<root.number_of_childs+1){
                number_of_childs *=2;
                root.childs = realloc(root.childs,number_of_childs*sizeof(LD_CmdTreeNode*));
                if(root.childs==NULL)LD_malloc_error_handler();//检测是否分配失败
            }

            //添加节点到root.childs数组里
            root.childs[root.number_of_childs] = calloc(1,sizeof(LD_CmdTreeNode));//开辟一个新的 树节点 空间 并返回其指针到root.childs这个 树节点指针数组 对应的位置
            if(root.childs[root.number_of_childs]==NULL)LD_malloc_error_handler();//检测是否分配失败

            //格式化一下这个新的节点并把对应的main词赋值到val里
            LD_cmdtreenode_init(root.childs[root.number_of_childs]);
            root.childs[root.number_of_childs]->val = current_main;

            //更新“存储到root的子节点树”，此数字同时也作为索引使用，所有对新建子节点的操作需要在这行代码前执行
            root.number_of_childs++;

            //给相同词语标记上已使用,加快入树速度
            for (int i2 = 0; i2 < cmd_array_node_size; i2++){
                if(strcmp(current_main,LD_cmd_array[i2].main)==0){
                    used_main_index[i2] = 1;
                }
            }
        }
    }
    //TODO:根据root节点记录的number_of_childs 用realloc缩小root.childs空间

    //<<<<< 添加main层节点end <<<<<

    //>>>>> TODO:为每个main层节点添加sub层节点start >>>>>
    for(;;){

    }
    //<<<<< 为每个main层节点添加sub层节点end <<<<<

    //要不要释放原来的表？
    //TODO:压入树功能未完成
}
void LD_uart_idle_interrupt_process(){//中断空闲处理，每次接收到完整一条命令时执行
    memcpy(LD_uart_buffer_copy,LD_uart_buffer,LD_uart_buffer_size);
    LD_uart_cmd_process();
    //TODO:此处开始计时缓存清空倒计时，
}

void LD_systick_1ms(){//1ms定时服务，用于定时清理缓存区，现在没启用
    /*
    -1:未启用，不会执行，不会计数
    0: 执行，执行后同一周期内计数器重置到-1未启用状态
    >0: 不会执行，每次调用--并计数
    */
    uint8_t uart_buffer_clean_count = -1;
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


void LD_malloc_error_handler(){
    while(1){
        printf("LD命令库内存分配失败，程序终止");
    }
}