/**
 * @file:	Graffito.hpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 14:23:29 Wednesday
 * @brief:
 **/

#ifndef __GRAFFITO__H__
#define __GRAFFITO__H__

#include <cstring>
#include <memory>
#include <string_view>
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
    requires(const S& s) {  // Supports containers of bytes
        { s.data() } -> std::convertible_to<const std::byte*>;
        { s.size() } -> std::convertible_to<size_t>;
    };

template <typename R>
concept Receivable =
    TriviallyCopyable<R> ||
    std::constructible_from<R, const char*, size_t> || // for string-like construction
    std::constructible_from<R, std::string_view>;

// clang-format on

// Single toBytes function that handles all Sendable types
template <Sendable S>
Bytes toBytes(const S& source)
{
    if constexpr (TriviallyCopyable<S>)
    {
        Bytes bytes(sizeof(S));
        std::memcpy(bytes.data(), &source, sizeof(S));
        return bytes;
    }
    else if constexpr (std::convertible_to<S, std::string_view>)
    {
        std::string_view view = source;
        Bytes bytes(view.size());
        std::memcpy(bytes.data(), view.data(), view.size());
        return bytes;
    }
    else
    {  // Byte container case
        return Bytes(
            reinterpret_cast<const std::byte*>(source.data()),
            reinterpret_cast<const std::byte*>(source.data()) + source.size());
    }
}

template <typename>
inline constexpr bool always_false = false;

// Single fromBytes function that handles all Receivable types
template <Receivable R>
R fromBytes(const Bytes& bytes)
{
    if constexpr (TriviallyCopyable<R>)
    {
        if (bytes.size() < sizeof(R))
        {
            throw std::runtime_error("Insufficient bytes for type");
        }
        R result;
        std::memcpy(&result, bytes.data(), sizeof(R));
        return result;
    }
    else if constexpr (std::constructible_from<R, const char*, size_t>)
    {
        return R(
            reinterpret_cast<const char*>(bytes.data()),
            bytes.size());
    }
    else if constexpr (std::constructible_from<R, std::string_view>)
    {
        return R(std::string_view(
            reinterpret_cast<const char*>(bytes.data()),
            bytes.size()));
    }
    else
    {
        static_assert(always_false<R>, "Type is not Receivable");
    }
}

// ================================================================================================

class Messenger
{
public:
    Messenger(std::string_view address, bool is_server);
    ~Messenger();

public:
    void send(const Bytes& msg) const;
    Bytes recv() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// ================================================================================================

class ReqClient
{
public:
    ReqClient(std::string_view address);
    ~ReqClient();

    template <Sendable S, Receivable R>
    R sendAndRecv(const S& msg) const
    {
        auto req = toBytes(msg);
        mMessenger.send(req);
        auto rep = mMessenger.recv();
        return fromBytes<R>(rep);
    }

private:
    Messenger mMessenger;
};

// ================================================================================================

class RepServer
{
public:
    RepServer(std::string_view address);
    ~RepServer();

    template <Sendable S, Receivable R>
    R recvAndSend(const S& msg)
    {
        auto req = mMessenger.recv();
        auto rep = toBytes(msg);
        mMessenger.send(rep);
        return fromBytes<R>(req);
    }

private:
    Messenger mMessenger;
};

#endif  //!__GRAFFITO__H__
