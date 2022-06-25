#ifndef SLOWER_H
#define SLOWER_H

#include <cstdint>

float slowerVersion(); 


typedef struct {
    int fd;  // UDP socket
} SlowerConnection;

int slowerSetup( SlowerConnection& slower, uint16_t port);


#endif  // SLOWER_H
