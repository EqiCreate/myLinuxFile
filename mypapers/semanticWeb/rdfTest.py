from rdflib import Graph, URIRef, Literal, Namespace

# 创建 RDF 图
g = Graph()
namespace = Namespace("http://example.org/")

# 定义实体和关系
subject = URIRef(namespace["Steve_Jobs"])
predicate = URIRef(namespace["founded"])
obj = URIRef(namespace["Apple_Inc"])

# 添加三元组
g.add((subject, predicate, obj))

# 保存到文件
g.serialize("output.rdf", format="turtle")
print("\nRDF Saved as output.rdf")
