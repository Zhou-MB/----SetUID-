#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/capability.h>
#include <sys/prctl.h>
using namespace std;

const int MAX_CAP_NAME_LEN = 19;

string choices = "\
 1：永久地删除子进程的某个权能。\n\
 2：暂时性地删除子进程的某个权能。\n\
 3：让上面被暂时性删除的权能重新获得。\n\
 4：查看当前进程的权能\n\
 5：启用子进程进行ping操作\n\
 q：退出本程序。\n\
 请输入选项对应的数字并回车：\n";

void printMenu()
{
    cout << choices;
}

// 打印进程uid gid信息
void whoami()
{
    printf("当前进程（uid=%i，euid=%i，gid=%i）\n", getuid(), geteuid(), getgid());
}

// 打印进程pid和权能信息
void listCaps()
{
    // 获取当前进程的权能，用以打印当前进程的权能状态
    cap_t caps = cap_get_proc();
    ssize_t y = 0;
    printf("当前进程pid=%d，权能%s\n", (int)getpid(), cap_to_text(caps, &y));
    fflush(0);
    // 及时释放空间
    cap_free(caps);
}

void doPing()
{
    if (!fork())
    {
        printf("执行shell命令的子进程（uid=%i，euid=%i，gid=%i）\n", getuid(), geteuid(), getgid());

        cap_t caps = cap_get_proc();
        ssize_t y = 0;
        printf("执行shell命令的子进程pid=%d，权能%s\n", (int)getpid(), cap_to_text(caps, &y));
        fflush(0);
        cap_free(caps);

        printf("进行ping baidu.com测试：\n");
        execlp("ping", "ping", "-c", "1", "baidu.com", NULL);
    }
}

void init()
{
    if (getuid() != 0)
    {
        printf("请使用sudo执行本程序\n");
        exit(1);
    }

    cap_value_t cap_values[] = {CAP_SETUID, CAP_SETGID, CAP_SETPCAP, CAP_NET_RAW}; // 设置权能列表
    cap_t caps = cap_init();                                                       // 初始化权能状态，返回结构体指针

    // 将caps_values数组中的权能的标记（EFFECTIVE/PERMITTED/INHERITABLE）置位
    cap_set_flag(caps, CAP_PERMITTED, 4, cap_values, CAP_SET);
    cap_set_flag(caps, CAP_EFFECTIVE, 4, cap_values, CAP_SET);

    cap_set_proc(caps);                 // 由caps设定当前进程的权能状态
    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0); // 切换用户时保留能力，让普通用户能继承root用户的特权
    cap_free(caps);                     // 释放权能状态

    printf("root用户下的进程和特权信息：\n");
    whoami();
    listCaps();

    // 切换成普通用户
    setgid(1000);
    setuid(1000);

    caps = cap_get_proc(); // 获取当前进程的权能状态
    cap_set_flag(caps, CAP_EFFECTIVE, 4, cap_values, CAP_SET);
    cap_set_flag(caps, CAP_INHERITABLE, 4, cap_values, CAP_SET);
    cap_set_proc(caps);
    cap_free(caps);

    printf("普通用户下的进程特权信息：\n");
    whoami();
    listCaps();
}

int main()
{
    string s;
    init();
    printMenu();
    cap_value_t cap2num = -1;
    while (getline(cin, s))
    {
        if (s.length() != 1)
        {
            printf("【warning】：请输入正确的数字或符号!\n");
        }
        else
        {
            char choice = s[0];
            // 永久或暂时删除子进程的某个权能
            if (choice == '1' || choice == '2' || choice == '3')
            {

                cap_t caps = cap_get_proc(); // 获取当前进程的权能状态

                // 能让上面被暂时性删除的权能重新获得
                if (choice == '3')
                {
                    if (cap2num == -1)
                    {
                        printf("【info】：没有可以恢复的权能！\n");
                    }
                    else
                    {
                        cap_set_flag(caps, CAP_EFFECTIVE, 1, &cap2num, CAP_SET);
                        cap_set_flag(caps, CAP_INHERITABLE, 1, &cap2num, CAP_SET);
                        if (cap_set_proc(caps))
                        {
                            printf("【error】：设置当前进程的权能状态失败！\n");
                        }
                        else
                        {
                            printf("【info】：成功恢复进程被暂时删除的权能\n");
                        }
                    }
                }
                else
                {
                    char cap_name[20];
                    string line;
                    printf("请输入想要操作的权能名称，如cap_net_raw（不区分大小写）：\n");
                    getline(cin, line);
                    if (line.length() > MAX_CAP_NAME_LEN)
                    {
                        printf("【warning】：请输入正确的权能名称！\n");
                    }
                    else
                    {
                        strcpy(cap_name, line.c_str());
                        if (cap_from_name(cap_name, &cap2num) < 0)
                        {
                            printf("【warning】：请输入正确的权能名称！\n");
                        }
                        else
                        {
                            // 2：暂时删除子进程的某个权能
                            cap_set_flag(caps, CAP_EFFECTIVE, 1, &cap2num, CAP_CLEAR);
                            cap_set_flag(caps, CAP_INHERITABLE, 1, &cap2num, CAP_CLEAR);
                            // 1：永久删除子进程的某个权能
                            if (choice == '1')
                            {
                                cap_set_flag(caps, CAP_PERMITTED, 1, &cap2num, CAP_CLEAR);
                            }

                            // 设置当前进程的权能状态
                            if (cap_set_proc(caps))
                            {
                                printf("【error】：设置当前进程的权能状态失败！\n");
                            }
                            else
                            {
                                printf("【info】：成功%s删除进程的%s权能\n", choice == '1' ? "永久" : "暂时", cap_name);
                            }
                        }
                    }
                }
                cap_free(caps); // 记得释放资源！！
            }
            else if (choice == '4')
            {
                whoami();
                listCaps();
            }
            else if (choice == '5')
            {
                doPing();
            }
            else if (choice == 'q')
            {
                break;
            }
            else
            {
                printf("【warning】：请输入正确的数字或符号!\n");
            }
        }
        int status = 0;
        wait(&status); // 等待子进程（ping baidu.com）结束
        if (!WIFEXITED(status))
        {
            printf("【error】：子进程ping baidu.com非正常结束\n");
        }
        printMenu(); // 子进程结束后再显示
    }
    return 0;
}