
#ifndef __GRAPHLAYOUT_HPP_INCLUDED__
#define __GRAPHLAYOUT_HPP_INCLUDED__

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>

typedef boost::square_topology<>::point_type Point;
struct VertexProperties
{
	std::size_t index;
	Point point;
};

struct EdgeProperty
{
	EdgeProperty(const std::size_t &w): weight(w) {}
	double weight;
};

typedef boost::adjacency_list<boost::listS,
	boost::listS, boost::undirectedS,
	VertexProperties, EdgeProperty> Graph;

typedef boost::property_map<Graph, std::size_t VertexProperties::*>::type VertexIndexPropertyMap;
typedef boost::property_map<Graph, Point VertexProperties::*>::type PositionMap;
typedef boost::property_map<Graph, double EdgeProperty::*>::type WeightPropertyMap;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;        

class GraphLayout {

private:
	Graph g;
	VertexIndexPropertyMap vertexIdPropertyMap;
	
public:
	GraphLayout();
	
	void addVertexes(int count);
	void addEdge(int from, int to);
	void layout(std::vector<std::pair<int, int> > *centers, int size, int centerx, int centery);
	
};

#endif // __GRAPHLAYOUT_HPP_INCLUDED__
