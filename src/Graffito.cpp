/**
 * @file:	Graffito.cpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 14:26:35 Wednesday
 * @brief:
 **/

#include "SparklingGraffito/Graffito.hpp"

#include <zmq.hpp>

std::vector<std::byte> m2b(const zmq::message_t& msg)
{
    const auto* data = static_cast<const std::byte*>(msg.data());
    return std::vector<std::byte>(data, data + msg.size());
}

// ================================================================================================
class ReqClient::Impl
{
public:
    Impl(const std::string& address)
        : mAddr(address),mZCtx(1), mReq(mZCtx, ZMQ_REQ)
    {
        mReq.connect(address);
    }

    ~Impl() = default;

    void send_impl(const Bytes& msg)
    {
        zmq::message_t req(msg.data(), msg.size());
        mReq.send(req, zmq::send_flags::none);
    }

    Bytes recv_impl()
    {
        zmq::message_t rep;
        auto rr = mReq.recv(rep);
        return m2b(rep);
    }

private:
    std::string mAddr;
    zmq::context_t mZCtx;
    zmq::socket_t mReq;
};

// ReqClient methods
ReqClient::ReqClient(const std::string& server_address)
    : mImpl(std::make_unique<Impl>(server_address)) {}

ReqClient::~ReqClient() = default;

void ReqClient::send_impl(const Bytes& msg)
{
    mImpl->send_impl(msg);
}

Bytes ReqClient::recv_impl()
{
    return mImpl->recv_impl();
}

// ================================================================================================

class RepServer::Impl
{
public:
    Impl(const std::string& address)
        : mAddr(address), mZCtx(1), mRep(mZCtx, ZMQ_REP)
    {
        mRep.bind(address);
    }

    ~Impl() = default;

    void send_impl(Bytes msg)
    {
        zmq::message_t req(msg.data(), msg.size());
        mRep.send(req, zmq::send_flags::none);
    }

    Bytes recv_impl()
    {
        zmq::message_t req;
        auto rr = mRep.recv(req);
        return m2b(req);
    }

private:
    std::string mAddr;
    zmq::context_t mZCtx;
    zmq::socket_t mRep;
};

// RepServer methods
RepServer::RepServer(const std::string& address)
    : mImpl(std::make_unique<Impl>(address)) {}

RepServer::~RepServer() = default;

void RepServer::send_impl(const Bytes& msg)
{
    mImpl->send_impl(msg);
}

Bytes RepServer::recv_impl()
{
    return mImpl->recv_impl();
}