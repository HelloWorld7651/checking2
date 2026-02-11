/*
yakclient.h
connect to host,get name/handle, get input from user, package it in message and send, recieve data from server, extract and display
*/
#ifndef YAKCLIENT_H
#define YAKCLIENT_H

#include "Object.h"
#include "TextBox.h"


#define yak_port 9876
#define message_size 1024
enum class MessageType{
    UNDEFINED = -1, EXIT, CHAT
};

class yakclient:public df::Object {
    private:
    df::TextBox *text_box; //ui box
    void handleMessage(); // read from server

    public:
    yakclient(df::TextBox *box); // constructor for text box
    ~yakclient();
    int eventHandler(const df::Event *event) override;
};

#endif