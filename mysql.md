## MYSQL
### 20230421
- 规范
  ```sql
  # Innodb 禁止外键约束？:级联删除会导致令人疑惑的现象，耦合度高，难以升级，并发困难
  # 精确浮点数用decimal代替double 和 float 
  # INT 而不是INT(4): 存储都是按照整形储存的，后面的数字只是显示不同的作用，比如预警像int(11)即是告诉最高整型只到10位，int(4)表示字节占用4个
  # 不使用text和blob类型？blob是大对象二进制，不太好，text对于搜索不太友好
  # 禁止存储图片和文件
  # 不使用enum而是tinyint
  # year(4) 而不是year(2)
  # 字段not null?  : null值的计算比较麻烦有坑，最好有默认值


  ## 数据更新需要加入两个时间段来记录生效和过期的时间点，甚至可以再加一个status： 这是因为update之后之前的数据就无法追溯了，所以可以增加一个status或者version 进行记录，总之就是将一个简单的update操作改成一个update+insert; 或者再建一个历史表，将之前的历史记录insert到历史表中，而主表只是update
  ## 主键和唯一键 区别 : 主键约束更强，要求非非空，而唯一键只是要求非null值不同，并且不对null值进行唯一性甄别，另外如果联合唯一键有null则可能会出错!!!

  ## groupby 后默认是有orderby的，可以强行写一个orderby null 来覆盖orderby的操作？

  ## 空间换时间的方式 create index 在有些情景下，大量的查询可以进行简化后再删除index 
  ## 可以利用trace等工具进行steps ，或者show status like '%handler%' 查看索引记录量进行检查
  ## 注意字符串的统一关系不     
  ## insert在不同的会话中，因为insert 过程如果需要作duplicate的操作，会拥有S锁共享，如果有其他会话中存在多个insert就可能会造成死锁！！！     总之就是insert操作不要分太多个单独的语句！！
  ## 与多个insert在同个会话相似，如果有delete语句就会获取X，如果此时其他会话取得S锁就会进行队列，而第一个会话中此时要重新获取X锁就必须等待第二个会话释放S锁，产生死锁，总之，会话不能丢失锁，获取重新获得锁，因为可能有其他会话已经加锁!!
  ## 存储过程难以调试，不利于分布式
  ## 建议rename 代替drop/truncate

  ```
### 20230202
- <mark style="color:red">sql 调优</mark>
- 安全性能攻击
  - 脚本注入: 如 用户名后面跟着'导致不需要密码就可以登陆;或者 直接在密码后面跟着select 语句进行尝试获取其他信息；或者利用返回结果尝试获取密码长度等信息
  - CSRF:跨伪站请求,因为存在cookie，当用户登陆后只要能欺骗用户点击钓鱼链接就能将如转账的请求连同cookie 发送给攻击者. 一般使用令牌token方式进行防御
- 索引
  - 索引不要超出6个
  - 索引不要进行计算比如乘除!
- 预编译查询: 程序查询的时候进行预编译而不是动态让数据库生成,预防脚本注入
  - 数据库用prepare 语句可完成预编译
- where 连接顺序,尽量放在join 之后减少操作量
- 压缩sql语句，减少权限校验等操作
- 使用表的别名,并且用t.xxx的方式，减少编译进行解析的时间
- union all 而不是union ，当结果是不需要去重的时候,减少去重所需的时间
- 采用临时表记录一些不需要经常更改的数据，缓存
- 能用触发器就用触发器，少用事务，触发器insert和update注意别死循环!
- 避免使用游标????,因为消耗内存，且会锁行导致并法问题!
- 字段数据类型需要考虑存储问题，如果量特别大的话
- 不要使用select *，尽量包含所需的列名
- 能用between ，不用in: 因为当过滤条件为空时，in 会跳过，而between会认为不符合过滤条件，严格限制输出
- update 不要全部的列
- 能用select into 就不用insert into
- 语句查看执行explain
  - type： ALL < index(全表索引) < range ~ index_merge < ref < eq_ref(join) < const < system
