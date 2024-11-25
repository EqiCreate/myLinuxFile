from rdflib import Graph

# 加载 RDF 数据
g = Graph()
g.parse("input.rdf", format="turtle")

# 提取动词（谓词）和多矢量值
extracted_data = {}
for subj, pred, obj in g:
    key = pred.split("/")[-1]  # 提取谓词的名称
    value = extracted_data.get(key, [])
    value.append(obj)
    extracted_data[key] = value

# 打印提取结果
for key, values in extracted_data.items():
    print(f"Key (Predicate): {key}, Value (Multi-Vectors): {values}")

# 构建训练数据
training_data = []
for key, values in extracted_data.items():
    training_data.append({
        "verb": key,
        "multi_vectors": values
    })

# 示例打印
print("Training Data:")
for data in training_data:
    print(data)
