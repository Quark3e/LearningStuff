
#include "hexclaw_networkDataThread.hpp"


// NETWORK_DATA_THREADCLASS::NETWORK_DATA_THREADCLASS() {
//     this->func_init();
// }
NETWORK_DATA_THREADCLASS::NETWORK_DATA_THREADCLASS(bool _init, std::string _ipAddress, int _port) {
    this->_local_IPADDRESS = _ipAddress;
    this->_local_PORT = _port;
    this->func_init();
}
NETWORK_DATA_THREADCLASS::~NETWORK_DATA_THREADCLASS() {
    assert(this->_closing());

    if(this->isRunning.load()) {
        this->running = false;
        this->threadObj.join();
    }
}
bool NETWORK_DATA_THREADCLASS::_closing() {
#if _WIN32
    closesocket(_localSocket);
    WSACleanup();
#else
    close(_remoteSocket);
#endif
    // mtx_print("NDT: closing");
    return true;
}


bool NETWORK_DATA_THREADCLASS::set_IPADDRESS(std::string _ipAddress) {
    if(_init) {
        return false;
    }
    _local_IPADDRESS = _ipAddress;
    return true;
}
bool NETWORK_DATA_THREADCLASS::set_PORT(int _port) {
    if(_init) {
        return false;
    }
    _local_PORT;
    return true;
}
std::string NETWORK_DATA_THREADCLASS::get_IPADDRESS() {
    return _local_IPADDRESS;
}
int NETWORK_DATA_THREADCLASS::get_PORT() {
    return _local_PORT;
}


#if _WIN32
SOCKET&     NETWORK_DATA_THREADCLASS::get_localSocket() { return _localSocket; }
SOCKET&     NETWORK_DATA_THREADCLASS::get_remoteSocket() { return _remoteSocket; }
#else
int&        NETWORK_DATA_THREADCLASS::get_localSocket() { return _localSocket; }
int&        NETWORK_DATA_THREADCLASS::get_remoteSocket() { return _remoteSocket; }
#endif

