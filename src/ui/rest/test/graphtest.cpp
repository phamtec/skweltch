#define BOOST_TEST_MODULE graph-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "../GraphLayout.hpp"

using namespace std;
using namespace boost;
        
BOOST_AUTO_TEST_CASE( graphTest )
{

	GraphLayout l;
	l.addVertexes(4);
	l.addEdge(0, 1);
	l.addEdge(0, 2);
	l.addEdge(1, 2);
	l.addEdge(0, 3);
	vector<pair<int, int> > centers;
	l.layout(&centers, 100, 100, 100);
	BOOST_CHECK(centers.size() == 4);
	
}
