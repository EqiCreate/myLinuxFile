## MYSQL
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