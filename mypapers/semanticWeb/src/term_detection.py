# term_detection.py
from knowledge_base import KnowledgeBase
from embedder import Embedder

def detect_specialized_terms(user_input, knowledge_base, embedder, similarity_threshold=0.8):
    user_embedding = embedder.embed(user_input)
    similar_terms = knowledge_base.find_similar_terms(user_embedding, threshold=similarity_threshold)
    return similar_terms
