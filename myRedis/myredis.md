### 20230318
- /dev 中查看所有加载设备， 开头c : 字符设备 b: 块设备 
- /dev 中 ls -l + 设备 : 返回主设备号和副设备号，规定类型和同类型下的不同机号
- nslookup : 通过服务器名字得到ip

### 20230315
- 优化参数除了看makefile，需要看.make-settings
- network.c
  - acceptCommonHandler
  - createClient 
    - c.argv=null 
  - linkClient+ initClientMultiState
- init & lookup commands 
  - populateCommandTable
  - lookupCommandLogic
- read from client
  - socket
    - connSocketEventHandler
    - call_read ->processCommandAndResetClient->processCommand
    - call(c,CMD_CALL_FULL); ->commandDocsCommand
    - callHandler
    - handleClientsWithPendingWrites->installClientWriteHandler->connSocketSetWriteHandler
  - network
    - readQueryFromClient
    - processMultibulkBuffer  reset c->argv
    - processCommandAndResetClient
    - processCommand
- send message
  - beforeSleep
  - handleClientsWithPendingWritesUsingThreads
  - _addReplyToBufferOrList
  - writeToClient : 如果只是简单的command比如ping
### 20230214
- 内联函数inline ：根define 差不多，空间换取时间,多用于for循环中的重叠代码，且自身简短不带有其他循环. 好处是不浪费函数栈只是简单的复制
- attribute 声明: __attribute__(attribute_list): 一般是告诉编译器gunc如何处理内存中的数据如 __attribute__((aligned(8))) : 结构对齐8字节
### 20230129
- va_start  //将参数进行va_list：一个字符指针，可以理解为指向当前参数的一个指针，取参必须通过这个指针进行。
    2）va_start：对ap进行初始化，让ap指向可变参数表里面的第一个参数。第一个参数是 ap 本身，第二个参数是在变参表前面紧挨着的一个变量，即“...”之前的那个参数；
    3）va_arg: 获取参数。它的第一个参数是ap，第二个参数是要获取的参数的指定类型。按照指定类型获取当前参数，返回这个指定类型的值，然后把 ap 的位置指向变参表中下一个变量的位置；
    4）va_end：释放指针，将输入的参数 ap 置为 NULL。通常va_start和va_end是成对出现
- fmacros.h 文件 定义posix等开关变量
- volatile 原子变化定义变量
### 20230115
- gcc -I 可以将自定义的头文件加入标准类库中，之后include 直接可以用<>包裹
-  #include "fpconv_dtoa.h"  ??????
-  #include <stddef.h>	/* NULL size_t */???? 在cluster.c 关于staic_extern?
### 20230114
- for (uint_fast8_t i = 0x01; i & 0xff; i <<= 1) {} : 根据位数迭代
- 位重置
  ```C
  static inline uint_fast64_t crc_reflect(uint_fast64_t data, size_t data_len) {
    uint_fast64_t ret = data & 0x01;

    for (size_t i = 1; i < data_len; i++) {
        data >>= 1;
        ret = (ret << 1) | (data & 0x01); //ret 左移,数据右移，或起来
    }

    return ret;
  }
  ```
### 20230113
- h文件，实际上只是编译器复制粘贴一些函数等说明，方便编译器进行检查，实际上的函数实现链接器会从.o文件中寻找，如果重名还会报错
### 20230111 
- /dev/null:空设备，是一个特殊的设备文件，它丢弃一切写入其中的数据（但报告写入操作成功），读取它则会立即得到一个EOF,如cat $filename >/dev/null  则不会得到任何信息
  - > 代表重定向到哪里
  - 1 表示stdout标准输出，系统默认值是1，所以">/dev/null"等同于"1>/dev/null"
  - 2 表示stderr标准错误
  - & 表示等同于的意思，2>&1，表示2的输出重定向等同于1 
  
- shell
  - test 命令,用于检查某个条件是否成立
  - if [-n "$str"] :判断字符串是否为空
  - $SOURCE_DATE_EPOCH :Uinx环境变量 时间戳