#define BOOST_TEST_MODULE xpath-tests
#include <boost/test/unit_test.hpp>

#include "../XPathSplitter.hpp"
#include "../XmlParser.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

using namespace std;
using namespace boost;

struct SetupLogging
{
    SetupLogging() {
		log4cxx::BasicConfigurator::configure();
    }
};

BOOST_AUTO_TEST_SUITE( xpathTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_chunk, IChunk )
{
	MOCK_METHOD( chunk, 1 )
};

MOCK_BASE_CLASS( mock_elem, IXmlElement )
{
	MOCK_METHOD( pi, 1 )
	MOCK_METHOD( startTag, 1 )
	MOCK_METHOD( attr, 1 )
	MOCK_METHOD( endAttr, 0 )
	MOCK_METHOD( endTag, 1 )
	MOCK_METHOD( content, 1 )
};

BOOST_AUTO_TEST_CASE( simplestXmlTest )
{
	string xml = "<book>A book</book>";

	istringstream ss(xml);
	mock_elem e;

	MOCK_EXPECT(e.startTag).exactly(1).with("book").returns(true);
	MOCK_EXPECT(e.endTag).exactly(1).with("book").returns(true);
	MOCK_EXPECT(e.content).exactly(1).with("A book").returns(true);
	MOCK_EXPECT(e.endAttr).returns(true);
	
	XmlParser p(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(p.process(&e));

}

BOOST_AUTO_TEST_CASE( simpleTest )
{
	string xml = "<book>A book</book>";

	istringstream ss(xml);
	mock_chunk c;

	MOCK_EXPECT(c.chunk).exactly(1).with("<book>A book</book>").returns(true);
	
	XPathSplitter splitter(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(splitter.process("book", &c));

}

BOOST_AUTO_TEST_CASE( nestedTest )
{
	string xml = "<catalog><book>A book</book><book>A book 2</book></catalog>";

	istringstream ss(xml);
	mock_chunk c;

	MOCK_EXPECT(c.chunk).exactly(1).with("<book>A book</book>").returns(true);
	MOCK_EXPECT(c.chunk).exactly(1).with("<book>A book 2</book>").returns(true);
	
	XPathSplitter splitter(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(splitter.process("catalog/book", &c));

}

BOOST_AUTO_TEST_CASE( xmlAttrTest )
{
	string xml = "<book id=\"1\">A book</book>";

	istringstream ss(xml);
	mock_elem e;

	MOCK_EXPECT(e.startTag).exactly(1).with("book").returns(true);
	MOCK_EXPECT(e.attr).exactly(1).with("id=\"1\"").returns(true);
	MOCK_EXPECT(e.endTag).exactly(1).with("book").returns(true);
	MOCK_EXPECT(e.content).exactly(1).with("A book").returns(true);
	MOCK_EXPECT(e.endAttr).returns(true);
	
	XmlParser p(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(p.process(&e));

}

BOOST_AUTO_TEST_CASE( nestedWithAttrTest )
{
	string xml = "<catalog><book id=\"1\">A book</book><book id=\"2\">A book 2</book></catalog>";

	istringstream ss(xml);
	mock_chunk c;

	MOCK_EXPECT(c.chunk).exactly(1).with("<book id=\"1\">A book</book>").returns(true);
	MOCK_EXPECT(c.chunk).exactly(1).with("<book id=\"2\">A book 2</book>").returns(true);
	
	XPathSplitter splitter(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(splitter.process("catalog/book", &c));

}

BOOST_AUTO_TEST_CASE( piTest )
{
	string xml = "<?xml version=\"1.0\"?>\n"
		"<catalog>\n"
		"</catalog>\n";

	istringstream ss(xml);
	mock_elem e;

	MOCK_EXPECT(e.pi).exactly(1).with("xml version=\"1.0\"?").returns(true);
	MOCK_EXPECT(e.startTag).exactly(1).with("catalog").returns(true);
	MOCK_EXPECT(e.endTag).exactly(1).with("catalog").returns(true);
	MOCK_EXPECT(e.content).exactly(2).with("\n").returns(true);
	MOCK_EXPECT(e.endAttr).returns(true);
	
	XmlParser p(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(p.process(&e));

}

BOOST_AUTO_TEST_CASE( xmlBooksTest )
{
	string xml = "<?xml version=\"1.0\"?>\n"
"<catalog>\n"
"   <book id=\"bk101\">\n"
"      <author>Gambardella, Matthew</author>\n"
"      <title>XML Developer's Guide</title>\n"
"   </book>\n"
"   <book id=\"bk102\">\n"
"      <author>Ralls, Kim</author>\n"
"      <title>Midnight Rain</title>\n"
"   </book>\n"
"</catalog>\n";

	istringstream ss(xml);
	mock_elem e;

	MOCK_EXPECT(e.pi).exactly(1).with(mock::any).returns(true);
	MOCK_EXPECT(e.startTag).exactly(1).with("catalog").returns(true);
	MOCK_EXPECT(e.startTag).exactly(2).with("book").returns(true);
	MOCK_EXPECT(e.startTag).exactly(2).with("author").returns(true);
	MOCK_EXPECT(e.startTag).exactly(2).with("title").returns(true);
	MOCK_EXPECT(e.endTag).exactly(1).with("catalog").returns(true);
	MOCK_EXPECT(e.endTag).exactly(2).with("book").returns(true);
	MOCK_EXPECT(e.endTag).exactly(2).with("author").returns(true);
	MOCK_EXPECT(e.endTag).exactly(2).with("title").returns(true);
	MOCK_EXPECT(e.attr).exactly(2).with(mock::any).returns(true);
	MOCK_EXPECT(e.content).with(mock::any).returns(true);
	MOCK_EXPECT(e.endAttr).returns(true);
		
	XmlParser p(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(p.process(&e));

}

BOOST_AUTO_TEST_CASE( booksTest )
{
	string book1 = "<book id=\"bk101\">\n"
"      <author>Gambardella, Matthew</author>\n"
"      <title>XML Developer's Guide</title>\n"
"   </book>";
	string book2 = "<book id=\"bk102\">\n"
"      <author>Ralls, Kim</author>\n"
"      <title>Midnight Rain</title>\n"
"   </book>";

	string xml = "<?xml version=\"1.0\"?>\n"
"<catalog>\n"
"   " + book1 + "\n" +
"   " + book2 + "\n" +
"</catalog>\n";

	istringstream ss(xml);
	mock_chunk c;

	MOCK_EXPECT(c.chunk).exactly(1).with(book1).returns(true);
	MOCK_EXPECT(c.chunk).exactly(1).with(book2).returns(true);
	
	XPathSplitter splitter(log4cxx::Logger::getRootLogger(), &ss);
	BOOST_CHECK(splitter.process("catalog/book", &c));

}

BOOST_AUTO_TEST_SUITE_END()
