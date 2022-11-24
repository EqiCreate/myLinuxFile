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