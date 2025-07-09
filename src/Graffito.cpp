/**
 * @file:	Graffito.cpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 14:26:35 Wednesday
 * @brief:
 **/

#include "SparklingGraffito/Graffito.hpp"

#include <iostream>
#include <stdexcept>

ReqClient::ReqClient(const std::string& server_address)
    : mZCtx(1), mReq(mZCtx, zmq::socket_type::req), mServerAddr(server_address)
{
    mReq.connect(mServerAddr);
};

std::string ReqClient::sendAndRecv(const std::string& message)
{
    zmq::message_t request(message.begin(), message.end());
    mReq.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    auto recv_result = mReq.recv(reply, zmq::recv_flags::none);
    if (!recv_result)
    {
        throw std::runtime_error("Failed to receive reply");
    }
    std::string reply_str(reply.to_string());
    return reply_str;
}

RepServer::RepServer(const std::string& address)
    : mZCtx(1), mRep(mZCtx, zmq::socket_type::rep), mAddr(address)
{
    mRep.bind(mAddr);
}

void RepServer::start()
{
    while (true)
    {
        zmq::message_t request;
        auto recv_result = mRep.recv(request, zmq::recv_flags::none);
        if (!recv_result)
        {
            throw std::runtime_error("Failed to receive request");
        }
        std::string request_str(request.to_string());
        std::cout << "Received request: " << request_str << std::endl;

        // Prepare and send reply
        std::string reply_str = "Reply to: " + request_str;
        zmq::message_t reply(reply_str.begin(), reply_str.end());
        mRep.send(reply, zmq::send_flags::none);
        std::cout << "Sent reply: " << reply_str << std::endl;
    }
}
