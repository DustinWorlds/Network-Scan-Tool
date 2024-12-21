#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

std::mutex output_mutex;

bool scanPort(const std::string& ip, int port, std::string& serviceName) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    u_long mode = 1; 
    ioctlsocket(sock, FIONBIO, &mode);

    int result = connect(sock, (sockaddr*)&addr, sizeof(addr));

    if (result == 0 || (result == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)) {
        fd_set writeSet;
        timeval timeout;
        FD_ZERO(&writeSet);
        FD_SET(sock, &writeSet);

        timeout.tv_sec = 0; 
        timeout.tv_usec = 0;

        result = select(0, NULL, &writeSet, NULL, &timeout);
        if (result > 0 && FD_ISSET(sock, &writeSet)) {
            closesocket(sock);

            
            struct servent* service = getservbyport(htons(port), "tcp");
            if (service) {
                serviceName = service->s_name; 
            }
            else {
                serviceName = "Unknown";
            }
            return true;
        }
    }

    closesocket(sock);
    return false;
}

void scanPorts(const std::string& ip, int startPort, int endPort) {
    for (int port = startPort; port <= endPort; ++port) {
        std::string serviceName;
        if (scanPort(ip, port, serviceName)) {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "Port " << port << " (" << serviceName << ") is open.\n";
        }
    }
}

int main() {
    std::string ip;
    int startPort = 1, endPort = 65535;

    std::cout << "Enter IP address to scan: ";
    std::cin >> ip;

    int threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    int portsPerThread = (endPort - startPort + 1) / threadCount;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    for (int i = 0; i < threadCount; ++i) {
        int threadStartPort = startPort + i * portsPerThread;
        int threadEndPort = (i == threadCount - 1) ? endPort : (threadStartPort + portsPerThread - 1);
        threads.emplace_back(scanPorts, ip, threadStartPort, threadEndPort);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    WSACleanup();

    std::cout << "\nScanning complete. Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();


    return 0;
}
