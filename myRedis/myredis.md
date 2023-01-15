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