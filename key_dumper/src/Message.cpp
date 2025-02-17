#include "Message.h"
#include <cstring>
Message::Message(
    uint32_t len1,
    const char* data1,
    uint32_t len2,
    const char* data2,
    uint32_t len3,
    const char* data3
){
    char checksum = 0;
    for(int i=0;i<len1;i++)
        checksum ^= data1[i];
    for(int i=0;i<len2;i++)
        checksum ^= data2[i];
    for(int i=0;i<len3;i++)
        checksum ^= data3[i];
    this->checksum = checksum;
    this->len1=len1;
    this->len2=len2;
    this->len3=len3;
    if(len1!=0){
        auto buffer = new char[len1];
        memcpy(buffer, data1, len1);
        this->data1 = buffer;
    }else
        this->data1=nullptr;
    if(len2!=0){
        auto buffer = new char[len2];
        memcpy(buffer, data2, len2);
        this->data2 = buffer;
    }else
        this->data2=nullptr;
    if(len3!=0){
        auto buffer = new char[len3];
        memcpy(buffer, data3, len3);
        this->data3 = buffer;
    }else
        this->data3=nullptr;
}

Message::~Message()
{
    if (this->data1 != nullptr)
        delete[] this->data1;
    if (this->data2 != nullptr)
        delete[] this->data2;
    if (this->data3 != nullptr)
        delete[] this->data3;
}
