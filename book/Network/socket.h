#ifndef __NETWORK_HEADER__
#define __NETWORK_HEADER__
#ifdef __cplusplus
extern "C"{
#endif
#include <Uefi.h>

#define IPV4(a,b,c,d) (a | b<<8 | c << 16 | d <<24)
typedef EFI_STATUS SOCKET_STATUS;

int Socket();
SOCKET_STATUS Connect(int fd, UINT32 Ip32, UINT16 Port);
SOCKET_STATUS Send(int fd, CHAR8* Data, UINTN Lenth);
SOCKET_STATUS Recv(int fd, CHAR8* Buffer, UINTN Lenth);
SOCKET_STATUS Close(int fd);

#ifdef __cplusplus
}
#endif
#endif
