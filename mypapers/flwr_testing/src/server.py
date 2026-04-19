# server.py
import flwr as fl

def main():
    # 配置服务器策略
    strategy = fl.server.strategy.FedAvg(
        fraction_fit=0.5,  # 参与训练的客户端比例
        min_fit_clients=2,  # 最少参与训练的客户端数量
        min_available_clients=2,  # 最少可用的客户端数量
    )

    # 启动服务器
    try:
        fl.server.start_server(server_address="127.0.0.1:9999", strategy=strategy)
    except Exception as e:
        print(f"Server error: {e}")

if __name__ == "__main__":
    main()
