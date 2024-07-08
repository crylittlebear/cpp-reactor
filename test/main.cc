#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "TcpServer.h"
#include "Logger.h"

int main() {
    unsigned short port = 9999;
    chdir("/home/heng/Res");

    TcpServer server(12, port);
    server.start();
    return 0;
}