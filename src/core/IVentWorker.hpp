
#ifndef __IVENTWORKER_HPP_INCLUDED__
#define __IVENTWORKER_HPP_INCLUDED__

class Vent;

class IVentWorker {

public:

	virtual int sendAll(Vent *vent) = 0;
	virtual bool shouldQuit() = 0;
	
};

#endif // __IVENTWORKER_HPP_INCLUDED__
