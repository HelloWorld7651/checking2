// 
  // NetworkManager.h
  //

  #ifndef NETWORK_MANAGER_H
  #define NETWORK_MANAGER_H

  // System includes.
  #include <string>
  #include <vector>

  // Engine includes.
  #include "Manager.h"

  // Two-letter acronym for easier access to manager.
  #define NM df::NetworkManager::getInstance()

  namespace df {

  const std::string DRAGONFLY_PORT = "9876";

  class NetworkManager : public Manager {

   private:
    NetworkManager();                       // Private since a singleton.
    NetworkManager(NetworkManager const&);  // Don't allow copy.
    void operator=(NetworkManager const&);  // Don't allow assignment.
    std::vector<int> m_sock;                // Connected network sockets.
    int m_accept_sock;                      // Socket for accept connections.

   public:

    // Get the one and only instance of the NetworkManager.
    static NetworkManager &getInstance();

    // Start up NetworkManager.
    int startUp() override;

    // Shut down NetworkManager.
    void shutDown() override;

    // Setup NetworkManager as server (if false, reset to client).
    // Return 0 if ok, else -1.
    int setServer(bool server = true, std::string port = DRAGONFLY_PORT);

    // Return true if successfully setup as server, else false.
    bool isServer() const;

    // Accept network connection.
    // If successful, generate EventNetwork (accept).
    // Return sock index if new connection (note, 0 for first).
    // Return -1 if no new connection, but no error.
    // Return -2 if error.
    int accept();

    // Make network connection to host at port.
    // If successful, generate EventNetwork (connect).
    // Return socket index if success, -1 if error.
    int connect(std::string host, std::string port = DRAGONFLY_PORT);

    // Get number of connected sockets.
    int getNumConnections() const;

    // Send bytes from buffer to connected network socket index.
    // Return 0 if success, -1 if error.
    int send(const void *buffer, int bytes, int sock_index=0);

    // Receive from connected network socket index (no more than nbytes).
    // If leave is true, don't remove data from socket (peek).
    // Return number of bytes received, -1 if error.
    int receive(void *buffer, int nbytes, bool leave, int sock_index=0);

    // Close network connection on indicated socket index.
    // If successful, generate EventNetwork (close).
    // Return 0 if success, else -1.
    int close(int sock_index=0);

    // Close all network connections.
    // If successful, generate EventNetwork (close), for each.
    // Return 0 if success, else negative number.
    int closeAll();

    // Return true if socket index is connected, else false.
    bool isConnected(int sock_index=0) const;

    // Check if network data on indicated socket index.
    // Return amount of data (0 if no data), -1 if not connected or error.
    int isData(int sock_index=0) const;

    // Return system socket from socket index, -1 if error.
    int getSocket(int sock_index=0) const;
  };

  } // end of namespace df

  #endif // NETWORK_MANAGER_H
