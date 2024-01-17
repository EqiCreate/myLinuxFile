#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
将文本整合到 train、test、val 三个文件中
"""

import os

def _read_file(filename):
    """读取一个文件并转换为一行"""
    with open(filename, 'r', encoding='utf-8') as f:
        return f.read().replace('\n', '').replace('\t', '').replace('\u3000', '')

def save_file(dirname):
    """
    将多个文件整合并存到3个文件中
    dirname: 原数据目录
    文件内容格式:  类别\t内容
    """
    # f_train = open('data/cnews/cnews.train.txt', 'w', encoding='utf-8')
    # f_test = open('data/cnews/cnews.test.txt', 'w', encoding='utf-8')
    # f_val = open('data/cnews/cnews.val.txt', 'w', encoding='utf-8')
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    data_dir = os.path.join(parent_dir, 'data/cnews')
    if not os.path.exists(data_dir):
        os.mkdir(data_dir)
    trainTxtPath = os.path.join(data_dir, 'cnews.train.txt')
    testTxtPath = os.path.join(data_dir, 'cnews.test.txt')
    valTxtPath = os.path.join(data_dir, 'cnews.val.txt')

    f_train = open(trainTxtPath, 'w', encoding='utf-8')
    f_test = open(testTxtPath, 'w', encoding='utf-8')
    f_val = open(valTxtPath, 'w', encoding='utf-8')
    for category in os.listdir(dirname):   # 分类目录
        cat_dir = os.path.join(dirname, category)
        if not os.path.isdir(cat_dir):
            continue
        files = os.listdir(cat_dir)
        count = 0
        for cur_file in files:
            filename = os.path.join(cat_dir, cur_file)
            content = _read_file(filename)
            if count < 5000:
                f_train.write(category + '\t' + content + '\n')
            elif count < 6000:
                f_test.write(category + '\t' + content + '\n')
            else:
                f_val.write(category + '\t' + content + '\n')
            count += 1

        print('Finished:', category)

    f_train.close()
    f_test.close()
    f_val.close()


if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.dirname(script_dir)
    data_dir = os.path.join(parent_dir, 'data/thucnews')
    save_file(data_dir)
    data_cnews_dir = os.path.join(parent_dir, 'data/cnews')
    # print(len(open('data/cnews/cnews.train.txt', 'r', encoding='utf-8').readlines()))
    print(len(open(os.path.join(data_cnews_dir, 'cnews.train.txt'), 'r', encoding='utf-8').readlines()))
    print(len(open(os.path.join(data_cnews_dir, 'cnews.test.txt'), 'r', encoding='utf-8').readlines()))
    print(len(open(os.path.join(data_cnews_dir, 'cnews.val.txt'), 'r', encoding='utf-8').readlines()))
    # print(len(open('data/cnews/cnews.test.txt', 'r', encoding='utf-8').readlines()))
    # print(len(open('data/cnews/cnews.val.txt', 'r', encoding='utf-8').readlines()))
