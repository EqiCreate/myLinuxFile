CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) DEFAULT NULL,
  `addr` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

--建立rand_str函数
DELIMITER $$
DROP FUNCTION IF EXISTS rand_str;
-- 若是存在就删除
create FUNCTION rand_str(strlen SMALLINT ) 
-- 建立函数名 rand_str  参数为返回的长度
RETURNS VARCHAR(255)
-- 返回值
BEGIN
    DECLARE randStr VARCHAR(255) DEFAULT 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890';
--     声明的字符串
    DECLARE i SMALLINT DEFAULT 0;
-- 声明 i 循环变量
    DECLARE resultStr VARCHAR(255) DEFAULT '';
-- 声明返回变量
    WHILE i<strlen DO
        SET resultStr=CONCAT(SUBSTR(randStr,FLOOR(RAND()*LENGTH(randStr))+1,1),resultStr);
        SET i=i+1;
    END WHILE;
    RETURN resultStr;
END $$
DELIMITER ;


--建立add_user存储函数
DROP PROCEDURE IF EXISTS `add_user`;
DELIMITER $$
CREATE PROCEDURE `add_user`(IN n int)
BEGIN
    DECLARE i int unsigned DEFAULT 0;
    WHILE i < n DO
        INSERT INTO `user`(name,addr) VALUES (rand_str(15),rand_str(15));
        SET i = i+1;
    END WHILE;
END $$
DELIMITER ;

--调用存储过程
CALL add_user(100000);

-- DELIMITER $$
-- DROP FUNCTION IF EXISTS rand_str;
-- create FUNCTION rand_str(strlen SMALLINT ) 
-- RETURNS VARCHAR(255)
-- BEGIN
--     DECLARE randStr VARCHAR(255) DEFAULT 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890';
--     DECLARE i SMALLINT DEFAULT 0;
--     DECLARE resultStr VARCHAR(255) DEFAULT '';
--     WHILE i<strlen DO
--         SET resultStr=CONCAT(SUBSTR(randStr,FLOOR(RAND()*LENGTH(randStr))+1,1),resultStr);
--         SET i=i+1;
--     END WHILE;
--     RETURN resultStr;
-- END $$
-- DELIMITER ;