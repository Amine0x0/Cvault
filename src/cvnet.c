#include "../include/cvault.h"

#define MAX_PORTS 1024

const char *testdns = "8.8.8.8";

static void cv_get_private_ip(char *dest, size_t max_len) {
    struct ifaddrs *ifaddr, *ifa;
    int family;

    if (getifaddrs(&ifaddr) == -1)
        return;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            char *ip = inet_ntoa(sa->sin_addr);
            if (strcmp(ip, "127.0.0.1") != 0) {
                snprintf(dest, max_len, "%s", ip);
                break;
            }
        }
    }
    freeifaddrs(ifaddr);
}

static void cv_get_public_ip(char *dest, size_t max_len) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return;

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(testdns);
    serv.sin_port = htons(53);

    if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) == 0) {
        struct sockaddr_in name;
        socklen_t namelen = sizeof(name);
        if (getsockname(sock, (struct sockaddr *)&name, &namelen) == 0) {
            inet_ntop(AF_INET, &name.sin_addr, dest, max_len);
        }
    }
    close(sock);
}

static void cv_scan_common_ports(t_netcontext *ctx) {
    int common_ports[] = {21, 22, 23, 25, 53, 80, 110, 443, 8080, 3306, 5432};
    int num_ports = sizeof(common_ports) / sizeof(common_ports[0]);
    
    ctx->port_count = 0;
    for (int i = 0; i < num_ports; i++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            continue;

        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(common_ports[i]);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

        connect(sock, (struct sockaddr *)&addr, sizeof(addr));

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(sock, &fdset);
        struct timeval tv = {0, 50000};

        if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
            int sockerr;
            socklen_t len = sizeof(sockerr);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, &sockerr, &len);
            if (sockerr == 0 && ctx->port_count < 64) {
                ctx->open_ports[ctx->port_count++] = common_ports[i];
            }
        }
        close(sock);
    }
}

t_netcontext *cv_netrecon(void) {
    t_netcontext *ctx = calloc(1, sizeof(t_netcontext));
    if (!ctx)
        return NULL;

    cv_get_private_ip(ctx->private_ip, sizeof(ctx->private_ip));
    cv_get_public_ip(ctx->public_ip, sizeof(ctx->public_ip));
    cv_scan_common_ports(ctx);

    return ctx;
}
