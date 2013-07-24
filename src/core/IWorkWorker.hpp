
#ifndef __IWORKWORKER_HPP_INCLUDED__
#define __IWORKWORKER_HPP_INCLUDED__

#include <msgpack.hpp>

class SinkMsg;

class IWorkWorker {

public:

	virtual void process(msgpack::object *obj, SinkMsg *smsg) = 0;
	virtual bool shouldQuit() = 0;

};

#endif // __IWORKWORKER_HPP_INCLUDED__
