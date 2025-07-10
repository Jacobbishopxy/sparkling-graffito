/**
 * @file:	test_req.cpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 23:37:05 Wednesday
 * @brief:
 **/

#include <SparklingGraffito/Graffito.hpp>
#include <iostream>
#include <thread>

// Same types as server for compatibility
struct BinaryRequest
{
    int32_t id;
    double value;
};

struct BinaryResponse
{
    int32_t status;
    double result;
};

struct TextResponse
{
    std::string data;

    TextResponse(std::string_view sv)
        : data(sv) {}
};

void run_test_client()
{
    try
    {
        ReqClient client("tcp://localhost:5555");

        // Test 1: Text request/response
        std::string text_resp = client.sendAndRecv<std::string, std::string>("Hello");
        std::cout << "Text response: " << text_resp << std::endl;

        // Test 2: Binary request/response
        BinaryResponse bin_resp = client.sendAndRecv<BinaryRequest, BinaryResponse>({42, 2.71828});
        std::cout << "Binary response: status=" << bin_resp.status
                  << ", result=" << bin_resp.result << std::endl;

        // Test 3: Mixed types
        TextResponse mixed_resp = client.sendAndRecv<std::string, TextResponse>("Query");
        std::cout << "Mixed response: " << mixed_resp.data << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}

int main()
{
    // Wait briefly to ensure server is up
    std::this_thread::sleep_for(std::chrono::seconds(1));

    run_test_client();
    return 0;
}