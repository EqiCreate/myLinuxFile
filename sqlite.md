id : 2ab58778c2967968b94284e989e43dc11791f548
server-id:  db8d9a75b2647558788036572713663d03d58ad2
admin-user: michael (password is "egdBQZU85f")

insert into temp_B values(2,"2"),(3,"3");  
select * from temp_B;
select * from temp;
explain select * from temp inner join temp_B on temp.one = temp_B.uuid;
### 20221126    
- VDBE 虚拟机环境数据引擎,解析数据到各个操作系统
- plain insert xxxx values(): 显示 执行多少条vdbe指令
  - 主要看comment, r[] 数组代表列，
  - halt 终止
  - Transcation
  - xx???
- 虚拟表 -- 视图
  - 只读
  - 无法删除，插入更新数据但是，数据库的更改会更新到视图上
- 语法书写顺序图
  - <img src="./select1.png">
  - 先进行From，将表进行连接，查询出的是个“笛卡尔积”集，然后交给on子句连接过滤，再交给where进行条件过滤，再进行Group分组，再进行having过滤分组，再进行select输出，如果有order，limit，offset再对结果集进行排序，再返回限定行数和限定偏移量的结果集
  - on > join > where ，实际中on 先过滤后再进行join，不同于where，则是笛卡尔积之后全部布尔运算
    -   SQLite官方文档中提出，针对 inner join和cross join，where子句和on子句没有区别。但是在left outer join区别很大
  - group by 也会先执行排序操作，与order by相比，group by只是多了排序之后的分组
- FTS3 虚拟表，进行全文检索???
  - FTS5????
- Wal 高效单机日志算法？？？？ rollback 
  - 修改并不直接写入到数据库文件中，而是写入到另外一个称为 WAL 的文件中；如果事务失败，WAL 中的记录会被忽略，撤销修改；如果事务成功，它将在随后的某个时间被写回到数据库文件中，提交修改。