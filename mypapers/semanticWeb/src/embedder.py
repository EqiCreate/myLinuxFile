# embedder.py
from transformers import BertModel, BertTokenizer
import torch

class Embedder:
    def __init__(self, model_name='bert-base-uncased'):
        self.tokenizer = BertTokenizer.from_pretrained(model_name)
        self.model = BertModel.from_pretrained(model_name)
        self.model.eval()
    
    def embed(self, text):
        with torch.no_grad():
            inputs = self.tokenizer(text, return_tensors='pt', truncation=True, padding=True)
            outputs = self.model(**inputs)
            # 使用[CLS]标记的输出作为嵌入
            cls_embedding = outputs.last_hidden_state[:, 0, :].squeeze().numpy()
        return cls_embedding
