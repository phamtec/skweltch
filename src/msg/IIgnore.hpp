
#ifndef __IIGNORE_HPP_INCLUDED__
#define __IIGNORE_HPP_INCLUDED__

class IDataMsg;

/*
	Abstract interface for working out if you can ignore a message.
*/

class IIgnore {
public:

	virtual bool canIgnore(IDataMsg *msg) = 0;
	
};

#endif // __IIGNORE_HPP_INCLUDED__

