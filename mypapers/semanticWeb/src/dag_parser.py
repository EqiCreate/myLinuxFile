# dag_parser.py
import torch
from torch_geometric.data import Data

def parse_dag_to_graph(dag_info, embedder, input_dim=768):
    edges = dag_info.split(',')
    edge_index = []
    nodes = set()
    for edge in edges:
        source, target = edge.strip().split('->')
        nodes.add(source)
        nodes.add(target)
        edge_index.append([source, target])

    node_list = sorted(list(nodes))
    node_indices = {node: idx for idx, node in enumerate(node_list)}
    edge_index = [[node_indices[edge[0]], node_indices[edge[1]]] for edge in edge_index]
    edge_index = torch.tensor(edge_index, dtype=torch.long).t().contiguous()

    num_nodes = len(node_list)
    # 使用嵌入器为每个节点生成嵌入
    x = torch.tensor([embedder.embed(node) for node in node_list], dtype=torch.float)

    # 示例标签
    y = torch.tensor([0 for _ in range(num_nodes)], dtype=torch.long)

    data = Data(x=x, edge_index=edge_index, y=y)
    return data, node_list
