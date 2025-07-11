/**
 * @file:	Graffito.hpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 14:23:29 Wednesday
 * @brief:
 **/

#ifndef __GRAFFITO__H__
#define __GRAFFITO__H__

#include <array>
#include <bit>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// ================================================================================================
// Alias

using Bytes = std::vector<std::byte>;

// ================================================================================================
// Concept Definitions

// clang-format off

template <typename T>
concept TriviallyCopyable =
    std::is_trivially_copyable_v<T> &&
    !std::is_pointer_v<T> &&
    !std::is_reference_v<T>;

template <typename S>
concept Sendable =
    TriviallyCopyable<S> ||  // Supports trivially copyable types
    std::convertible_to<S, std::string_view> ||  // Supports string-like types
    requires(const S& s) {  // Supports containers of bytes (e.g., std::vector<std::byte>)
        { s.data() } -> std::convertible_to<const std::byte*>;
        { s.size() } -> std::convertible_to<size_t>;
    };

template <typename R>
concept Receivable =
    TriviallyCopyable<R> ||
    std::constructible_from<R, const char*, size_t> || // for string-like construction
    std::constructible_from<R, std::string_view>;

// clang-format on

// ================================================================================================

template <TriviallyCopyable T>
auto toBytesArr(const T& object)
{
    static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");

    return std::bit_cast<std::array<std::byte, sizeof(T)>>(object);
}

template <TriviallyCopyable T>
T fromBytesArr(const std::array<std::byte, sizeof(T)>& bytes)
{
    return std::bit_cast<T>(bytes);
}

// Convert object to vector of bytes
template <TriviallyCopyable T>
std::vector<std::byte> toBytes(const T& object)
{
    std::vector<std::byte> bytes(sizeof(T));
    std::memcpy(bytes.data(), &object, sizeof(T));
    return bytes;
}

// Convert vector of bytes back to object
template <TriviallyCopyable T>
T fromBytes(const std::vector<std::byte>& bytes)
{
    if (bytes.size() < sizeof(T))
    {
        throw std::runtime_error("Insufficient bytes for type");
    }

    T object;
    std::memcpy(&object, bytes.data(), sizeof(T));
    return object;
}

// For string-like types (convertible to string_view)
template <typename S>
    requires(!TriviallyCopyable<S> && std::convertible_to<S, std::string_view>)
std::vector<std::byte> toBytes(const S& str)
{
    std::string_view view = str;
    std::vector<std::byte> bytes(view.size());
    std::memcpy(bytes.data(), view.data(), view.size());
    return bytes;
}

// For string-like types (constructible from string_view)
template <typename R>
    requires(!TriviallyCopyable<R> && std::constructible_from<R, std::string_view>)
R fromBytes(const std::vector<std::byte>& bytes)
{
    std::string_view view(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    return R(view);
}

// ================================================================================================

// CRTP base
template <typename Drv>
class Messenger
{
protected:
    void send(const Bytes& msg)
    {
        static_cast<Drv*>(this)->send_impl(msg);
    }

    Bytes recv()
    {
        return static_cast<Drv*>(this)->recv_impl();
    }
};

// ================================================================================================

// CRTP derived
class ReqClient : public Messenger<ReqClient>
{
    friend class Messenger<ReqClient>;

public:
    ReqClient(const std::string& address);
    ~ReqClient();

    template <Sendable S, Receivable R>
    R sendAndRecv(const S& msg)
    {
        auto req = toBytes(msg);
        this->send(req);
        auto rep = this->recv();
        return fromBytes<R>(rep);
    }

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;

private:
    void send_impl(const Bytes& msg);
    Bytes recv_impl();
};

// ================================================================================================

// CRTP derived
class RepServer : public Messenger<RepServer>
{
    friend class Messenger<RepServer>;

public:
    RepServer(const std::string& address);
    ~RepServer();

    template <Sendable S, Receivable R>
    R recvAndSend(const S& msg)
    {
        auto req = this->recv();
        auto rep = toBytes(msg);
        this->send(rep);
        return fromBytes<R>(req);
    }

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;

private:
    void send_impl(const Bytes& msg);
    Bytes recv_impl();
};

#endif  //!__GRAFFITO__H__
