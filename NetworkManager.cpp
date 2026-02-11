#include "NetworkManager.h"
#include "EventNetwork.h"
#include "WorldManager.h"

// Linux/Mac specific support for sockets.

  // Header files.
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

using namespace df;

//constructor
NetworkManager::NetworkManager(){
    setType("NetworkManager");
    m_accept_sock = -1;
}

// Get the one and only instance of the NetworkManager.
NetworkManager &NetworkManager::getInstance(){
    static NetworkManager instance;
    return instance;
}

 // Start up NetworkManager.
int NetworkManager::startUp(){
    m_accept_sock = -1;
    m_sock.clear();
    return Manager::startUp();
}
// Shut down NetworkManager.
void NetworkManager::shutDown(){
    setServer(false);
    closeAll();
    Manager::shutDown();
}



// Setup NetworkManager as server (if false, reset to client).
// Return 0 if ok, else -1.
int NetworkManager::setServer(bool server, std::string port){
    if(!server){
        //turn off server
        if (m_accept_sock != -1){
            ::close(m_accept_sock);
            m_accept_sock = -1;
        }
        return 0;
    }

    //for Turn on server
    struct addrinfo request, *results;
    memset(&request, 0, sizeof(request));
    request.ai_family = AF_INET; //ip4
    request.ai_socktype = SOCK_STREAM; //tcp
    request.ai_flags = AI_PASSIVE; //bind local ip

    //resolve address
    if(getaddrinfo(nullptr, port.c_str(), &request,&results) != -0){
        return -1;
    }

    //create socket
    m_accept_sock = socket(results -> ai_family, results -> ai_socktype, results-> ai_protocol);
    if(m_accept_sock == INVALID_SOCKET){
        freeaddrinfo(results);
        return -1;
    }
    //bind socket to port, check return value of bind
    if(bind(m_accept_sock, results -> ai_addr, (int)results -> ai_addrlen) == SOCKET_ERROR){
        ::close(m_accept_sock);
        freeaddrinfo(results);
        return -1;
    }
    freeaddrinfo(results);
    //listening
    if(listen(m_accept_sock, SOMAXCONN) == SOCKET_ERROR){
        ::close(m_accept_sock);
        return -1;
    }
    //non-blocking
    fcntl(m_accept_sock, F_SETFL, O_NONBLOCK);
    return 0;
}
// Accept network connection.
// If successful, generate EventNetwork (accept).
// Return sock index if new connection (note, 0 for first).
// Return -1 if no new connection, but no error.
// Return -2 if error.
int NetworkManager::accept(){
    if(m_accept_sock == -1){
        return -1;
    }
    //call accept
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = ::accept(m_accept_sock, (struct sockaddr *)&client_addr, &client_len);
    //check for new connection
    if(client_sock != INVALID_SOCKET){
        fcntl(client_sock, F_SETFL, O_NONBLOCK);
        //add socket
        m_sock.push_back(client_sock);
        //generate event network
        EventNetwork en(NetworkEventLabel::ACCEPT);
        en.setSocketIndex(m_sock.size()-1);
        WM.onEvent(&en);
        return en.getSocketIndex();
    }
    return -1;
}

// Make network connection to host at port.
// If successful, generate EventNetwork (connect).
// Return socket index if success, -1 if error.
int NetworkManager::connect(std::string host, std::string port){
    //setup structure for getadr
    struct addrinfo request, *results;
    memset(&request, 0, sizeof(request));
    request.ai_family = AF_INET; //ip4
    request.ai_socktype = SOCK_STREAM; //tcp

    //lockup host name
    if(getaddrinfo(host.c_str(), port.c_str(), &request, &results) != 0){
        return -1;
    }

    //create socket
    int sock = socket(results -> ai_family, results -> ai_socktype, results-> ai_protocol);
    if(sock == INVALID_SOCKET){
        freeaddrinfo(results);
        return -1;
    }

    //connect to server
    if(::connect(sock, results->ai_addr, (int)results->ai_addrlen) == SOCKET_ERROR){
        ::close(sock);
        freeaddrinfo(results);
        return -1;
    }   

    //free address
    freeaddrinfo(results);

    fcntl(sock, F_SETFL, O_NONBLOCK);

    //add socket to vector
    m_sock.push_back(sock);
    //EventNetwortk
    EventNetwork en(NetworkEventLabel::CONNECT);
    en.setSocketIndex(m_sock.size() - 1);
    WM.onEvent(&en);
    return m_sock.size() - 1;    
}

// Send bytes from buffer to connected network socket index.
// Return 0 if success, -1 if error.
int NetworkManager::send(const void *buffer, int bytes, int sock_index){
    //bounds check, to make sure not crashing or make sure no error
    if (sock_index < 0 || sock_index >= m_sock.size()) return -1;
    //send data through connect 
    int data = ::send(m_sock[sock_index], (const char*)buffer, bytes, 0);

    if(data == SOCKET_ERROR){
        return -1;
    }
    return 0;
}
// Receive from connected network socket index (no more than nbytes).
// If leave is true, don't remove data from socket (peek).
// Return number of bytes received, -1 if error.
int NetworkManager::receive(void *buffer, int nbytes, bool leave, int sock_index){
    if (sock_index < 0 || sock_index >= m_sock.size()) return -1;
    //determine flag
    int flag = 0;
    if(leave){
        flag = MSG_PEEK;
    }
    //retrieve data
    int data = ::recv(m_sock[sock_index], (char*)buffer, nbytes, flag);
    // If data returns 0, the connection has been closed by the other side.
    // Call close(sock_index) and return 0.
    //means no data
    if(data == SOCKET_ERROR){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return -1;
    }
    //disconnected
    if(data == 0){
        close(sock_index);
        return 0;
    }
    return data;
}
// Close network connection on indicated socket index.
// If successful, generate EventNetwork (close).
// Return 0 if success, else -1.
int NetworkManager::close(int sock_index){
    if (sock_index < 0 || sock_index >= m_sock.size()) return -1;
    //get socket
    int sock = m_sock[sock_index];

    //close socket
    ::close(sock);
    
    //EventNetwork 
    EventNetwork en(NetworkEventLabel::CLOSE);
    en.setSocketIndex(sock_index);
    WM.onEvent(&en);

    //remove socket
    m_sock.erase(m_sock.begin() + sock_index);
    return 0;
}

// Close all network connections.
// If successful, generate EventNetwork (close), for each.
// Return 0 if success, else negative number.
int NetworkManager::closeAll(){
    for(int x = m_sock.size()-1; x >= 0; x--){
        close(x);
    }
    return 0;
}

// Return true if socket index is connected, else false.
bool NetworkManager::isConnected(int sock_index) const{
    if(sock_index >= 0 && sock_index < m_sock.size()){
        return true;
    }
    return false;
}

// Check if network data on indicated socket index.
// Return amount of data (0 if no data), -1 if not connected or error.
int NetworkManager::isData(int sock_index) const{
    if (sock_index < 0 || sock_index >= m_sock.size()) return -1;
    int bytes = 0;
    if(ioctl(m_sock[sock_index], FIONREAD, &bytes) == -1){
        return -1;
    }
    return bytes;
}

// Return system socket from socket index, -1 if error.
int NetworkManager::getSocket(int sock_index) const{
    if (sock_index < 0 || sock_index >= m_sock.size()) return -1;
    return m_sock[sock_index];
}

// Return true if successfully setup as server, else false.
bool NetworkManager::isServer() const{
    return m_accept_sock != -1;
}

