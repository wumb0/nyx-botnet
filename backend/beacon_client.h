
#define BUF_SIZE 8192

#ifdef _WIN32
    #define __windows__ //consistancy
    #define SHELL "C:\\windows\\system32\\cmd.exe"
    #define OS "Windows"
    #define ID_CMD "VER"
    #include <windows.h>
    #include <io.h>
    #include <process.h>
#elif __APPLE__
    #define __apple__ //consistancy
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        #define SHELL "/bin/bash"
        #define OS "Mac (Unix)"
        #define ID_CMD "sw_vers"
        #include <unistd.h>
    #else
    #   error "Unsupported OS"
    #endif
#elif __linux__
    #define SHELL "/bin/bash"
    #define OS "Linux"
    #define ID_CMD "cat /etc/*releases || lsb_release -a || cat /proc/version"
    #include <unistd.h>
#elif __unix__
    #define SHELL "/bin/sh"
    #define OS "Unix"
    #define ID_CMD "uname -a"
    #include <unistd.h>
#else
#   error "Unsupported OS"
#endif

char *get_os();

char *run_cmd(char *cmd, char **param);
