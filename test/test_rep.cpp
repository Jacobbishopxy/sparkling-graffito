/**
 * @file:	test_rep.cpp
 * @author:	Jacob Xie
 * @date:	2025/07/09 23:37:12 Wednesday
 * @brief:
 **/

#include <SparklingGraffito/Graffito.hpp>

int main(int argc, char** argv)
{
    const std::string addr = "tcp://localhost:5555";

    RepServer server(addr);

    server.start();

    return 0;
}
