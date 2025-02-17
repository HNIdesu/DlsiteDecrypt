#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
class TcpClient
{
private:
    unsigned int mSocket;
    bool mIsClosed = false;
    void AssertNotClosed() const;
public:
    TcpClient();
    ~TcpClient();
    bool Connect(const char* host, unsigned short port);
    bool IsClosed() const {
        return this->mIsClosed;
    }
    int Send(const char* data, int length);
    int Receive(char* buffer, int bufferSize);
    void Close();
};

#endif
