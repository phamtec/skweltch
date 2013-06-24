#define BOOST_TEST_MODULE work-tests
#include <boost/test/unit_test.hpp>

#include "../JsonConfig.hpp"
#include "../Work.hpp"
#include "../Messager.hpp"
#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

MOCK_BASE_CLASS( mock_messager, IMessager )
{
	MOCK_METHOD( send, 1 )
	MOCK_METHOD( receive, 1 )
};

MOCK_BASE_CLASS( mock_work, IWorkMsgExecutor )
{
	MOCK_METHOD( init, 0 )
	MOCK_METHOD( doit, 1 )
};

BOOST_AUTO_TEST_CASE( workTest )
{
	mock_messager m;
	mock_work w;
	
	MOCK_EXPECT(m.receive).with(mock::any).exactly(1);
	MOCK_EXPECT(m.send).with(mock::any).exactly(1);
	MOCK_EXPECT(w.init).once();
	MOCK_EXPECT(w.doit).with(mock::any).exactly(1);

    Work s(&m, &w);
    
    stringstream json("{}");
	JsonConfig c(&json);
	JsonNode root;
	c.read(&root);
    
    s.service(&root, false);
    
}
