## MYSQL
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