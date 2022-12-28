### 20221226
- 多次循环执行命令
  - 如 执行50次循环的sensor : for i in {1..50}; do sensors;sleep 2;done
- 开机启动的设置
  - sudo systemctl disable mysql // 设置开机启动
  - systemctl is-enabled mysql //判断是否开机启动
  - service --status-all // 查询当前所有的任务 ，不断是否已经stop
- 命令行是程序shell 放在系统变量下(ENV | grep PATH) 的，类似git 命令都可以在/usr/bin 中找到程序


### 20221128
- grep
  - .* 匹配所有
  - * 匹配前面出现或者每出现的字符
  - \+ 匹配至少出现一次的可选择[]
  - \<parttern\> 匹配句首 句尾之间的数据
  - 分组，因为诸如[2]只能【匹配前面一个字符，所以采用分组进行整体多次匹配如
    ```shell
        grep "\(aaa\)\{2\}.*" text.txt :只能匹配重复aaa 2次的数据！
        
    ```
  - 分组向后引用： 其实就是动态字符，代替之前的分组内容
    ``shell
        grep "\(aaa\)\{2\}.*\1" text.txt :只能匹配重复aaa 2次且最后又出现aaa的数据！ 如aaaaaabbbbbbbaaa
        
    ```
- awk
  - 文件流编辑语言，默认是按照行进行处理
  - awk '{command $列}' 列从1 开始，0 默认是原数据 如awk '{print $1}' text.txt, 打印第1 列，默认以空格为分割符，可以进行修改
  - 内置函数，比如sin ,exp,sum等函数进行数学运算
  - 可进行if else 等逻辑功能
  - 可进行正则匹配等？？
    - 如awk -F: '$1=="root"{print $2}' text.txt 定位数据比如root:mike这一行
  - 可设置变量等
  