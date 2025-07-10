/**
 * @file:	test_rep.cpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 23:37:12 Wednesday
 * @brief:
 **/

#include <SparklingGraffito/Graffito.hpp>
#include <iostream>
#include <thread>

// Simple message types
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

void run_test_server()
{
    try
    {
        RepServer server("tcp://localhost:5555");

        std::cout << "Server ready\n";

        int round(0);

        while (true)
        {
            std::cout << "round: " << round++ << std::endl;
            // Test 1: Text request/response
            auto text_req = server.recvAndSend<std::string, std::string>("ACK");
            std::cout << "Received text: " << text_req << std::endl;

            // // Test 2: Binary request/response
            // auto bin_req = server.recvAndSend<BinaryResponse, BinaryRequest>({200, 3.14159});
            // std::cout << "Received binary: id=" << bin_req.id
            //           << ", value=" << bin_req.value << std::endl;

            // // Test 3: Mixed types
            // TextResponse mixed_resp = server.recvAndSend<std::string, TextResponse>("Processed");
            // std::cout << "Mixed response: " << mixed_resp.data << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}

int main()
{
    run_test_server();
    return 0;
}