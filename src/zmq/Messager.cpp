#include "Messager.hpp"
#include <iostream>
#include <string>

using namespace std;

void ZmQMessager::send(zmq::message_t *message) {

    sender->send(*message);

}

void ZmQMessager::receive(zmq::message_t *message) {

	receiver->recv(message);

}
