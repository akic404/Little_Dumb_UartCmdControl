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

void LD_cmd_put_in_tree(){//指令表压入树,dps逻辑

    uint16_t cmd_array_node_quantity = sizeof(LD_cmd_array)/sizeof(LD_CmdArrayNode);//计算并存储有多少条命令

    uint8_t* used_main_index = calloc(cmd_array_node_quantity,sizeof(uint8_t));//建立此数组用于记录每条命令中的main部分是否已经被压入树
    if(used_main_index==NULL)LD_malloc_error_handler();//检测是否分配失败

    uint8_t* used_sub_index = calloc(cmd_array_node_quantity,sizeof(uint8_t));//建立此数组用于记录每条命令中的sub部分是否已经被压入树
    if(used_sub_index==NULL)LD_malloc_error_handler();//检测是否分配失败

    uint16_t space_of_root_node_childs = 10;//记录分配的空间大小

    for(int i = 0;i<cmd_array_node_quantity;i++){
        //>>>>> 添加main层节点start >>>>>

        //判断当前cmd的main是否已被压入树中
        if(used_main_index[i]){
            //如果在'used_main_index'已经记录为已使用则跳过当前这条命令
            continue;
        }else{//如果未使用则进行压入

            //获取当前正在处理的 命令的 指针，专门用于main层
            LD_CmdArrayNode* current_arraynode_for_main = &LD_cmd_array[i];

            //判断现有数据量是否到达阈值决定是否扩容根节点的childs空间
            //判断方式：若加上一条命令，已占用空间是否超过总空间的0.8
            //若超过：空间*2重新分配
            if (root.number_of_childs>(float)space_of_root_node_childs*0.8){
                space_of_root_node_childs *=2;
                root.childs = realloc(root.childs,space_of_root_node_childs*sizeof(LD_CmdTreeNode*));
                if(root.childs==NULL)LD_malloc_error_handler();//检测是否分配失败
            }

            //添加节点到root.childs数组里
            LD_CmdTreeNode* current_treenode_level_main = calloc(1,sizeof(LD_CmdTreeNode));//开辟一个新的 树节点 空间
            if(current_treenode_level_main==NULL)LD_malloc_error_handler();//检测是否分配失败
                root.childs[root.number_of_childs]=current_treenode_level_main;//返回其指针到root.childs这个 树节点指针数组 对应的位置


            //格式化一下这个新的节点并转移数据
            LD_cmdtreenode_init(current_treenode_level_main);
            current_treenode_level_main->val= current_arraynode_for_main->main;

            //给相同词语标记上已使用,加快入树速度
            for (int i2 = 0; i2 < cmd_array_node_quantity; i2++){
                if(strcmp(current_arraynode_for_main->main,LD_cmd_array[i2].main)==0){
                    used_main_index[i2] = 1;
                }
            }

            //>>>>> 为当前main层节点添加sub层节点start >>>>>
            uint16_t space_of_main_node_childs = 10;//用于处理main节点用

            for (int j = 0; j < cmd_array_node_quantity; j++) {
                if(used_sub_index[j]==1){
                    //如果在'used_sub_index'已经记录为已使用则跳过当前这条命令
                    continue;
                }else{//如果未使用则进行压入
                    LD_CmdArrayNode* current_arraynode_for_sub =& LD_cmd_array[j];

                    //判断当前遍历到的准备用来压入sub的命令 其main部分是不是等于上一步压入的main词,避免sub压入了错误的main下面
                    if(strcmp(current_arraynode_for_main->main,current_arraynode_for_sub->main)!=0){
                        continue;
                    }else if(strcmp(current_arraynode_for_main->main,current_arraynode_for_sub->main)==0){

                        //扩展空间,这里是和当前main节点的childs空间大小进行对比
                        if (current_treenode_level_main->number_of_childs>(float)space_of_main_node_childs * 0.8 )
                        {
                            space_of_main_node_childs *=2;
                            current_treenode_level_main->childs = realloc(current_treenode_level_main->childs,space_of_main_node_childs*sizeof(LD_CmdTreeNode*));
                            if(current_treenode_level_main->childs==NULL)LD_malloc_error_handler();//检测是否分配失败
                        }

                        //新建一个treenode节点并存入当前main节点的childs对应位置
                        LD_CmdTreeNode* current_treenode_level_sub = calloc(1,sizeof(LD_CmdTreeNode));
                        if(current_treenode_level_sub==NULL)LD_malloc_error_handler();
                        current_treenode_level_main->childs[current_treenode_level_main->number_of_childs]=current_treenode_level_sub;

                        //初始化并转移数据
                        LD_cmdtreenode_init(current_treenode_level_sub);
                        current_treenode_level_sub->val = current_arraynode_for_sub->sub;

                        //更新“存储到当前main的子节点数量”，++前作为索引用，++后作为子节点数量数用
                        current_treenode_level_main->number_of_childs++;

                        //标记sub相同的命令为已使用
                        for(int j2 = 0;j2<cmd_array_node_quantity;j2++){
                            if(used_main_index[j2]==1){//这行用于避免标记了 未使用的 main 相同 sub的命令
                                if(strcmp(current_arraynode_for_sub->sub,LD_cmd_array[j2].sub)==0){
                                    used_sub_index[j2]=1;
                                }
                            }
                        }
                    }
                }
            }
            //收回main节点的childs多余分配出去的空间
            if(current_treenode_level_main->number_of_childs!=0){
            current_treenode_level_main->childs=realloc(current_treenode_level_main->childs,current_treenode_level_main->number_of_childs*sizeof(LD_CmdTreeNode*));
            }

            //<<<<< 为当前main层节点添加sub层节点end <<<<<

            //更新“存储到root的子节点数量”，++前作为索引用，++后作为子节点数量数用，所有对新建main节点的操作需要在这行代码前执行,也包括在此main节点压入sub节点
            root.number_of_childs++;


        }
        //<<<<< 添加main层节点end <<<<<


    }

    //收回root节点childs多余分配出去的空间
    if(root.number_of_childs!=0){
        root.childs=realloc(root.childs,root.number_of_childs*sizeof(LD_CmdTreeNode*));
    }

    //TODO:压入树功能未完成
}
//TODO：下面加个参数用于复制吧
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