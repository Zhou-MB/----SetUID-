## 国科大——《操作系统安全》

### Set-UID与Capability

#### 1.解释“passwd” , “sudo” ,  “ping”等命令为什么需要setuid位，去掉s位试运行，添加权能试运行。

**答**：“passwd”，”sudo”，”ping”都需要root权限来运行命令，普通用户没有权限运行，设置setuid位，任何用户执行时，都以setuid程序文件所属的用户的身份运行。

实验过程如下：

##### (1) passwd

去掉s位后无法执行：

![image-20230313203458992](picture\image-20230313203458992.png)

添加相应权能后恢复执行：

![image-20230313213848059](picture\image-20230313213848059.png)

##### (2) sudo

去掉s位后无法执行：

![image-20230313200858776](picture\image-20230313200858776.png)

恢复s位：

![image-20230313202953503](picture\image-20230313202953503.png)

##### (3) ping

去掉s位后无法执行：

![image-20230313200209275](picture\image-20230313200209275.png)

添加相应权能后恢复执行：

![image-20230313200444841](picture\image-20230313200444841.png)

#### 2.指出每个权能对应的系统调用，简要解释功能

通过find命令查找capability.h所在的位置，查看权能详情：

![image-20230313220653657](picture\image-20230313220653657.png)

总结如下：

|         权能         |      编号       |                             描述                             |
| :------------------: | :-------------: | :----------------------------------------------------------: |
|      CAP_CHOWN       |    0(chown)     |                     允许改变文件的所有权                     |
|   CAP_DAC_OVERRIDE   |        1        |                 忽略对文件的所有DAC访问限制                  |
| CAP_DAC_READ_SEARCH  |        2        |                 忽略所有对读、搜索操作的限制                 |
|      CAP_FOWNER      |        3        |          如果文件属于进程的UID，就取消对文件的限制           |
|      CAP_FSETID      |        4        |                       允许设置setuid位                       |
|       CAP_KILL       |     5(kill)     |                允许对不属于自己的进程发送信号                |
|      CAP_SETGID      |    6(setgid)    |                         允许改变组ID                         |
|      CAP_SETUID      |    7(setuid)    |                        允许改变用户ID                        |
|     CAP_SETPCAP      |    8(capset)    |       允许向其它进程转移能力以及删除其它进程的任意能力       |
| CAP_LINUX_IMMUTABLE  |    9(chattr)    |  允许修改文件的不可修改(IMMUTABLE)和只添加(APPEND-ONLY)属性  |
| CAP_NET_BIND_SERVICE |       10        |                   允许绑定到小于1024的端口                   |
|  CAP_NET_BROADCAST   |       11        |                    允许网络广播和多播访问                    |
|    CAP_NET_ADMIN     |       12        |          允许执行网络管理任务：接口、防火墙和路由等          |
|     CAP_NET_RAW      |   13(socket)    |                   允许使用原始(raw)套接字                    |
|     CAP_IPC_LOCK     |    14(mlock)    |                     允许锁定共享内存片段                     |
|    CAP_IPC_OWNER     |       15        |                      忽略IPC所有权检查                       |
|    CAP_SYS_MODULE    | 16(init_module) |                      插入和删除内核模块                      |
|    CAP_SYS_RAWIO     |       17        |                   允许对ioperm/iopl的访问                    |
|    CAP_SYS_CHROOT    |   18(chroot)    |                   允许使用chroot()系统调用                   |
|    CAP_SYS_PTRACE    |   19(ptrace)    |                       允许跟踪任何进程                       |
|    CAP_SYS_PACCT     |    20(acct)     |             允许配置进程记帐(process accounting)             |
|    CAP_SYS_ADMIN     |       21        | 允许执行系统管理任务：加载/卸载文件系统、设置磁盘配额、开/关交换设备和文件等 |
|     CAP_SYS_BOOT     |   22(reboot)    |                       允许重新启动系统                       |
|     CAP_SYS_NICE     |    23(nice)     |             允许提升优先级，设置其它进程的优先级             |
|   CAP_SYS_RESOURCE   |  24(setrlimit)  |                         忽略资源限制                         |
|     CAP_SYS_TIME     |    25(stime)    |                       允许改变系统时钟                       |
|  CAP_SYS_TTY_CONFIG  |   26(vhangup)   |                       允许配置TTY设备                        |
|      CAP_MKNOD       |    27(mknod)    |                   允许使用mknod()系统调用                    |
|      CAP_LEASE       |    28(fcntl)    |                      为任意文件建立租约                      |
|   CAP_AUDIT_WRITE    |       29        |                   允许向内核审计日志写记录                   |
|  CAP_AUDIT_CONTROL   |       30        |            启用或禁用内核审计，修改审计过滤器规则            |
|     CAP_SETFCAP      |       31        |                         设置文件权能                         |
|   CAP_MAC_OVERRIDE   |       32        |            允许MAC配置或状态改变，为smack LSM实现            |
|    CAP_MAC_ADMIN     |       33        |                       覆盖强制访问控制                       |
|      CAP_SYSLOG      |   34(syslog)    |                    执行特权syslog(2)操作                     |
|    CAP_WAKE_ALARM    |       35        |                     触发将唤醒系统的东西                     |
|  CAP_BLOCK_SUSPEND   |    36(epoll)    |                    可以阻塞系统挂起的特性                    |
|    CAP_AUDIT_READ    |       37        |              允许通过一个多播socket读取审计日志              |

