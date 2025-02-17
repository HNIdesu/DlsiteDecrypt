#include "TcpClient.h"
#include <iostream>
#include <ws2tcpip.h>

void TcpClient::AssertNotClosed() const
{
    if (this->mIsClosed)
        throw std::exception("Socket has been closed");
}

TcpClient::TcpClient(){
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::exception("WSAStartup failed");
    auto clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );
    if (clientSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::exception("Create socket failed");
    }
    this->mSocket = clientSocket;
}

bool TcpClient::Connect(const char* host, unsigned short port)
{
    AssertNotClosed();
    auto addr=sockaddr_in();
    if (inet_pton(AF_INET, host, &(addr.sin_addr)) < 0)
        throw std::exception("InetPton failed");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    auto result = connect(this->mSocket, (sockaddr*)&addr, sizeof(addr));
    return result != SOCKET_ERROR;
}


int TcpClient::Send(const char* data, int length){
    this->AssertNotClosed();
    auto result = send(this->mSocket, data, length, NULL);
    return result == SOCKET_ERROR ? -1 : result;

}

int TcpClient::Receive(char* buffer, int bufferSize)
{
    this->AssertNotClosed();
    auto result = recv(this->mSocket, buffer, bufferSize, 0);
    return result == SOCKET_ERROR ? -1 : result;
}

void TcpClient::Close()
{
    if (!mIsClosed) {
        closesocket(this->mIsClosed);
        WSACleanup();
        mIsClosed = true;
        this->mSocket = INVALID_SOCKET;
    }
}

TcpClient::~TcpClient() {
    Close();
}
