### 20221127
- lua 
  - 分布式锁
    - setnx??:判断key是否存在和设置key的值作为一个指令执行的操作
- epoll 多路复用
  - 传统是遍历所有的连接，将fd集合拷贝到内核中，开销大，且支持的文件描述数量小只有1024个
  - epoll 维持fd列表，一直在内核
    - epoll不再是一个单独的系统调用，而是由epoll_create/epoll_ctl/epoll_wait三个系统调用组成。​ epoll通过在Linux内核中申请一个简易的文件系统(文件系统一般用什么数据结构实现？B+树), 将调用分成了3部分。
      - create : create root of b+ tree and defined it as epoll handler,linux will define double-link structure add record callback events.
      - ctl :add alive socket to the tree
      - wait :find alive event by the double-link 
    - 所有添加到epoll中的事件都会与设备(网卡)驱动程序建立回调关系，也就是说，当相应的事件发生时会调用这个回调方法,从而实现多路复用。
  - 
- redis 事件机制
  - IO 事件
    - 可读事件、可写事件和屏障事件(反转事件)
  - time 事件
    - 如serverCron 函数中会以 1 秒 1 次的频率，检查 AOF 文件是否有写错误。如果有的话，serverCron 就会调用 flushAppendOnlyFile 函数，再次刷回 AOF 文件的缓存数据
- redis 事件
  - 封装系统epoll 时间为aeCreateEventLoop
  - 所有网络 IO 事件对应文件描述符的掩码，初始化为 AE_NONE，表示暂时不对任何事件进行监听
  - 要监听的文件描述符 fd 在数组中的类型不是 AE_NONE，则表明该描述符已做过设置，那么操作类型就是修改操作
  - 
### 20221125 
- Makefile 
  - 规则
    ``` makefile(
      目标(生成的文件，all 表示？？？) : 依赖（由哪些文件生成）
        命令1
        命令2
    ```
  - $() 取值
  - var a := A, 克隆赋值
  - var a= A ,浅拷贝赋值，会被后面逻辑覆盖
  - var a?= A ,如果变量没有赋值，则赋值后面的值
  - @echo "aa" 去除回显@  
  - 函数 $(funcname p1,p2,p3 ...)
  - clean: rm -rf xxxx
  - .PHONY 伪目标??? 强制让程序继续下
  - Makefile.dep 先执行的部分??????
- 在.c 文件中定义变量，在.h 文件中extern 这个变量，使这个变量变成全局变量！！！
- define 宏定义 是可以传参的比如,
  ```c
    #define show(a,b,c)\
    printf(xxxx,a)...
  ```
- dict 类型????
  - dictType : 定义函数指针,包括 hashFunction,keyDup,valDup,keyComparer,keyDestructor,valDestructor,expandAllowed
  - dictEntry: 链表，union 一个val
    - 定义一个数组指针 metadata
    - 定义 函数指针 key (void *key)
- ACL :access control list
  - 规定一个默认的账户用来实现不需要密码登陆的服务??
- aeEventLoop 实际上根据这个事件是否结束来决定是否退出server 进程
- epoll 基于回调的: 多路复用?????
- connection
  - type
    - connectionState
    - connetctionType
      - conn
      - init();cleanup();
    - flag,refs,last_error,private_data,fd
    - Func: conn_handler,write_handler,read_handler
  - 行为
    - 新建connLister ,创建TCP，TLS，UNIX的connListenr并记录在全局数组中
    - 连接connListen(Listener)
      - listener->ct->listen(listener)
    - 创建socket句柄 ,createSocketAcceptHandler-->aeCreateFileEvent-->aeEventLoop+aeFileProc
    - InitServerLast
      - bioInit 创建线程??+initThreadedIO
### 20221124
- C function
  - memcpy(dest.source,len)
  - __attribute__((malloc)):告诉gcc等编译器,不检查是否内存null？？？？
- jemolloc
  - 基于？？？的内存分配器
  - 分配内存使用SLAB算法，类似伙伴算法，2的指数次大小块，但是在基础上增加优化??
  - 根据线程分配内存，一个cpu一个arena块
  - 每个arena 包含一个bin数组，可能是用于记录大中小3个档位的？？？
  - 每个arena 被划分几个chunks,chunks包含若干个run,run由连续page组成
  - run还会划分若干个region
  - 每个线程对应一个tcache私有缓存，对应一个arena,本身也有一个bin数组，但是它没有run。每次tcache从arena中申请一批内存，首先在tcache中寻找，从而避免锁竞争，分配失败才通过run执行内存分配
  - 目前之使用了jemelloc的malloc,realloc,free，可能可以有更多的内存分配器???
- sds
  - ``inline``
    - 使用动态函数体代替函数栈实现对函数入栈的需求，但是同时会使程序内部代码量激增,适用于大量迭代的函数体(非函数声明部分)
  - long long value : 64位
  - 动态字符类型
    - typedef char* sds;
    - memcpy ,copy source to sds.buf array
    - sdsfree : zfree occupied mem
    - catlen : makeroomfor if need,and change the exiting sds
    - catlenprintf: 格式化输入到sds，但是实际上是采用数组赋值的方式来实现逻辑，代价大，效率低 ==> sdsfromlonglong() 
    - sdsrange : 重写改动buf array部分,useful 
    - sdscpy : 尽可能的在原来的source参数中更改而不是输出一个新的值. 不要这样写:  s = sdscpy(s,xxx) ！！！！因为你不知道返回的s是旧的那个还是新的那个
    - sdsRemoveFreeSpace: 当字符已经被裁剪后可使用对其进行空洞释放空间
  - Heaer+binary safe c array+ null term
  - 缺点
    - 不要 即当输入又当输出，可能bug
    - 如果程序在多个不同地方引用的时候，最好压缩进带有引用技术的数据类型，否则容易内存泄漏
      比如 
      ```js
        struct myshardString{
          int refcount;
          sds string;
        }
      ```
  - 优点
    - 易使用,因为是二进制的，所以???
    - 可以当成数组使用,应该是buf当成sds的指针了???
### 20221122
- data structure
  - string,hash,list,set,zset(有序列表)
    - string ==>sds 空间换时间，加入meta info
  - geospatial地图信息
  - hyperloglog ??
    - 基数统计算法的数据结构，如统计网站的
  - bitmap 位图
- 虚拟内存机制
- 缓存
  - 穿透    :不存在的寻找key，导致每次都是到达数据库db
    - 检测参数
    - 过期时间设置
    - hash 散列判断是否大概率存在数据库上
  - 雪崩    : 大量的key同时到期，导致数据库压力
    - 集群
    - 设置过期时间
  - 击穿    : hot key 在过期时候大量并法导致 数据库崩溃
    - 互排斥锁
    - 永不过期 redission
    - 解决hot key问题，分片副本 二级缓存
  - 内存过期淘汰机制
    - LRU
- 持久化
  - 快照存储
  - AOF 日志存储 增量的问题

- 使用
  - 缓存
  - 队列 不常用
  - 位图记录
  - 高可用方案
    - 主从模式
    - 哨兵 --> 管理主从，设置主从
    - 集群cluster 类似UDP ???
    - 一致性问题
      - 延时双删（获取后再删一遍） + 多次重试
    - 原子性
      - 一般是事务性，但是事务如果多个步骤，则执行过的无法回滚
  - 分布式锁
    - redlock 类似投票选定是否解锁，释放锁???
  - C# 使用的情景
    - 可能是守护进程 client
    - 消息队列
    - 缓存 client? 