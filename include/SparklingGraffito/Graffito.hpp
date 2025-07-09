/**
 * @file:	Graffito.hpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 14:23:29 Wednesday
 * @brief:
 **/

#ifndef __GRAFFITO__H__
#define __GRAFFITO__H__

#include <zmq.hpp>

class ReqClient
{
public:
    ReqClient(const std::string& server_address);
    ~ReqClient() = default;

    std::string sendAndRecv(const std::string& message);

private:
    zmq::context_t mZCtx;
    zmq::socket_t mReq;
    std::string mServerAddr;
};

class RepServer
{
public:
    RepServer(const std::string& address);
    ~RepServer() = default;

    void start();

private:
    zmq::context_t mZCtx;
    zmq::socket_t mRep;
    std::string mAddr;
};

#endif  //!__GRAFFITO__H__
