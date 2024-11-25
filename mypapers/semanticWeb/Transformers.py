from transformers import pipeline

# 使用 Hugging Face 提取关系
extractor = pipeline("zero-shot-classification", model="facebook/bart-large-mnli")

# 输入文本和候选关系
text = "Steve Jobs founded Apple Inc. in Cupertino."
labels = ["founder", "location", "date"]

result = extractor(text, labels)
print("\nZero-shot Classification Results:")
print(result)
