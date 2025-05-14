#include "server.hpp"



int main()
{
    Buffer buf;
    for(int i = 0; i <= 200; i++)
    {
        std::string str = "hello!!" + std::to_string(i) + '\n';
        buf.WriteStringAndPush(str);
    }
    while (buf.ReadAbleSize() > 0)
    {
        std::string line = buf.GetLineAndPop();
        //std::cout << line << std::endl;
        DBG_LOG("hello");
    }
    

    // std::string tmp;
    // tmp = buf.ReadAsStringAndPop(buf.ReadAbleSize());
    // std::cout << tmp << std::endl;


    // std::string str = "Hello!!";
    // buf.WriteStringAndPush(str);

    // Buffer buf1;
    // buf1.WriteBufferAndPush(buf);

    // std::string tmp = buf1.ReadAsStringAndPop(buf1.ReadAbleSize());

    // std::cout << tmp << std::endl;
    // std::cout << buf.ReadAbleSize() << std::endl;
    // std::cout << buf1.ReadAbleSize() << std::endl;
    return 0;
}