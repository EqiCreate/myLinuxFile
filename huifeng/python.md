
### 1031
    - 没有double 类型
    - print('%.8f'&x)
### 0527
    - 坑
        - 作用域,所有的数据都是对象，优化的时候可能会直接引用而不是创建新对象
        - 所有的属性都是public
            - private 需要特殊写法?
            ``` ython
                # 限制只能绑定的对象
                __slot__=("'_age'")
                @property
                def age(self):
                    return self.age
                @age.setter
                def age(self,age):
                    self._age=age
            ```
            
        - 静态方法
            ```python
                @staticmethod
            ```
        - 继承多态
        ```python
        @abstractmethod
        ``` 
        - 单例模式
            ```js
                from functools import wraps
                from threading import RLock
                @wraps(xxx)
            ``` 
        - 特性
          - 魔术方法
            - __call__ 令实例化的对象有函数
            - __init__ 构造函数
            - __iter__ 和 __next__ 迭代器协议应用
            - __main__ 
            - __rstr__ 迭代输出调用
            - __str__ == ToString()
          - 切片
            - 前闭后开， a[1:4] 取值只有1,2,3
          - asyncio ？？？
### debug
- pycharm ->run/edit configuration
  - config script path / parameters(such as runserver in Django) 
  - click debug
- vitualEnv
  - 下载套件
- 一次性安装
  - pip install -r requirments.txt(txt show list of required librarys)
    