- ……
- <mark style="color:red">sql 死锁的发生和应对</mark>
  - 死锁可以避免都会牺牲性能，不损失性能只能尽量减少可能存在的概率或者次数
  - 死锁发生的情况
    - 执行顺序相反,如互相拥有对方的锁，而且至少有一方是排他锁时
    - 排他锁正在执行且此时有其他事务获取共享锁，且正好也要升级为排他锁，由于第一个排他锁因为第二个事务无法释放共享锁(处在等待升级的状态)
    - ……
  - 死锁的应对
    - 时刻记住基数*概率，第一步永远是紧跟业务，是否能分库分表，是否能简化流程
    - 技术上减少概率或者次数: 如规定代码程序的事务执行逻辑(先后顺序等),利用数据库框架本身支持的技术比如sqlserver的更新锁，oracle的for update 语法糖,前端注意事务线程等，避免执行事务太久,避免出现roll back
    - 出现后补救
      - 确认死锁:show engine innodb status;
      - kill process : use information_scheme; select * from processlist where db=xxx; kill xxx;
    - example
      ```sql
      例子1->
      transaCtion A:
        START TRANSACTION;
        SELECT * FROM t WHERE i = 1 LOCK IN SHARE MODE;//1. 获取共享锁
        DELETE FROM t WHERE i = 1; //3. 死锁产生，因为此时事务B 想要排他X锁，等待A，而A 尝试升级为排他X锁排在事务A 后，等待A的释放.
      
      transaction B:
        START TRANSACTION;
        DELETE FROM t WHERE i = 1; //2. 这里事务B 将会被阻塞，因为尝试获取排他X 锁,然而共享锁在事务A

      例子2 ->
      不同表格之间操作的顺序相反导致

      ```
    - ……
- sql fn
  - year(xxx) : 获取某字段的年份
  - now () : 当前时间
  - timestampdiff(month,xxxx,now())/12 : 根据时间距离计算岁数
  - current_date : 当前日期 : select current_date;
  - describe table; 显示表的列名信息等
  - 取余操作只能用MOD(m,n) !!
  - cast/convert : 转换格式 i.e. cast(xx as unsigned)
  - 设置变量如果右边是select 应该加括号 i.e. set @a= (select xxx from table);
  - 变量的初始化在select中的用法
    ```sql
    # select 多表实际上因为多表没有筛选过滤所以也可以看成两个select
    # 在(select xxx as table 中@v:=0) 代表已经声明了该变量 并且赋初始值
    # 在每次select @v 中，每触发一次就会执行一次变量操作
    select sid,@r:=@r+2 as num_row,@t:=@t+1 as tt from score,(select @r:=0 )a
    s B,(select @t:=0)as C order by sid;

    # 获取行数在mysql有fn: row_number()
    select * from (select * ,row_number() over (order by id) as rn from orderT) as V where V.rn <= (select 0.2 * count(*) from orderT);
    ```
### 20230201
- 拥有主键的表相当于父表，拥有外键的表是子表，当有数据插入到子表的时候必须该行数据是被包含在父表中的.
- Group by 需要聚集函数进行划分数据，比如Min,Max，Sum,Count,Avg,而不是一个简单的字段进行划分
- having 与where 区别, where是 笛卡尔积进行数据筛选，耗时,having 会在聚集函数筛选之后再进行过滤
- exists 和 not exists
- 去重 select distinct xx from table ; select (distinct (xx)) from table
- 正序 逆序 : select xx from table order by xx asc; select xx from table order by xx desc;
- insert into 多个实体: insert into table values (v1),(v2),(v3)……
- 判断null : where xx is null ; where xx is not null
- update table set col=xxx where x
- delete from table where xx
- 限制最终行数: select top xx from table ; select xx from table limit 10;
- sql 中的模式匹配 %xx% ; _xx; [sd]
- where in ;not in
- select into table
- insert into select xx from table
- case when then end
  ```js
    SELECT CustomerName, City, Country
    FROM Customers
    ORDER BY
    (CASE
        WHEN City IS NULL THEN Country
        ELSE City
    END);
  ```
- useful fn
    - rank()
    ```sql
    SELECT name, rollno, score
    , rank() over (order by score, dob desc) as rnk
    FROM score
    WHERE examid = '1'
    ORDER BY score DESC, dob ASC

    # 传统做法
    SELECT
    a.id,
    a.score AS score,
    ( SELECT count( DISTINCT b.score ) FROM test_score b WHERE b.score >= a.score) AS Rank 
    FROM
        test_score a 
    ORDER BY
        a.score DESC;
    ```
    - isnull(xx,0) ; 
