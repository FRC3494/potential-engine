#include "networker.h"

bool isValidIP(const char* address) {
    struct sockaddr_in i; // throwaway value
    int result = inet_pton(AF_INET, address, &i);
    return (result == 1);
}
