/**
 * @file:	test_req.cpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 23:37:05 Wednesday
 * @brief:
 **/

#include <SparklingGraffito/Graffito.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    const std::string addr = "tcp://localhost:5555";

    ReqClient client(addr);
    auto reply = client.sendAndRecv("Hello, Server!");
    std::cout << "reply: " << reply << std::endl;

    return 0;
}
