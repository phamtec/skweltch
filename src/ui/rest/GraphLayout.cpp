
#include "GraphLayout.hpp"

#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topology.hpp>

using namespace std;
using namespace boost;

GraphLayout::GraphLayout() {

	vertexIdPropertyMap = get(&VertexProperties::index, g);
	
}

void GraphLayout::addVertexes(int count) {

	for (int i=0; i<count; i++) {
		vertexIdPropertyMap[add_vertex(g)] = i;
	}
	
}

void GraphLayout::addEdge(int from, int to) {

	add_edge(vertex(from, g), vertex(to, g), EdgeProperty(10), g);

}

void GraphLayout::layout(vector<pair<int, int> > *centers, int size, int centerx, int centery) {

	// Create property maps for vertex position and for edge weight
	PositionMap positionMap = get(&VertexProperties::point, g);
	WeightPropertyMap weightPropertyMap = get(&EdgeProperty::weight, g);

	// apply Boost's circle layout
	circle_graph_layout(g, positionMap, 100);

	// apply Boost's kamada layout
	if (!kamada_kawai_spring_layout(g, positionMap, weightPropertyMap, 
											square_topology<>(), side_length<double>(size),     
											layout_tolerance<>(), 1, vertexIdPropertyMap)) {
		throw runtime_error("layout failed.");
	}
											
	graph_traits<Graph>::vertex_iterator i, end;
	for (tie(i, end) = vertices(g); i != end; ++i) {
		centers->push_back(pair<int, int>((int)(positionMap[*i][0]) + centerx, (int)(positionMap[*i][1]) + centery));
	}

}

