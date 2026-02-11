#include "Sentry.h"
#include "NetworkManager.h"
#include "EventNetwork.h"
#include "EventStep.h"
#include "WorldManager.h"

using namespace df;

//constructor
Sentry::Sentry(){
    setType("Sentry");
    setSolidness(df::SPECTRAL);
    registerInterest(df::STEP_EVENT);
}

//event handler
//checks incoming event
int Sentry::eventHandler(const Event *p_e){
    if (p_e->getType() == STEP_EVENT){
        handleStep();
        return 1;
    }
    else{
        return 0;
    }
}

/*HandleStep
checks new connection to see if data arrived
if full message, call EventManager
*/
void Sentry::handleStep(){
    //check is a server
    if(NM.isServer() == true){
        NM.accept();
    }
    //loop through all connected socket
    for(int x = 0; x < NM.getNumConnections(); x++){
        //socket is valid
        if(NM.isConnected(x)){
            //how many bytes, we need 4 bytes 
            int bytes = NM.isData(x);
            if(bytes>=(sizeof(int))){
                int msg_size = 0;
                //get first 4 bytes as data, but don't remove data
                NM.receive(&msg_size,sizeof(int), true, x);
                //check for full message
                if(bytes >= msg_size){
                    EventNetwork networkEvent (NetworkEventLabel :: DATA);
                    networkEvent.setSocketIndex(x);
                    networkEvent.setBytes(msg_size);
                    WM.onEvent(&networkEvent);
                }
            }
        }
    }
}