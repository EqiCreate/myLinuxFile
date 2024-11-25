import spacy

# 加载 SpaCy 的模型
nlp = spacy.load("en_core_web_sm")

# 输入非结构化文本
text = "Apple Inc. was founded by Steve Jobs in Cupertino, California in 1976."

# 分析文本
doc = nlp(text)

# 输出分词和词性标注
print("Tokens and POS tags:")
for token in doc:
    print(f"{token.text} ({token.pos_})")

# 识别命名实体
print("\nNamed Entities:")
for ent in doc.ents:
    print(f"{ent.text} ({ent.label_})")

# 提取三元组
print("Extracted Triples:")
for sent in doc.sents:  # 遍历句子
    root = sent.root  # 谓语动词
    subj = [child.text for child in root.children if child.dep_ in ("nsubj", "nsubjpass")]
    obj = [child.text for child in root.children if child.dep_ in ("dobj", "attr", "prep")]
    if subj and obj:
        print(f"Subject: {', '.join(subj)}, Predicate: {root.text}, Object: {', '.join(obj)}")