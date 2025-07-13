/**
 * @file:	Graffito.cpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 14:26:35 Wednesday
 * @brief:
 **/

#include "SparklingGraffito/Graffito.hpp"

#include <memory>
#include <string_view>
#include <zmq.hpp>

std::vector<std::byte> m2b(const zmq::message_t& msg)
{
    const auto* data = static_cast<const std::byte*>(msg.data());
    return std::vector<std::byte>(data, data + msg.size());
}

// ================================================================================================
class Messenger::Impl
{
public:
    Impl(std::string_view address, bool is_server)
        : mAddr(address), mZCtx(1)
    {
        if (is_server)
        {
            mSocket = zmq::socket_t(mZCtx, ZMQ_REP);
            mSocket.bind(mAddr);
        }
        else
        {
            mSocket = zmq::socket_t(mZCtx, ZMQ_REQ);
            mSocket.connect(mAddr);
        }
    }

    ~Impl() = default;

    void send_impl(const Bytes& msg)
    {
        zmq::message_t req(msg.data(), msg.size());
        mSocket.send(req, zmq::send_flags::none);
    }

    Bytes recv_impl()
    {
        zmq::message_t rep;
        auto rr = mSocket.recv(rep);
        return m2b(rep);
    }

private:
    std::string mAddr;
    zmq::context_t mZCtx;
    zmq::socket_t mSocket;
};

Messenger::Messenger(std::string_view addr, bool is_server)
{
    pImpl = std::make_unique<Impl>(addr, is_server);
}

Messenger::~Messenger() = default;

void Messenger::send(const Bytes& msg) const
{
    pImpl->send_impl(msg);
}

Bytes Messenger::recv() const
{
    return pImpl->recv_impl();
}

// ================================================================================================

ReqClient::ReqClient(std::string_view address)
    : mMessenger(address, false)
{
}

ReqClient::~ReqClient() = default;

// ================================================================================================

RepServer::RepServer(std::string_view address)
    : mMessenger(address, true)
{
}

RepServer::~RepServer() = default;
