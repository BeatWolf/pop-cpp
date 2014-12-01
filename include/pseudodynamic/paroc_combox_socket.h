/*
AUTHORS: Tuan Anh Nguyen

DESCRIPTION: socket declaration of combox
 */

#ifndef POPC_COMBOX_SOCKET_H
#define POPC_COMBOX_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <ctype.h>

#include <paroc_array.h>

#include "paroc_combox.h"
/**
 * @class paroc_connection_sock
 * @brief Socket connection, used by POP-C++ runtime.
 * @author Tuan Anh Nguyen
 *
 */
class paroc_connection_sock: public  paroc_connection {
public:
    paroc_connection_sock(paroc_combox *cb);
    paroc_connection_sock(int fd, paroc_combox *cb);
    paroc_connection_sock(paroc_connection_sock &me);

    virtual paroc_connection *Clone();

    int sockfd;
};

/**
 * @class paroc_combox_socket
 * @brief Socket declaration of combox, used by POP-C++ runtime.
 * @author Tuan Anh Nguyen
 *
 */
class paroc_combox_socket:public paroc_combox {
public:
    paroc_combox_socket();

    virtual ~paroc_combox_socket();

    virtual bool Create(char* host, int port=0, bool server=false);

    virtual bool Connect(const char *url);
    virtual bool connect_and_die(std::string &url) {
        return true;
    }

    virtual paroc_connection* get_connection();

    virtual int Send(const char *s,int len);
    virtual int Send(const char *s,int len, paroc_connection *conn, bool unlock);


    virtual int Recv(char *s,int len, bool unlock);
    virtual int Recv(char *s,int len, paroc_connection *&iopeer, bool unlock);

    virtual paroc_connection *Wait();

    virtual void Close();



    /**
     * @brief Returns URL of object
     * @param accesspoint Returned URL (protocol://host:port)
     * @return true if success
     */
    virtual bool GetUrl(paroc_string & accesspoint);
    virtual bool GetProtocol(paroc_string & protocolName);

protected:
    virtual paroc_connection *CreateConnection(int fd);
    bool CloseSock(int fd);
    bool Connect(const char *host,int port);
    virtual bool disconnect(paroc_connection *connection);
    virtual bool is_server();

    int GetSockInfo(sockaddr &info,socklen_t &len);
    int GetPort();

    int GetOpt(int level, int opt, char *buf, socklen_t &len);
    int SetOpt(int level, int opt, char *buf, socklen_t len);

protected:
    int sockfd;
    bool _isServer;
    bool isCanceled;

    paroc_connection_sock *peer;

    //Only used by combox server...
    paroc_array<pollfd> pollarray;
    paroc_array<paroc_connection_sock *> connarray;
    int index;
    int nready;
};



#endif







