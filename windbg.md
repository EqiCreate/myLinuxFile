### tools
- procdump
	- procdump -ma -i @pat
- ppee 获取exe 信息
### prepare
- .loadby sos clr
- .load xxx/sosex.dll（.load D:\sosex_64\sosex.dll）
- 捕捉事后异常
    - AeDebug+procdump
### command
- !dumpdomain 展示所有domain(systemdoamin,shared domain,domain1)
- !address -summary 全部分配的虚拟内存
    - !heap -s ：进程堆
    - !eeheap -gc :clr 堆
    - !dumpheap xx_begin xx_alloced : 展示范围内的对象
        - !dumpheap -min 0nxxx -max 0nxxx : 寻找大小在范围内的所有对象
        - !dumpheap -type System.String
        - !dumpheap -mt xxx: 根据方法表查找
- !fq : 终结器队列
- dp 0x?? 显示内存，包括指针等
- !clrstack -L 显示局部变量
- !sos.help + msn 
- 对于类，引用类型可使用!do 0xxx 找到对象体
- !dumpheadp -type person 托管堆中寻找对象
- 调试
    - p 单步执行 f10
    - t 进入调用函数 f11
    - pc 继续运行直到遇到call 函数
        - tc 继续运行直到遇到函数体内的call
    - pt 继续运行直到遇到return
        - tt 继续运行直到遇到函数体内的return
- 非托管代码调试C /C++
    - 线程栈 k
    - clr 托管栈 !clrstack
    - 混和 !dumpstack
- !name2ee module_name!namespace.classname.functionname
- !bpmd module_name!namespace.classname.functionname 给托管下断点
- !mbm module_name!namespace.classname.functionname 给托管下断点
- 对于泛型，需要找到最终的方法名等
- 对象检查
    - 找到对象地址后, !do xxx
    - 数组展开 !da -details xxx
- 局部值类型
    - r esp
- 事后调试
    - !t
    - !pe 异常描述
- 线程调试
    - 所有线程执行command : ~*e !command
    - 死锁
        - !dlk(只适用与framework)
        - (netcore)!syncblk 查看同步块
            - kb 查看非托管以及入参数，方便查看入参
            - !do 查看入参对象
    - 孤儿锁(适用monitor.enter 后没有相应monitor.exit后续导致)
    - 销毁线程(非托管逻辑里面操作lock导致) ：相当复杂!!!
        - 查看!syncblk 看看是不是损坏了
- 线程同步与原语
    - 事件原语
        - autoresetevent
        - manualresetevent
        - semaphore 内核态维护一个数值
        - monitor:基于autoresetevetn,
        - thinlock:轻量级自旋锁 ，没有和内核态交互,默认线程少的时候使用，写入lock对象中的-0x4,一般
        执行速度很快的时候使用，如果很多线程，clr可能转成monitor内核态锁
    - !handle 
    - !syncblk: 查看同步块，内核态，如果使用了autoresetevent会被记录
- 汇编
    - u  ： 转成汇编
    - !ip2md xxx : 从地址获取相关的函数说明和所在的模块
    - !savemodule xxx: 从地址保存运行的模块
    - !eeversion  
    - !dso 将当前线程栈所有的变量显示
    - !gcroot xxx
- sosex
    - !mbp xxx numberrow : 直接从行数 下断点
    - !mdt xxx -r : 展示负责对象的全部属性
    - !mfrag : free 块
    - !dlk : 死锁检测
- 搜索
    - !strings /m:*%xxx : 匹配搜索
    - !mx xxx_module!*xx* : 万物匹配搜索
- 动态代码(如emit) 的调试
    - kb : 对应参数
    - !dumpmd xxx :根据指针找到方法名以及模块等/是否已经被Jit编译
    - 等待jit编译后，找准时间打入断点，然后查看dumpmd
- 程序集泄露
    - 通过!dumpdomain 查看程序集，如果过多，可能存在泄露的问题，可以bp到非托管的断点，
    一般可以通过 *CreateDymanicDomaoin*,进行生成程序集的拦截，进入断点后通过k/clrstack 进行查看(常见例子是xml 反序列化的new对象)
- 托管堆损坏(地址传给了非托管，后者越界操作)
    - mda : 强制在托管转向非托管时候进行事前事后确认同时gc
- 托管碎片化
    - 因为free块前后的对象被持有，不能很好合并free块
- 非托管泄露
- PInvoke
    - 操作
        - 查找非托管函数 : x kernel32!*beep*
        - bp或者bm xxx 断点 g
        - !clrstack-> InlinedCallFrame: @a
        - dp @a L1->@b
        - ln @b 或者 u @b 
    - 崩溃
        - 调用协定
            - 先断点到非托管函数
            - dv 获取非托管得到的参数
            - !dumpmt xxx(方法表地址xx)
            - r 读取当前寄存器值 ecx (@c)
            - du @c
        - 委托异步(委托被托管gc导致)
            - 适用gchandler 固定回调函数指针,然后再析构这个handler
        - 非托管内存泄露(非托管没有回收内存)
            - 开启栈跟踪 stack back traces: gflags /i demo.exe +ust
            - !heap -s 找到最多句柄的那个heap @h
            - !heap -h @h ,选择其中一个@v1
            - !heap -x @v1 ,得到user ,记@v2
            - !heap -p -a @v2 查看调用栈


- 压测CLR
    - perview 采集数据