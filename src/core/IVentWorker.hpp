
#ifndef __IVENTWORKER_HPP_INCLUDED__
#define __IVENTWORKER_HPP_INCLUDED__

class Vent;

/**
	An interface to the Vent worker.
*/

class IVentWorker {

public:

	virtual int sendAll(Vent *vent) = 0;
	virtual bool shouldQuit() = 0;
	
};

#endif // __IVENTWORKER_HPP_INCLUDED__