#### 3.查找你Linux发行版系统(Ubuntu/centos等)中所有设置了setuid位的程序，指出其应该有的权能

使用`sudo find / -perm /u=s`命令查找所有设置了setuid位的程序，结果如下：

```
/usr/lib/openssh/ssh-keysign
/usr/lib/snapd/snap-confine
/usr/lib/xorg/Xorg.wrap
/usr/lib/eject/dmcrypt-get-device
/usr/lib/policykit-1/polkit-agent-helper-1
/usr/lib/dbus-1.0/dbus-daemon-launch-helper
/usr/sbin/pppd
/usr/bin/gpasswd
/usr/bin/arping
/usr/bin/pkexec
/usr/bin/chfn
/usr/bin/chsh
/usr/bin/passwd
/usr/bin/newgrp
/usr/bin/vmware-user-suid-wrapper
/usr/bin/sudo
/usr/bin/traceroute6.iputils
/snap/core20/1081/usr/bin/chfn
/snap/core20/1081/usr/bin/chsh
/snap/core20/1081/usr/bin/gpasswd
/snap/core20/1081/usr/bin/mount
/snap/core20/1081/usr/bin/newgrp
/snap/core20/1081/usr/bin/passwd
/snap/core20/1081/usr/bin/su
/snap/core20/1081/usr/bin/sudo
/snap/core20/1081/usr/bin/umount
/snap/core20/1081/usr/lib/dbus-1.0/dbus-daemon-launch-helper
/snap/core20/1081/usr/lib/openssh/ssh-keysign
/snap/core20/1828/usr/bin/chfn
/snap/core20/1828/usr/bin/chsh
/snap/core20/1828/usr/bin/gpasswd
/snap/core20/1828/usr/bin/mount
/snap/core20/1828/usr/bin/newgrp
/snap/core20/1828/usr/bin/passwd
/snap/core20/1828/usr/bin/su
/snap/core20/1828/usr/bin/sudo
/snap/core20/1828/usr/bin/umount
/snap/core20/1828/usr/lib/dbus-1.0/dbus-daemon-launch-helper
/snap/core20/1828/usr/lib/openssh/ssh-keysign
/snap/snapd/18357/usr/lib/snapd/snap-confine
/snap/core18/2708/bin/mount
/snap/core18/2708/bin/ping
/snap/core18/2708/bin/su
/snap/core18/2708/bin/umount
/snap/core18/2708/usr/bin/chfn
/snap/core18/2708/usr/bin/chsh
/snap/core18/2708/usr/bin/gpasswd
/snap/core18/2708/usr/bin/newgrp
/snap/core18/2708/usr/bin/passwd
/snap/core18/2708/usr/bin/sudo
/snap/core18/2708/usr/lib/dbus-1.0/dbus-daemon-launch-helper
/snap/core18/2708/usr/lib/openssh/ssh-keysign
/snap/core18/2128/bin/mount
/snap/core18/2128/bin/ping
/snap/core18/2128/bin/su
/snap/core18/2128/bin/umount
/snap/core18/2128/usr/bin/chfn
/snap/core18/2128/usr/bin/chsh
/snap/core18/2128/usr/bin/gpasswd
/snap/core18/2128/usr/bin/newgrp
/snap/core18/2128/usr/bin/passwd
/snap/core18/2128/usr/bin/sudo
/snap/core18/2128/usr/lib/dbus-1.0/dbus-daemon-launch-helper
/snap/core18/2128/usr/lib/openssh/ssh-keysign
/snap/core22/522/usr/bin/chfn
/snap/core22/522/usr/bin/chsh
/snap/core22/522/usr/bin/gpasswd
/snap/core22/522/usr/bin/mount
/snap/core22/522/usr/bin/newgrp
/snap/core22/522/usr/bin/passwd
/snap/core22/522/usr/bin/su
/snap/core22/522/usr/bin/sudo
/snap/core22/522/usr/bin/umount
/snap/core22/522/usr/lib/dbus-1.0/dbus-daemon-launch-helper
/snap/core22/522/usr/lib/openssh/ssh-keysign
/bin/umount
/bin/fusermount
/bin/su
/bin/mount
/bin/ping
```

部分程序及其对应的权能如下：

![image-20230313221557264](picture\image-20230313221557264.png)

#### 4.实现一个程序其满足以下的功能：

(1)能够永久的删除其子进程的某个权能。

(2)能暂时性的删除其子进程的某个权能。

(3)能让上面被暂时性删除的权能重新获得。

##### 常规功能测试

使用`sudo`执行程序后设置权能，并在成功切换成普通用户与普通组时传递权能，进行ping测试：

![image-20230317161325882](picture\image-20230317161325882.png)

暂时删除cap_net_raw权能后进行ping操作失败：

![image-20230317161955671](picture\image-20230317161955671.png)

恢复cap_net_raw权能后ping操作成功：

![image-20230317162329811](picture\image-20230317162329811.png)

永久删除cap_net_raw权能后ping操作失败：

![image-20230317162822103](picture\image-20230317162822103.png)

尝试恢复被永久删除的cap_net_raw权能失败：

![image-20230317163154696](picture\image-20230317163154696.png)

##### 异常功能测试

未使用`sudo`执行程序：

![image-20230317163419574](picture\image-20230317163419574.png)

刚进入程序就尝试恢复：

![image-20230317163521263](picture\image-20230317163521263.png)

输入选项错误：

![image-20230317163644368](picture\image-20230317163644368.png)

权能名称错误：

![image-20230317163805062](picture\image-20230317163805062.png)