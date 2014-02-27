#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "../MachineGraph.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

struct SetupLogging
{
    SetupLogging() {
		log4cxx::BasicConfigurator::configure();
    }
};

BOOST_AUTO_TEST_SUITE( graphTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

BOOST_AUTO_TEST_CASE( smokeTest )
{
	stringstream json("{\n"
"	\"vent\" : {\n"
"		\"name\": \"VentRandomNum\",\n"
"		\"node\": \"localhost\",\n"
"		\"connections\": {\n"
"			\"pushTo\": {\n"
"				\"direction\": \"to\",\n"
"				\"mode\":\"bind\",\n"
"				\"address\": \"*\",\n"
"				\"port\": 5557\n"
"			},\n"
"			\"syncTo\": {\n"
"				\"direction\": \"to\",\n"
"				\"mode\":\"connect\",\n"
"				\"block\": \"SinkElapsed\",\n"
"				\"connection\": \"pullFrom\"\n"
"			}\n"
"		},\n"
"		\"config\" : {\n"
"			\"count\": 50,\n"
"			\"low\": 1,\n"
"			\"high\": 10,\n"
"			\"sleep\": 0\n"
"		}\n"
"	},\n"
"	\"sink\" : {\n"
"		\"name\": \"SinkElapsed\",\n"
"		\"connections\": {\n"
"		},\n"
"		\"config\" : {\n"
"		}\n"
"	},\n"
"	\"background\": [\n"
"	],\n"
"	\"reap\": {\n"
"		\"name\": \"GrimReaper\",\n"
"		\"config\" : {\n"
"		}\n"
"	}\n"
"}\n");

string result("digraph G {\n"
"	splines=spline;\n"
"	nodesep=2;\n"
"	node [fontcolor=white,\n"
"		fillcolor=black,\n"
"		fontname=Geneva,\n"
"		label=\"\\N\",style=filled\n"
"	];\n"
"	edge [color=blue,\n"
"		fontcolor=black,\n"
"		fontname=Geneva,\n"
"		fontsize=10,\n"
"		fontcolor=lightblue,\n"
"		style=bold\n"
"	];\n"
"	VentRandomNum [shape=box,label=<\n"
"<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n"
"	<TR>\n"
"		<TD><FONT POINT-SIZE=\"18\" COLOR=\"green\">VentRandomNum</FONT></TD>\n"
"	</TR>\n"
"	<TR>\n"
"		<TD>\n"
"			<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n"
"				<TR>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">node</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">localhost</FONT></TD>\n"
"				</TR>\n"
"				<TR>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">pushTo</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">bind:*:5557</FONT></TD>\n"
"				</TR>\n"
"				<TR>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">syncTo</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">connect</FONT></TD>\n"
"				</TR>\n"
"			</TABLE>\n"
"		</TD>\n"
"	</TR>\n"
"	<TR>\n"
"		<TD>\n"
"			<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n"
"				<TR>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">count</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">50</FONT></TD>\n"
"				</TR>\n"
"				<TR>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">low</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">1</FONT></TD>\n"
"				</TR>\n"
"				<TR>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">high</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">10</FONT></TD>\n"
"				</TR>\n"
"				<TR>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">sleep</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">0</FONT></TD>\n"
"				</TR>\n"
"			</TABLE>\n"
"		</TD>\n"
"	</TR>\n"
"</TABLE>>];\n"
"	VentRandomNum -> SinkElapsed[headlabel=\"pullFrom\",taillabel=\"syncTo\"];\n"
"	SinkElapsed [shape=box,label=<\n"
"<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n"
"	<TR>\n"
"		<TD><FONT POINT-SIZE=\"18\" COLOR=\"green\">SinkElapsed</FONT></TD>\n"
"	</TR>\n"
"	<TR>\n"
"		<TD>\n"
"		</TD>\n"
"	</TR>\n"
"	<TR>\n"
"		<TD>\n"
"		</TD>\n"
"	</TR>\n"
"</TABLE>>];\n"
"	GrimReaper [shape=box,label=<\n"
"<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n"
"	<TR>\n"
"		<TD><FONT POINT-SIZE=\"18\" COLOR=\"green\">GrimReaper</FONT></TD>\n"
"	</TR>\n"
"	<TR>\n"
"		<TD>\n"
"		</TD>\n"
"	</TR>\n"
"	<TR>\n"
"		<TD>\n"
"		</TD>\n"
"	</TR>\n"
"</TABLE>>];\n"
"}");

	stringstream dot;
	MachineGraph(log4cxx::Logger::getRootLogger(), &dot).makeDOT(&json);
	
//	cout << dot.str() << endl;
	
	BOOST_CHECK(dot.str() == result);
	
}

BOOST_AUTO_TEST_SUITE_END()
