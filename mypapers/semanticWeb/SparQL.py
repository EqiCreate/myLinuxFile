from rdflib import Graph

# 加载 RDF 数据
g = Graph()
g.parse("output.rdf", format="turtle")

# 使用 SPARQL 查询
query = """
    SELECT ?s ?p ?o
    WHERE {
        ?s ?p ?o.
    }
"""
print("\nSPARQL Query Results:")
for row in g.query(query):
    print(row)
