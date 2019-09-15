# Generic-Directed-Weighted-Graph
Generic Directed Weighted Graph (GDWG) with value-like semantics in C++

   The data stored at a node and the weight stored at an edge will be of generic types. Both generic types may be different. 
   For example, here is a graph with nodes storing std::string and edges weighted by int:
   gdwg::Graph<std::string,int> g;
   Formally, this directed weighted graph G=(N,E) consist of a set of nodes N and a set of weighted edges E.