- procedure: create procedure xxx; exec xxx;
- check (xxx <limit): 限制条件
- procedure
  ```sql
    CREATE TABLE orderT (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    C2 VARCHAR(100),
    C3 VARCHAR(100) )
    ENGINE=InnoDB
    COMMENT="NDB_TABLE=READ_BACKUP=0,PARTITION_BALANCE=FOR_RP_BY_NODE";

    # procedure 
    use test_db;
    DROP PROCEDURE if EXISTS BatchInsert;
    delimiter //
    CREATE PROCEDURE BatchInsert(IN initId INT, IN loop_counts INT)
    BEGIN
        DECLARE dy_Var INT default 0;
        DECLARE dy_ID INT default 0;
        SET dy_Var = 0;
        SET dy_ID = initId;
        set autocommit=0;
        WHILE dy_Var < loop_counts DO
            INSERT INTO `orderT` (`C2`,`C3`) 
            VALUES (CONCAT('20220704', 100000000000 + dy_ID),CONCAT('C0', 512201907191454553491 + dy_ID));
            SET dy_ID = dy_ID + 1;
            SET dy_Var = dy_Var + 1;
        END WHILE;
        COMMIT;
    END//;

    delimiter ;
    --show procedure status ; -- 查看存储过程
    CALL BatchInsert(1, 1000);  -- 调用存储过程
  ```
### 20220511
- knowledge freequent
    - function & procedure : call precedure , function like max(),version()
    - mysql version like community version ,is not enable to have ablity to audit record of deleted operation.But enterprise can. Alternative plan is  in MariaDB,you can use 'Audit PLugin' tool to audit record of deleted opearation.
    - role
    ```sql
        create role develop;
        rant select,insert,update,delete on *.* to develop;
    ```
    - term
    ```js
        DML（data manipulation language）是数据操纵语言：它们是SELECT、UPDATE、INSERT、DELETE，就象它的名字一样，这4条命令是用来对数据库里的数据进行操作的语言。
        DDL（data definition language）是数据定义语言：DDL比DML要多，主要的命令有CREATE、ALTER、DROP等，DDL主要是用在定义或改变表（TABLE）的结构，数据类型，表之间的链接和约束等初始化工作上，他们大多在建立表时使用。
        DCL（DataControlLanguage）是数据库控制语言：是用来设置或更改数据库用户或角色权限的语句，包括（grant,deny,revoke等）语句。
    ```
    - in/exits/join
    ```js
        mysql in/exits ,will firstly query outer table and then inner table,slowing down query speed.
        mysql join ,better.
    ```
    - in / or
    ```sql
        select * from t1 where (id,k) in ((11,0),(22,0))  //which will go throught whole table
        select * from t1 where (id=11 and k=0) or (id=22,k=0) //which will go throught 'index'
    ```
    - preload
    ```js
        seletct count(*) from table; <==> buffer_pool_dump_at_shutdown
    ```
- script freequent
    - show
    ```js
        show variables;//all variables;
        show processlist;// working process;
        show procedure[function] status like '%xxx%';
    ```
    - setttings
    ```sql
        thread_pool_max_threads 
        optimiser_switch // generate outer table with index
    ```
- variable freequent 
```JS
    innodb_buffer_pool_dump_at_shutdown;// searilise entity to disk
    optimiser_switch // MRR ,multi range read,order IO rather than radam IO
```
- tool freequent
```js
    Sphinx //  finding keyword in whole context
    pt-kill //kill sql scirpt which execute slowly 
    druid  //Alibaba Thread Pool
    audit plugin //madiaDb plugin
    TokuDb   // useful log cache engine
```

- script
    - json
    ```sql
    create table `t1`(`id` int(11) NOT NULL,`context` json DEFAULT NULL,primary key(`id`))engine=InnoDB DEFAULT CHARSET=utf8 ;

    insert into t1 values(1,`{"name":"zhang","age":21},{"name":"Li","age":33}`);

    select id,json_extract(context,`$.name`) name from t1;

    ```
    - log_trust
    ```sql
        SET GLOBAL log_bin_trust_function_creators = 1;
    ```

- characteristic
    - function index i.e. id%10