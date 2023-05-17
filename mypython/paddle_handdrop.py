import paddle
from paddle.nn import Linear
import paddle.nn.functional as F
import os
import numpy as np
import matplotlib.pyplot as plt
# 定义mnist数据识别网络结构，同房价预测网络
class MNIST(paddle.nn.Layer):
    def __init__(self):
        super(MNIST, self).__init__()
        
        # 定义一层全连接层，输出维度是1
        self.fc = paddle.nn.Linear(in_features=784, out_features=1)
        
    # 定义网络结构的前向计算过程
    def forward(self, inputs):
        outputs = self.fc(inputs)
        return outputs


if __name__=='__main__':
    # 设置数据读取器，API自动读取MNIST数据训练集
    train_dataset = paddle.vision.datasets.MNIST(mode='train')
    train_data0 = np.array(train_dataset[0][0])
    train_label_0 = np.array(train_dataset[0][1])
    plt.figure("Image") # 图像窗口名称
    plt.figure(figsize=(2,2))
    plt.imshow(train_data0, cmap=plt.cm.binary)
    plt.axis('on') # 关掉坐标轴为 off
    plt.title('image') # 图像题目
    plt.show()

    print("图像数据形状和对应数据为:", train_data0.shape)
    print("图像标签形状和对应数据为:", train_label_0.shape, train_label_0)
    print("\n打印第一个batch的第一个图像，对应标签数字为{}".format(train_label_0))