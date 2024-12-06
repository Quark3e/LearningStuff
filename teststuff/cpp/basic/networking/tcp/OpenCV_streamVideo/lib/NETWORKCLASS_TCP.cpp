
#include "NETWORKCLASS_TCP.hpp"


NETWORKCLASS_TCP::NETWORKCLASS_TCP() {
    this->func_init();
}
NETWORKCLASS_TCP::NETWORKCLASS_TCP(std::string _ipAddress, int _port) {
    this->_local_IPADDRESS = _ipAddress;
    this->_local_PORT = _port;
    this->func_init();
}


bool NETWORKCLASS_TCP::set_IPADDRESS(std::string _ipAddress) {
    if(_init) {
        return false;
    }
    _local_IPADDRESS = _ipAddress;
    return true;
}
bool NETWORKCLASS_TCP::set_PORT(int _port) {
    if(_init) {
        return false;
    }
    _local_PORT;
    return true;
}
std::string NETWORKCLASS_TCP::get_IPADDRESS() {
    return _local_IPADDRESS;
}
int NETWORKCLASS_TCP::get_PORT() {
    return _local_PORT;
}


#if _WIN32
SOCKET*     NETWORKCLASS_TCP::get_localSocket() { return &_localSocket; }
SOCKET*     NETWORKCLASS_TCP::get_remoteSocket() { return &_remoteSocket; }
#else
int         NETWORKCLASS_TCP::get_localSocket() { return _localSocket; }
int         NETWORKCLASS_TCP::get_remoteSocket() { return _remoteSocket; }
#endif

bool NETWORKCLASS_TCP::func_init() {
    if(this->_init) return true;
#if _WIN32
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    if(wsaerr!=0) {
        std::cout << "The winsock dll not found" << std::endl;
        return false;
    }
    else {
        std::cout << "The winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }
#else

#endif

    this->_init = true;
}
bool NETWORKCLASS_TCP::func_sockCreate() {
#if _WIN32
    this->_localSocket = INVALID_SOCKET;
    this->_localSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(_localSocket==INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    else {
        std::cout << "Socket successfully created" << std::endl;
    }
#else
    if((_localSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "socket() failed!" << std::endl;
        return false;
    }
    else {
        std::cout << "Socket successfully created." << std::endl;
    }
#endif

    return true;
}
bool NETWORKCLASS_TCP::func_connect() {

    _remote_sockaddr_in.sin_family = AF_INET;
    _remote_sockaddr_in.sin_addr.s_addr = inet_addr(_local_IPADDRESS.c_str());
    _remote_sockaddr_in.sin_port = htons(_local_PORT);
#if _WIN32
    if(connect(_localSocket, reinterpret_cast<SOCKADDR*>(&_remote_sockaddr_in), sizeof(sockaddr_in)) < 0) {
        std::cout << "connect() failed." << std::endl;
        WSACleanup();
        return false;
    }
#else
    if(connect(_localSocket, (sockaddr*)&_remote_sockaddr_in, sizeof(sockaddr_in)) < 0) {
        std::cout << "connect() failed." << std::endl;
        return false;
    }
#endif

    return true;
}
bool NETWORKCLASS_TCP::func_bind() {

    _local_sockaddr_in.sin_family = AF_INET;
    _local_sockaddr_in.sin_addr.s_addr = (_local_IPADDRESS=="ANY"? INADDR_ANY : inet_addr(_local_IPADDRESS.c_str()));
    _local_sockaddr_in.sin_port = htons(_local_PORT);
#if _WIN32
    if(bind(_localSocket, reinterpret_cast<SOCKADDR*>(&_local_sockaddr_in), sizeof(_local_sockaddr_in)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(_localSocket);
        WSACleanup();
        return false;
    }
#else
    if(bind(_localSocket, (struct sockaddr*)&_localSocket, sizeof(_localSocket)) < 0) {
        std::cout << "bind() failed!" << std::endl;
        return false;
    }
#endif
    else std::cout << "Socket successfully bound." << std::endl;

    return true;
}
bool NETWORKCLASS_TCP::func_listen(int _numQueued) {

#if _WIN32
    if(listen(_localSocket, _numQueued) == SOCKET_ERROR) {
        std::cout << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
        return false;
    }
#else
    if(listen(_localSocket, _numQueued) < 0) {
        std::cout << "listen(): Error listening on socket. " << std::endl;
        return false;
    }
#endif
    else {
        std::cout << "Waiting for new connections..." << std::endl;
    }

    return true;
}
bool NETWORKCLASS_TCP::func_accept() {

#if _WIN32
    if((_remoteSocket = accept(_localSocket, nullptr, nullptr)) == INVALID_SOCKET) {
        std::cout << "accept() failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
#else
    if((_remoteSocket = accept(_localSocket, (struct sockaddr*)&_remoteSocket, (socklen_t*)&_sockAddrLen)) < 0) {
        std::cout << "accept() failed" << std::endl;
        return false;
    }
#endif
    else {
        std::cout << "Successfully ran accept()" << std::endl;
    }

    return true;
}
bool NETWORKCLASS_TCP::func_receive() {
    this->func_receive(recvBuffer, sizeof(recvBuffer), 0);
    if(_bytesRecv<0) return false;
    return true;
}
int NETWORKCLASS_TCP::func_receive(void* _recvBuf, size_t _nBytes, int _flags) {
    _bytesRecv = recv(
        _remoteSocket,
#if _WIN32
        (char*)_recvBuf,
        (int)_nBytes,
#else
        _recvBuf,
        _nBytes,
#endif
        _flags
    );

    return _bytesRecv;
}
bool NETWORKCLASS_TCP::func_send() {
    this->func_send(sendBuffer, sizeof(sendBuffer), 0);
    if(_bytesSent<0) return false;
    return true;
}
int NETWORKCLASS_TCP::func_send(const void* _sendBuf, size_t _nBytes, int _flags) {
    _bytesSent = send(
        _remoteSocket,
#if _WIN32
        (const char*)_sendBuf,
        (int)_nBytes,
#else
        _sendBuf,
        _nBytes,
#endif
        _flags
    );
    return _bytesSent;
}
