### 20220510
- 不支持事务
- B 树,有磁盘IO 优势，但是不利于范围上下检索(mysql 是 B+)
- 主从赋值,切片, shared/config/router 分离服务，建立3个集群，代码易操作
- BSON 数据储存,二进制JSON