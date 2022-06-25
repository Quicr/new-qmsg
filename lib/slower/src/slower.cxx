
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include <slower.h>

float slowerVersion() {
  return 0.1;
}

int slowerSetup( SlowerConnection& slower, uint16_t port) {
  int err;
  
  slower.fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (slower.fd < 0) {
    perror("probelm opening socket");
    exit(-1);
  }

  int optionValue = 1;
  err = setsockopt(slower.fd, SOL_SOCKET, SO_REUSEPORT, (const void *)&optionValue,
                   sizeof(optionValue));
  if (err != 0) {
    perror("problem setting socket re-use option");
    exit(-2);
  }
  
  struct sockaddr_in srvAddr;
  bzero((char *)&srvAddr, sizeof(srvAddr));
  srvAddr.sin_family = AF_INET;
  srvAddr.sin_port = htons(port);
  srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  err = bind(slower.fd, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
  if (err < 0) {
    perror("problem on binding to port to listen on");
    exit(-3);
  }

  return 0;
}