int NETWORK_DATA_THREADCLASS::func_init() {
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

	memset(&_local_sockaddr_in, 0, sizeof(_local_sockaddr_in));
	memset(&_remote_sockaddr_in, 0, sizeof(_remote_sockaddr_in));


    assert(func_createSocket(AF_INET, SOCK_STREAM));
    assert(func_connect());

    threadObj = std::thread(_NDT_threadFunc, this);

    this->_init = true;
    return true;
}
bool NETWORK_DATA_THREADCLASS::func_createSocket(int _sock_family, int _sock_type, int _sock_proto) {
#if _WIN32
    this->_localSocket = INVALID_SOCKET;
    this->_localSocket = socket(_sock_family, _sock_type, _sock_proto);

    if(_localSocket==INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    else {
        // std::cout << "Socket successfully created" << std::endl;
    }
#else
    if((_localSocket = socket(_sock_family, _sock_type, _sock_proto)) < 0) {
        std::cout << "socket() failed!" << std::endl;
        return false;
    }
    else {
        // std::cout << "Socket successfully created." << std::endl;
    }
#endif
    
    return true;
}
bool NETWORK_DATA_THREADCLASS::func_connect() {
    std::cout << _local_IPADDRESS << std::endl;

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
    if(connect(_localSocket, (sockaddr*)&_remote_sockaddr_in, sizeof(_remote_sockaddr_in)) < 0) {
        std::cout << "connect() failed." << std::endl;
        return false;
    }
#endif

    return true;
}
bool NETWORK_DATA_THREADCLASS::func_bind() {

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
    if(bind(_localSocket, (const struct sockaddr*)&_local_sockaddr_in, sizeof(_local_sockaddr_in))==-1) {
        std::cout << "bind() failed!" << std::endl;
        return false;
    }
#endif
    // else std::cout << "Socket successfully bound." << std::endl;

    return true;
}
bool NETWORK_DATA_THREADCLASS::func_listen(int _numQueued) {

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
        std::cout << "Waiting for new connections at: " << std::endl;
        std::cout << " ip   : " << _local_IPADDRESS << std::endl;
        std::cout << " port : " << _local_PORT << std::endl;
    }

    return true;
}
bool NETWORK_DATA_THREADCLASS::func_accept() {

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
        std::cout << "Successfully accepted connection." << std::endl;
        // getsockname(_remoteSocket, )
    }

    return true;
}
bool NETWORK_DATA_THREADCLASS::func_recv(int recvFrom) {
    this->func_recv(recvFrom, recvBuffer, sizeof(recvBuffer), 0);
    if(_bytesRecv<0) return false;
    return true;
}
int NETWORK_DATA_THREADCLASS::func_recv(int recvFrom, void* _recvBuf, size_t _nBytes, int _flags) {
    _bytesRecv = recv(
        (recvFrom==0? _localSocket : _remoteSocket),
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
bool NETWORK_DATA_THREADCLASS::func_send(int sendTo) {
    this->func_send(sendTo, sendBuffer, sizeof(sendBuffer), 0);
    if(_bytesSent<0) return false;
    return true;
}
int NETWORK_DATA_THREADCLASS::func_send(int sendTo, const void* _sendBuf, size_t _nBytes, int _flags) {
    _bytesSent = send(
        (sendTo==0? _localSocket : _remoteSocket),
#if _WIN32
        reinterpret_cast<const char*>(_sendBuf),
        (int)_nBytes,
#else
        _sendBuf,
        _nBytes, MSG_NOSIGNAL | 
#endif
        _flags
    );
    return _bytesSent;
}
#if _WIN32
int NETWORK_DATA_THREADCLASS::func_recvfrom(SOCKET _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len) {
    _bytesRecv = recvfrom(
        _sock,
        reinterpret_cast<char*>(_sendBuf),
        _nBytes,
        _flags,
        _from_addr,
        (int*)_from_addr_len
    );
    return _bytesRecv;
}
int NETWORK_DATA_THREADCLASS::func_sendto(SOCKET _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len) {
    _bytesSent = sendto(
        _sock,
        reinterpret_cast<const char*>(_sendBuf),
        int(_nBytes),
        _flags,
        _to_addr,
        _to_addr_len
    );
    return _bytesSent;
}
#else
int NETWORK_DATA_THREADCLASS::func_recvfrom(int _sock, void* _sendBuf, size_t _nBytes, int _flags, sockaddr* _from_addr, void* _from_addr_len) {
    _bytesRecv = recvfrom(
        _sock,
        (char*)_sendBuf,
        _nBytes,
        _flags,
        _from_addr,
        (socklen_t*)_from_addr_len
    );
    return _bytesRecv;
}
int NETWORK_DATA_THREADCLASS::func_sendto(int    _sock, const void* _sendBuf, size_t _nBytes, int _flags, const sockaddr* _to_addr, int _to_addr_len) {
    _bytesSent = sendto(
        _sock,
        _sendBuf,
        _nBytes,
        _flags,
        _to_addr,
        _to_addr_len
    );
    return _bytesSent;
}
#endif

void _NDT_threadFunc(NETWORK_DATA_THREADCLASS* ndt_obj) {
    std::unique_lock<std::mutex> u_lck_imgArr((*ndt_obj).imgMutex, std::defer_lock);
    (*ndt_obj).isRunning = true;
    int _rc, _i, _j;
    struct jpeg_decompress_struct   cinfo;
    struct jpeg_error_mgr           jerr;
    int row_stride, width, height, pixel_size;
    uint16_t arrSize;

    // mtx_print("ndt thread started: ");

    if(!(*ndt_obj)._init) {
        std::cerr << "init not true before _threadFunc initialized."<<std::endl;
        exit(1);
    }
    if(!(*ndt_obj).running.load()) {
        std::cerr << "running called false before _threadFunc initialized."<<std::endl;
        exit(1);
    }
    while((*ndt_obj).running.load()) {
        // mtx_print("ndt: thread iteration");
        if(!(*ndt_obj).runLoop.load()) continue;
        // mtx_print("ndt: proces started");
        if(((*ndt_obj)._bytesSent = (*ndt_obj).func_send(0, "request---", 10, 0))==-1) {
            perror("send() for request failed: ");
            exit(1);
        }

        if(((*ndt_obj)._bytesRecv = (*ndt_obj).func_recv(0, &arrSize, sizeof(arrSize), MSG_WAITALL))==-1) {
            perror("recv() for arrSize failed: ");
            exit(1);
        }
        std::vector<uint8_t> bitArr = std::vector<uint8_t>(arrSize);
        if(((*ndt_obj)._bytesRecv = (*ndt_obj).func_recv(0, bitArr.data(), arrSize*sizeof(bitArr[0]), MSG_WAITALL))==-1) {
            perror("recv() for bitArr failed: ");
            exit(1);
        }

        (*ndt_obj).img_size = arrSize;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);
        jpeg_mem_src(&cinfo, bitArr.data(), (*ndt_obj).img_size);
        _rc = jpeg_read_header(&cinfo, true);
        if(_rc!=1) {
            perror("jpeg_read_header() error: ");
            exit(1);
        }
        cinfo.out_color_components = JCS_GRAYSCALE;
        jpeg_start_decompress(&cinfo);
        width       = cinfo.output_width;
        height      = cinfo.output_height;
        pixel_size  = cinfo.output_components;
        (*ndt_obj).img_size    = width * height * pixel_size;
        (*ndt_obj).imgArr_sub = std::vector<uint8_t>((*ndt_obj).img_size);
        row_stride  = width * pixel_size;
        while(cinfo.output_scanline < cinfo.output_height) {
            unsigned char *buffer_array[1];
            buffer_array[0] = (*ndt_obj).imgArr_sub.data() + (cinfo.output_scanline)*row_stride;
            jpeg_read_scanlines(&cinfo, buffer_array, 1);
        }
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        u_lck_imgArr.lock();
        (*ndt_obj).imgArr = (*ndt_obj).imgArr_sub;
        u_lck_imgArr.unlock();
        (*ndt_obj).imgInit = true;
    }
    if(((*ndt_obj)._bytesSent = (*ndt_obj).func_send(0, "disconnect", 10, 0))==-1) {
        perror("send() for disconnect failed: ");
        exit(1);
    }
    (*ndt_obj).isRunning = false;
}