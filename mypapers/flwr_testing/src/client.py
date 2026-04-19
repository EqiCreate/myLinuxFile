# client.py
import flwr as fl
import torch
from torch.utils.data import DataLoader, Dataset
import torch.nn.functional as F
from model import GNNModel
from torch_geometric.data import Data
from term_detection import detect_specialized_terms
from dag_input_dialog import get_dag_input
from dag_parser import parse_dag_to_graph
from knowledge_base import KnowledgeBase
from embedder import Embedder

# 自定义数据集
class GraphDataset(Dataset):
    def __init__(self, graphs):
        self.graphs = graphs

    def __len__(self):
        return len(self.graphs)

    def __getitem__(self, idx):
        return self.graphs[idx]

# 定义Flower客户端
class FlowerClient(fl.client.NumPyClient):
    def __init__(self, model, train_loader, device, knowledge_base, embedder):
        self.model = model
        self.train_loader = train_loader
        self.device = device
        self.knowledge_base = knowledge_base
        self.embedder = embedder

    def get_parameters(self,config):
        return [val.cpu().numpy() for _, val in self.model.state_dict().items()]

    def set_parameters(self, parameters):
        state_dict = self.model.state_dict()
        for key, val in zip(state_dict.keys(), parameters):
            state_dict[key] = torch.tensor(val)
        self.model.load_state_dict(state_dict)

    def fit(self, parameters, config):
        self.set_parameters(parameters)
        self.model.train()
        optimizer = torch.optim.Adam(self.model.parameters(), lr=0.01)
        for epoch in range(1):
            for batch in self.train_loader:
                batch = batch.to(self.device)
                optimizer.zero_grad()
                output = self.model(batch)
                loss = F.nll_loss(output, batch.y)
                loss.backward()
                optimizer.step()
        return self.get_parameters(), len(self.train_loader.dataset), {}

    def evaluate(self, parameters, config):
        self.set_parameters(parameters)
        self.model.eval()
        loss, accuracy = 0.0, 0.0
        # 这里可以添加评估代码
        return float(loss), len(self.train_loader.dataset), {"accuracy": float(accuracy)}

def process_user_input(user_input, knowledge_base, embedder):
    detected_terms = detect_specialized_terms(user_input, knowledge_base, embedder)
    if not detected_terms:
        # 如果没有检测到相似的术语，弹出对话框让用户输入DAG图
        dag_info = get_dag_input()
        if dag_info:
            # 解析DAG图
            graph, node_list = parse_dag_to_graph(dag_info, embedder)
            # 添加到知识库中
            term = " ".join(node_list)  # 假设术语是节点列表的组合
            knowledge_base.add_term(term, graph)
            print(f"新增术语和图结构: {term}")
            return graph
        else:
            print("未提供DAG图信息。")
            return None
    else:
        print(f"检测到相似术语: {detected_terms}")
        # 根据检测到的术语，提取对应的图结构
        # 这里可以根据具体需求进行处理
        return detected_terms
        # return None

def main():
    # 初始化嵌入器和知识库
    embedder = Embedder()
    knowledge_base = KnowledgeBase(embedder)

    # 假设从某处获取用户输入
    user_input = "我在研究图神经网络的应用。"

    # 处理用户输入
    new_graph = process_user_input(user_input, knowledge_base, embedder)

    # 加载本地数据
    graphs = []  # 加载或生成您的图数据
    if new_graph:
        graphs.append(new_graph)
    train_dataset = GraphDataset(graphs)
    train_loader = DataLoader(train_dataset, batch_size=32, shuffle=True)

    # 初始化模型
    input_dim = 768  # BERT的[CLS]嵌入维度
    hidden_dim = 256
    output_dim = 2  # 根据任务调整
    model = GNNModel(input_dim, hidden_dim, output_dim)
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model.to(device)

    # 启动客户端
    client = FlowerClient(model, train_loader, device, knowledge_base, embedder)
    fl.client.start_numpy_client(server_address="localhost:9999", client=client,insecure=True)

if __name__ == "__main__":
    main()
