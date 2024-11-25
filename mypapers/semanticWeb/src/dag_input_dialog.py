# dag_input_dialog.py
import tkinter as tk
from tkinter import simpledialog, messagebox

def get_dag_input():
    # 创建隐藏的主窗口
    root = tk.Tk()
    root.withdraw()

    # 弹出对话框获取DAG图的相关信息
    dag_info = simpledialog.askstring("输入DAG图", "请输入您的DAG图（使用节点间的关系表示，例如 A->B, B->C）:")

    if dag_info:
        # 简单验证输入格式
        if validate_dag_input(dag_info):
            return dag_info
        else:
            messagebox.showerror("错误", "DAG图格式不正确，请重新输入。")
            return get_dag_input()
    else:
        messagebox.showinfo("提示", "未输入DAG图。")
        return None

def validate_dag_input(dag_info):
    # 简单的格式验证：检查是否包含"->"
    return "->" in dag_info
