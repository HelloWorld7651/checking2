/*
yakserver.h
accepts connections from yak clients
accepts messages from client
*/

#ifndef YAKSERVER_H
#define YAKSERVER_H

#include "Object.h"

class yakserver: public df::Object{
    private:
    //extract message and send to client
    void handleMessage(int index);

    public:
    //constructor
    yakserver();
    //deconstructor
    ~yakserver();

    //handle network event
    int eventHandler(const df::Event *event) override;
};



#endif