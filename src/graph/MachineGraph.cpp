
#include "MachineGraph.hpp"

#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "JsonPath.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

bool MachineGraph::makeDOT(istream *json) {

	*dot << "digraph G {\n"
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
"	];\n";

	JsonObject m;
	if (!m.read(logger, json)) {
		LOG4CXX_ERROR(logger, "no graph created.")
		return false;
	}
	
	{
		JsonObject vent = m.getChild("vent");
		writeNode(vent);
	}
	{
		JsonObject sink = m.getChild("sink");
		writeNode(sink);
	}
	{
		JsonArray bg = m.getArray("background");
		for (JsonArray::iterator i=bg.begin(); i != bg.end(); i++) {
			writeNode(bg.getValue(i));
		}
	}
	{
		JsonObject reap = m.getChild("reap");
		writeNode(reap);
	}
	
	*dot << "}";

	return true;
}

void MachineGraph::writeTableHeader() {

	*dot << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\">\n";
	
}

void MachineGraph::writeNode(const JsonObject &node) {

	string name = node.getString("name");
	
	*dot << "	" << name << " [shape=box,label=<\n";
	writeTableHeader();

	*dot << "	<TR>\n"
"		<TD><FONT POINT-SIZE=\"18\" COLOR=\"green\">" << name  << "</FONT></TD>\n"
"	</TR>\n";

	writeNodeInfo(node);
	writeNodeConfig(node);
	
	*dot << "</TABLE>>];\n";
	
	JsonObject confconn = node.getChild("connections");
	if (!confconn.empty()) {
		for (JsonObject::iterator i = confconn.begin(); i != confconn.end(); i++) {
			JsonObject o = confconn.getValue(i);
			if (o.getString("mode") == "connect") {
				if (o.getString("direction") == "to") {
					writeEdge(name, o.getString("block"), o.getString("connection"), confconn.getKey(i), false);
				}
				else {
					writeEdge(o.getString("block"), name, confconn.getKey(i), o.getString("connection"), false);
				}
			}
		}
	}

}

void MachineGraph::writeNodeInfo(const JsonObject &node) {

	*dot << "	<TR>\n"
"		<TD>\n";

	bool any = false;
	// the count can be missing.
	int count = node.getInt("count", -1);
	if (count > 0) {
		any = true;
		*dot << "			";
		writeTableHeader();
		writeNodeInfoRow("count", lexical_cast<string>(count));
	}
	
	// the node can be missing.
	string s;
	try {
		s = node.getString("node");
	}
	catch (std::runtime_error &e) {
		s = "";
	}
	if (!s.empty()) {
		if (!any) {
			any = true;
			*dot << "			";
			writeTableHeader();
		}
		writeNodeInfoRow("node", s);
	}
	
	JsonObject confconn = node.getChild("connections");
	if (!confconn.empty()) {
		for (JsonObject::iterator i = confconn.begin(); i != confconn.end(); i++) {
			if (!any) {
				any = true;
				*dot << "			";
				writeTableHeader();
			}
			JsonObject o = confconn.getValue(i);
			if (o.getString("mode") == "connect") {
				writeNodeInfoRow(confconn.getKey(i), "connect");
			}
			else {
				stringstream s;
				s << "bind:" << o.getString("address") << ":" << o.getInt("port", -1); 
				writeNodeInfoRow(confconn.getKey(i), s.str());
			}
		}
	}
	if (any) {
		*dot << "			</TABLE>\n";
	}
	*dot << "		</TD>\n"
"	</TR>\n";

}

void MachineGraph::writeNodeConfig(const JsonObject &node) {

	*dot << "	<TR>\n"
"		<TD>\n";
	JsonObject config = node.getChild("config");
	if (!config.empty()) {
		bool any = false;
		for (JsonObject::iterator i = config.begin(); i != config.end(); i++) {
			if (!any) {
				*dot << "			";
				writeTableHeader();
				any = true;
			}
			writeNodeConfigRow(config.getKey(i), config.getValueString(i));
		}
		if (any) {
			*dot << "			</TABLE>\n";
		}
	}
	*dot << "		</TD>\n"
"	</TR>\n";

}

void MachineGraph::writeEdge(const std::string &from, const std::string &to, 
	const std::string &fromlabel, const std::string &tolabel, bool samehead) {

	*dot << "	" << from << " -> " << to << "[headlabel=\"" << fromlabel << "\",taillabel=\"" << tolabel << "\"";
	if (samehead) {
		*dot << ",samehead=true";
	}
	*dot << "];\n";

}

void MachineGraph::writeNodeInfoRow(const string &name, const string &value) {

	*dot << "				<TR>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">" << name << "</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT POINT-SIZE=\"10\">" << value << "</FONT></TD>\n"
"				</TR>\n";

}

void MachineGraph::writeNodeConfigRow(const string &name, const string &value) {

	*dot << "				<TR>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">" << name << "</FONT></TD>\n"
"					<TD ALIGN=\"left\"><FONT COLOR=\"lightgray\" POINT-SIZE=\"10\">" << value << "</FONT></TD>\n"
"				</TR>\n";

}
