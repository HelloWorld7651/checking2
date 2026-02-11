//
  // EventNetwork.h - A network event.
  //

  #ifndef EVENT_NETWORK_H
  #define EVENT_NETWORK_H

  // System includes.
  #include <string>

  // Engine includes.
  #include "Event.h"

  namespace df {

  const std::string NETWORK_EVENT = "df-network";

  enum class NetworkEventLabel {
    UNDEFINED = -1,
    ACCEPT,
    CONNECT,
    CLOSE,
    DATA,
  };  

  class EventNetwork : public Event {

   private:
    EventNetwork();            // Must provide label in constructor.
    NetworkEventLabel m_label; // Label of event.
    int m_socket_index;        // Socket index of connection.
    int m_bytes;               // Number of bytes in message (0 if connect).

   public:
    // Constructor must provide label.  
    EventNetwork(NetworkEventLabel label);

    // Set label.
    void setLabel(NetworkEventLabel new_label);

    // Get label.
    NetworkEventLabel getLabel() const;

    // Set socket index.
    void setSocketIndex(int new_socket_index);

    // Get socket index.
    int getSocketIndex() const;

    // Set number of bytes in message.
    void setBytes(int new_bytes);

    // Get number of bytes in message.
    int getBytes() const;
  };

  } // end of namespace df

  #endif // EVENT_NETWORK_H