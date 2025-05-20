#include "echo.hpp"

int main()
{
    EchoServer server(8080);
    server.Start();
    std::cout << "its all " << std::endl;
    return 0;
}
