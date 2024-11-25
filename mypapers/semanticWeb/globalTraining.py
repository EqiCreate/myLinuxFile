from transformers import AutoTokenizer, AutoModel

# 加载预训练模型
tokenizer = AutoTokenizer.from_pretrained("bert-base-uncased")
model = AutoModel.from_pretrained("bert-base-uncased")

# 编码动词和多矢量
for data in training_data:
    inputs = tokenizer(data["verb"] + " " + " ".join(data["multi_vectors"]), return_tensors="pt")
    outputs = model(**inputs)
    embeddings = outputs.last_hidden_state.mean(dim=1)  # 平均池化得到句子嵌入

    print(f"Embedding for {data['verb']}: {embeddings}")
