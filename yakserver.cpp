/*
yakserver.cpp
accepts connections from yak clients
accepts messages from client
*/

#include "yakserver.h"
#include "NetworkManager.h"
#include "LogManager.h"
#include "EventNetwork.h"

#define yak_port 9876
#define message_size 1024
enum class MessageType{
    UNDEFINED = -1, EXIT, CHAT
};

struct packet{
    int size; // size of message
    MessageType type; //type of message
};

using namespace df;
//constructor
yakserver::yakserver(){
    setType("yakserver");

    //starting network manager
    if(NM.startUp()){
        LM.writeLog("Yak server: error starting network manager");
    }

    //Start Server Listening
    if(NM.setServer(true,yak_port) != 0){
        LM.writeLog("YakServer: Failed to open on", yak_port);
    }
    else{
        LM.writeLog("yak server: good open on", yak_port);
    }

    //register for events
    registerInterest(df::NETWORK_EVENT);
}
//deconstructor
yakserver::~yakserver(){
    NM.closeAll();
    NM.shutDown();
}
//handle network event
int yakserver::eventHandler(const df::Event event_pointer){
    if(events->getType() == df::NETWORK_EVENT){
        const EventNetwork *network_event_pointer = <const EventNetwork*>(event_pointer);

        switch(network_event_pointer->getLabel()){
            case NetworkEventLabel::ACCEPT:
                LM.writeLog("YakServer: connected", network_event_pointer->getSocketIndex());
                break;
            case NetworkEventLabel::DATA:
                handleMessage(network_event_pointer->getSocketIndex());
                break;
            case NetworkEventLabel::CLOSE:
                LM.writeLog("yakServer: client disconnected");
                break;
            default:break;
        }
        return 1;
    }
    return 0;
}
//extract message and send to client
void yakserver::handleMessage(int index){
    //read header
    PacketHeader header;
    
    int bytes = NM.recieve(&header, sizeof(header), false, index);

    if(bytes != sizeof(header)) return;

    //read data
    int length = header.size - sizeof(PacketHeader);
    char buffer[message_size];
    memset(buffer, 0, message_size);

    if(data_len > 0){
        NM.recieve(buffer, len, false, index);
    }

    //broadcast message
    if(header.type == MessageType::CHAT){
        LM.writeLog("YakServer: Broadcast");

        //send to every other client
        int count = NM.getNumConnections();
        for(int x = 0; x < count; x++){
            if(x != index && NM.isConnected(x)){
                NM.send(&header,sizeof(header),x);
                NM.send(buffer,len,x);
            }
        }
    }
    else if(header.type == MessageType::EXIT){
        NM.close(index);
    }
}

