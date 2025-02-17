#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <stdint.h>
#include <string>
class Message{
public:
    char signature[4]={'P','A','C','K'};
    uint32_t len1;
    uint32_t len2;
    uint32_t len3;
    const char* data1;
    const char* data2;
    const char* data3;
    char checksum;
    Message(
        uint32_t len1,
        const char* data1,
        uint32_t len2,
        const char* data2,
        uint32_t len3,
        const char* data3
    );
    Message(
        uint32_t len1,
        const char* data1,
        uint32_t len2,
        const char* data2
    ):Message(len1,data1,len2,data2,0,nullptr){};
    Message(
        uint32_t len1,
        const char* data1
    ):Message(len1,data1,0,nullptr,0,nullptr){ };
    Message(
        std::string data1,
        std::string data2,
        std::string data3
    ):Message(data1.length(),data1.c_str(),data2.length(),data2.c_str(),data3.length(),data3.c_str()) {};
    Message(
        std::string data1,
        std::string data2
    ):Message(data1.length(),data1.c_str(),data2.length(),data2.c_str(),0,nullptr) {};
    Message(
        std::string data1
    ):Message(data1.length(),data1.c_str(),0,nullptr,0,nullptr) {};
    ~Message();
};
#endif