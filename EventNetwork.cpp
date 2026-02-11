#include "EventNetwork.h"

using namespace df;

//constructor
EventNetwork:: EventNetwork(NetworkEventLabel label){

    setType(df::NETWORK_EVENT);
    m_label = label;
    m_socket_index = -1;
    m_bytes = 0;
}

//Set Label
void EventNetwork::setLabel(NetworkEventLabel new_label){
    m_label = new_label;
}
// Get label.
NetworkEventLabel EventNetwork::getLabel() const{
    return m_label;
}
//set socket index
void EventNetwork::setSocketIndex(int new_socket_index){
    m_socket_index = new_socket_index;
}
//get socket index
int EventNetwork::getSocketIndex() const{
    return m_socket_index;
}
// set number of bytes in message
void EventNetwork::setBytes(int new_bytes){
    m_bytes = new_bytes;
}
int EventNetwork::getBytes() const{
    return m_bytes;
}
