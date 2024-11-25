# knowledge_base.py
import pickle
import os
from sklearn.metrics.pairwise import cosine_similarity
import numpy as np

class KnowledgeBase:
    def __init__(self, embedder, kb_file='knowledge_base.pkl'):
        self.embedder = embedder
        self.kb_file = kb_file
        if os.path.exists(kb_file):
            with open(kb_file, 'rb') as f:
                self.kb = pickle.load(f)
        else:
            self.kb = {}  # 格式: {term: {'embedding': np.array, 'graph': graph_data}}

    def save_kb(self):
        with open(self.kb_file, 'wb') as f:
            pickle.dump(self.kb, f)

    def add_term(self, term, graph):
        embedding = self.embedder.embed(term)
        self.kb[term] = {'embedding': embedding, 'graph': graph}
        self.save_kb()

    def find_similar_terms(self, user_embedding, threshold=0.8):
        terms = list(self.kb.keys())
        embeddings = [self.kb[term]['embedding'] for term in terms]
        if not embeddings:
            return []
        similarities = cosine_similarity([user_embedding], embeddings)[0]
        similar_terms = [terms[i] for i, sim in enumerate(similarities) if sim >= threshold]
        return similar_terms
