#ifndef HIVE_MIND_BRIDGE_TCPSERVER_H
#define HIVE_MIND_BRIDGE_TCPSERVER_H

#include "hivemind-bridge/ITCPServer.h"
#include <cpp-common/ILogger.h>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

class TCPServer : public ITCPServer {
  public:
    /**
     * Construct a TCPServer
     * @param port Port to open the server to
     * @param logger used to log info of the server
     */
    TCPServer(int port, ILogger& logger);
    ~TCPServer();

    /**
     * Start listening for incoming client connections and accept one client. This is blocking until
     * a client connects.
     */
    void listen();

    /**
     * Read data from socket.
     * @param data The buffer on which the incoming data will be written.
     * @param length The length of the provided buffer.
     * @return The length of the data read.
     */
    bool receive(uint8_t* data, uint16_t length) override;

    /**
     * Send some data to the client.
     * @param data The buffer containing the data to send.
     * @param length The length of the data on the buffer.
     */
    bool send(const uint8_t* data, uint16_t length) override;

    /**
     * Terminate a client connection.
     */
    void close() override;

    /**
     * See if the server has a client connected
     * @return true if the TCP server has a client connected.
     */
    bool isClientConnected() override;

    /**
     * Register a callback to be run when a TCP connection is established with a client HiveMind
     * @param hook The function to be run
     */
    void onConnect(std::function<void()> hook) override;

    /**
     * Register a callback to be run as soon as the TCP Server notices that the connection was lost.
     * @param hook The function to be run
     */
    void onDisconnect(std::function<void()> hook) override;

  private:
    ILogger& m_logger;
    int m_serverFd, m_clientFd, m_port;
    int m_addressLength;
    struct sockaddr_in m_address;
    bool m_isClientConnected = false;
    std::function<void()> m_onConnect;
    std::function<void()> m_onDisonnect;

    /**
     * Create and bind the socket.
     */
    void init();
};

#endif // HIVE_MIND_BRIDGE_TCPSERVER_H
