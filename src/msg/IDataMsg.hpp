
#ifndef __IDATAMSG_HPP_INCLUDED__
#define __IDATAMSG_HPP_INCLUDED__

/**
	Abstract message that contains data. They all have an id.
*/

class IDataMsg {

public:
	
	// getters.
	virtual int getId() = 0;
	
};

#endif // __IDATAMSG_HPP_INCLUDED__
