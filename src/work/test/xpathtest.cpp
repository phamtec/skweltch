#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include <stdio.h>
#include <boost/algorithm/hex.hpp>
#include <turtle/mock.hpp>
#include <pugixml.hpp>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( xpathTest )
{

	string xml = "<book id=\"bk101\">\n"
"      <author>Gambardella, Matthew</author>\n"
"      <title>XML Developer's Guide</title>\n"
"   </book>";

	string xpath = "book/title/text()";
	
	pugi::xml_document doc;
	doc.load_buffer(xml.c_str(), xml.length());
	pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
	pugi::xpath_node_set::const_iterator i=nodes.begin();
	BOOST_CHECK(i != nodes.end());
	BOOST_CHECK(i->node().type() != pugi::node_null);
    BOOST_CHECK(i->node().value() == string("XML Developer's Guide"));
    i++;
	BOOST_CHECK(i == nodes.end());
	
}

BOOST_AUTO_TEST_CASE( listTest )
{

	string xml = "<Topic r:id=\"Top/Arts/Animation\">\n"
"	<catid>423945</catid>\n"
"	<link1 r:resource=\"http://www.awn.com/\"></link1>\n"
"	<link r:resource=\"http://animation.about.com/\"></link>\n"
"	<link r:resource=\"http://www.toonhound.com/\"></link>\n"
"	<link r:resource=\"http://enculturation.gmu.edu/2_1/pisters.html\"></link>\n"
"	<link r:resource=\"http://www.digitalmediafx.com/Features/animationhistory.html\"></link>\n"
"	<link r:resource=\"http://www-viz.tamu.edu/courses/viza615/97spring/pjames/history/main.html\"></link>\n"
"	<link r:resource=\"http://www.spark-online.com/august00/media/romano.html\"></link>\n"
"	<link r:resource=\"http://www.animated-divots.net/\"></link>\n"
"</Topic>\n";

	string xpath = "Topic/*[starts-with(name(), 'link')]/@r:resource";

	pugi::xml_document doc;
	doc.load_buffer(xml.c_str(), xml.length());
	pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
	pugi::xpath_node_set::const_iterator i=nodes.begin();
	BOOST_CHECK(i != nodes.end());
	BOOST_CHECK(i->node().type() == pugi::node_null);
    BOOST_CHECK(i->attribute().value() == string("http://www.awn.com/"));
    i++;
	BOOST_CHECK(i != nodes.end());
    BOOST_CHECK(i->attribute().value() == string("http://animation.about.com/"));
    i++;
	
}
