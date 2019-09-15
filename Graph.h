/**
 * Graph.h
 * Generic Directed Weighted Graph (GDWG): Generic Directed Weighted Graph (GDWG) with value-like semantics in C++.
   The data stored at a node and the weight stored at an edge will be of generic types. Both generic types may be different. 
   For example, here is a graph with nodes storing std::string and edges weighted by int:
   gdwg::Graph<std::string,int> g;
   Formally, this directed weighted graph G=(N,E) consist of a set of nodes N and a set of weighted edges E.
   The Code helps to create a Graph object and  perform various member operation on it.
   Created by Arvind Bahl
 *
 */
#ifndef GRAPH_H_GUARD
#define GRAPH_H_GUARD

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <iterator>
#include <set>
#include <exception>

//namespace
namespace gdwg {

template <typename N, typename E> class Graph {

	private:
		class GEdge;
		class GNode;
		mutable std::map< N, std::shared_ptr<GNode> > node_cont;
		mutable typename std::map<N, std::shared_ptr<GNode>>::iterator customiter = node_cont.begin();

	public:
		//constructor
		Graph() {
		}
		// copy constructor
		Graph(const Graph& cgraph) {
			node_cont = cgraph.node_cont;
		}
		// move constructor
		Graph(Graph&& mgraph)  {
			node_cont = mgraph.node_cont;
			mgraph.node_cont.clear();
		}

		//destructor
		~Graph()
		{
			node_cont.clear();
		};

		//copy Operator=
		Graph& operator=(const Graph &ngraph){

			if (node_cont != ngraph.node_cont) {
				node_cont.erase(node_cont.begin(),node_cont.end());
			}


			for(auto it =ngraph.node_cont.begin(); it != ngraph.node_cont.end(); ++it ){

				node_cont[it->second->valueofNode] = std::make_shared<GNode>(GNode{it->second->valueofNode});

			}

			for (auto tnode : ngraph.node_cont) {
				for (auto tedge : tnode.second->edges) {
					if (auto shptr = tedge->destin.lock()) {
						auto exist = node_cont.find(shptr->valueofNode);
						if (auto shtpr2 = tedge->destin.lock()) {
							auto ndest = node_cont.find(shtpr2->valueofNode);
							node_cont[tnode.second->valueofNode]->edges.insert(std::make_shared<GEdge>(GEdge(exist->second,ndest->second,tedge->valofEdge)));
						}
					}
				}
			}
		return *this;

		}
		//Operator=move
		Graph& operator=(Graph &&othergraph)
		{
			if ((*this).node_cont != othergraph.node_cont) {
				node_cont = othergraph.node_cont;
				othergraph.node_cont.clear();
			}
			return *this;
		}

	private:
		//Node Class
		class GNode {
		public:
			std::set<std::shared_ptr<GEdge>> edges;
			N valueofNode;
			// Node constructor
			GNode() {
			}
			GNode(const N va) : valueofNode{va} {
				}
			// copy constructor for GNode
			GNode(const GNode& from) {
				valueofNode = from.valueofNode; // copy value field
			}
		};

		//Edge Class
		class GEdge {
		public:
			std::weak_ptr<GNode> source;
			std::weak_ptr<GNode> destin;
			E valofEdge;
			// Default constructor
			GEdge() {
			}
			// Copy constructor for GEdge
			GEdge(std::shared_ptr<GNode> src, std::shared_ptr<GNode> dest, E value) {

				source = src;
				destin = dest;
				valofEdge = value;
			}
			//Destructor for GEdge class
			~GEdge(){

				destin.reset();
				source.reset();
			}
		};


public :

	//function to check existence of source and dest node.
	void checkNodes(const N& src, const N& dst) const{
		auto findSNode = node_cont.find(src);
		if (findSNode == node_cont.end()) throw std::runtime_error("Origin does not exist");
		auto findDNode = node_cont.find(dst);
		if (findDNode == node_cont.end())throw std::runtime_error("Dest does not exist");
	}

	// Function to add node.
	bool addNode(const N& input) {
		auto findNode = node_cont.find(input);
		if (findNode == node_cont.end()) {
			std::shared_ptr<GNode> node = std::make_shared<GNode>();
			node->valueofNode = input;
			node_cont[input] = node;
			return true;
		}
		return false;
	}

	//function to add Edge
	bool addEdge(const N& src, const N& dst, const E& w){

		checkNodes(src, dst);
		auto findDNode = node_cont.find(dst);
		auto findSNode = node_cont.find(src);
		for(auto it =findSNode->second->edges.begin(); it!= findSNode->second->edges.end(); ++it){
			if ((*it)->valofEdge ==w){

				auto tempShDest=(*it)->destin.lock();
				if ((tempShDest->valueofNode) ==dst){
					return false;
				}
			}
		}
		std::shared_ptr<GEdge>tempedge = std::make_shared<GEdge>(findSNode->second,
				findDNode->second, w );
		findSNode->second->edges.insert(tempedge);
		return true;
	}

	//function to replace the edge
	bool replace(const N& oldData, const N& newData){

		auto findONode = node_cont.find(oldData);
		if (findONode == node_cont.end()) throw std::runtime_error("Origin does not exist (replace)");

		auto findNNode = node_cont.find(newData);
		if (findNNode != node_cont.end()) return false;

		node_cont[newData] = node_cont[oldData];
		node_cont[newData]->valueofNode = newData;
		node_cont.erase(findONode);
		return true;
	}

	// function to merge replace the node
	void mergeReplace(const N& oldData, const N& newData) {

		checkNodes(oldData, newData);
		auto findONode = node_cont.find(oldData);
		auto findNNode = node_cont.find(newData);

		auto tempend = findONode->second->edges.end();
		auto tempnend = findNNode->second->edges.end();
		for (auto it = findONode->second->edges.begin(); it!= tempend; ++it){
			int i =0;
			auto tempShDest=(*it)->destin.lock();
			auto value_edge = (*it)->valofEdge;
			for(auto itt = findNNode->second->edges.begin(); itt!= tempnend; ++itt){
				if (((*it)->destin.lock() == (*itt)->destin.lock())&& (value_edge == (*itt)->valofEdge )){
					i=1;
					break;
				}
			}
			if (i==1){continue;}
			if (tempShDest->valueofNode ==oldData){
				std::shared_ptr<GEdge>tempedge = std::make_shared<GEdge>(findNNode->second,
						findNNode->second, value_edge );
				findNNode->second->edges.insert(tempedge);
			}
			else{
				std::shared_ptr<GEdge>tempedge = std::make_shared<GEdge>(findNNode->second,
					tempShDest, value_edge );
				findNNode->second->edges.insert(tempedge);}
		}

		for(auto mit = node_cont.begin(); mit!= node_cont.end(); ++mit){

			auto temedgeedn1 = mit->second->edges.end();
			for (auto ittm = mit->second->edges.begin(); ittm != temedgeedn1; ++ittm){

				auto temspointer = (*ittm)->destin.lock();
				if (temspointer->valueofNode == oldData){

					(*ittm)->destin= findNNode->second;
				}

			}

		}

		findONode->second.reset();
		node_cont.erase(findONode);

		auto findmmmNode = node_cont.find(3);
		for(auto imt =findmmmNode->second->edges.begin(); imt!= findmmmNode->second->edges.end(); ++imt){
					auto tempsource = ((*imt)->source).lock();
					auto tempdestin = ((*imt)->destin).lock();
					//std::cout<<tempsource->valueofNode<<(*imt)->valofEdge<<tempdestin->valueofNode<<std::endl;
				}
	}

	// function to delete the node
	void deleteNode(const N& graphnode) noexcept{

		auto findONode = node_cont.find(graphnode);
		if (findONode == node_cont.end()) return;
		findONode->second.reset();
		node_cont.erase(findONode);

	}

	//function to delete the edge
	void deleteEdge(const N& src, const N& dst, const E& w) noexcept{

		auto findONode = node_cont.find(src);
		if (findONode == node_cont.end()) return;

		auto findDNode = node_cont.find(dst);
		if (findDNode == node_cont.end()) return;

		auto tempend = findONode->second->edges.end();
		for(auto it = findONode->second->edges.begin(); it!= tempend; it++ ){

			auto temspointer = (*it)->destin.lock();
			if (temspointer->valueofNode == dst && (*it)->valofEdge ==w){
				findONode->second->edges.erase(it);
			}
		}
		return;
	}

	//function to clear the content of graph
	void clear() noexcept{

		node_cont.clear();
		return;
	}

	//function to find if node exist
	bool isNode(const N& val) const{

		auto findONode = node_cont.find(val);
		if (findONode != node_cont.end()) return true;
		return false;
	}

	//Function to find if the nodes are connected.
	bool isConnected(const N& src, const N& dst) const{

		checkNodes(src, dst);
		auto findONode = node_cont.find(src);
		auto tempend = findONode->second->edges.end();
		for(auto it = findONode->second->edges.begin(); it!= tempend; it++ ){

			auto temspointer = (*it)->destin.lock();
			if (temspointer->valueofNode==dst){
				return true;
			}
		}
		return false;
	}

	//function to print the nodes
	void printNodes() const{

		std::map<int, std::set<N>>edges_count;
		for (auto it = node_cont.begin(); it != node_cont.end(); ++it){
			auto totalnoedges = it->second->edges.size();
			auto findcount = edges_count.find(totalnoedges);
			if (findcount != edges_count.end()){
				findcount->second.insert(it->first);
			}
			else{
				std::set<N> tempset;
				tempset.insert(it->first);
				edges_count[totalnoedges] = tempset;
			}
		}
		for(auto itt = edges_count.begin(); itt != edges_count.end(); ++itt){
			for (auto its = itt->second.begin();  its != itt->second.end(); ++its ){
				std::cout<<*its<<std::endl;
			}
		}
	}

//function to print the edges
	void printEdges(const N& val) const{

		auto findONode = node_cont.find(val);
		if (findONode == node_cont.end()) throw std::runtime_error("Node does not exist (printEdges)");

		std::cout<<"Edges attached to Node "<<findONode->first<<std::endl;

		auto edgepointer = findONode->second->edges;
		if (findONode->second->edges.size() == 0){
			std::cout<<"(null)"<<std::endl;
			return;
		}
		std::map<E, std::set<N>> tempsetmap;
		for(auto it = edgepointer.begin(); it!= edgepointer.end(); ++it){

			auto findvalEdge = tempsetmap.find((*it)->valofEdge);

			if ((*it)->destin.expired() == true){continue;}
			if (findvalEdge!=tempsetmap.end()){
				findvalEdge->second.insert(((*it)->destin).lock()->valueofNode);
			}
			else{
				std::set<N> tempset;
				tempset.insert(((*it)->destin).lock()->valueofNode);
				tempsetmap[(*it)->valofEdge] = tempset;
			}

		}

		for(auto itt = tempsetmap.begin(); itt != tempsetmap.end(); ++itt){
			for (auto its = itt->second.begin();  its != itt->second.end(); ++its ){
				std::cout<<*its<<" "<<itt->first<<std::endl;
			}
		}
	}

	//Iterator begin
	void begin() const{
		customiter = node_cont.begin();
	}
	//Iterator end
	bool end() const{
		if(customiter != node_cont.end()){return false;}
		return true;
	}

	//Iterator next
	void next() const{
		++customiter;
	}

	//Iterator value
	const N& value() const{
		return customiter->first;
	}
};
}

#endif
