
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

#include "../../../LIB_NETWORKCLASS/NETWORKCLASS.hpp"

NETWORKCLASS udpObj;

int main(int argc, char** argv) {

    std::string serverIP    = "192.168.1.177";
    int         serverPORT  = 1086;

    if(argc>1) {
        if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            std::cout <<"Usage: cv_video_cli -p/--port [port] -a/--address [address]\n" <<
                        " port      : socket port (8080 default)\n" <<
                        " address   : server IPaddress (\"192.168.1.177\" default (same device))\n" << std::endl;
            exit(0);
        }
        for(int i=1; i<argc; i++) {
            if(i<argc) {
                if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
                    serverPORT = std::stoi(argv[i+1]);
                }
                if(!strcmp(argv[i], "-a") || !strcmp(argv[i], "--address")) {
                    serverIP = argv[i+1];
                }
            }
        }
    }
    

    udpObj = NETWORKCLASS(serverIP, serverPORT);
    if(!udpObj.func_createSocket(AF_INET, SOCK_DGRAM)) exit(1);
    
    fcntl(udpObj.get_localSocket(), F_SETFL, O_NONBLOCK);
    memset(&udpObj._local_sockaddr_in, 0, sizeof(udpObj._local_sockaddr_in));

    udpObj._local_sockaddr_in.sin_family = AF_INET;
    if(bind(udpObj.get_localSocket(), (sockaddr*)&udpObj._local_sockaddr_in, sizeof(udpObj._local_sockaddr_in))==-1) {
        int lasterror = errno;
        std::cout << "bind() error: " << errno << std::endl;
        exit(1);
    }
    // if(!udpObj.func_bind()) exit(1);

    if(resolvehelper(serverIP.c_str(), AF_INET, std::to_string(serverPORT).c_str(), (sockaddr_storage*)&udpObj._remote_sockaddr_in)!=0) {
        int lasterror = errno;
        std::cout << "resolvehelper() error: " << lasterror << std::endl;
        exit(1);
    }
    
    cv::Mat img;
    // img = cv::Mat::zeros(480, 640, CV_8UC1);
    img = cv::Mat::zeros(200, 320, CV_8UC1);
    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int key = 0;
    if(!img.isContinuous()) img = img.clone();

    std::cout << "Image Size: " << imgSize << std::endl;

    cv::namedWindow("CV Video Client", 1);

    std::string msgSend = "Hello server.";
    int recvLen = 0;
    // std::string msgRecv = ""
    while(key!='q') {
        int result = udpObj.func_sendto(
            udpObj.get_localSocket(),
            msgSend.c_str(),
            msgSend.length(),
            0,
            (sockaddr*)&udpObj._remote_sockaddr_in,
            sizeof(udpObj._remote_sockaddr_in)
        );
        if(result==-1) {
            int lasterror = errno;
            std::cout << "sendto() error: " << lasterror << std::endl;;
            exit(1);
        }
        if((bytes = udpObj.func_recvfrom(
                udpObj.get_localSocket(),
                (char*)iptr,
                imgSize,
                MSG_WAITALL,
                (struct sockaddr*)&udpObj._remote_sockaddr_in,
                &recvLen
            ))==-1
        ) {
            // std::cerr << "recv failed: " << bytes << " bytes. errno:" << errno << std::endl;
            continue;
            // exit(1);
        }

        cv::imshow("CV Video Client", img);
        if(key = cv::waitKey(10)>=0) break;
    }

    return 0;
}
