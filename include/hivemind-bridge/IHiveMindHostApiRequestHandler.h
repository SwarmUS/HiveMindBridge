#ifndef HIVEMINDBRIDGE_IHIVEMINDHOSTAPIREQUESTHANDLER_H
#define HIVEMINDBRIDGE_IHIVEMINDHOSTAPIREQUESTHANDLER_H

#include "pheromones/HiveMindHostApiRequestDTO.h"
#include "pheromones/MessageDTO.h"
#include <functional>

class IHiveMindHostRequestHandler {
  public:
    virtual void handleMessage(const MessageDTO& message,
                               const HiveMindHostApiRequestDTO& hmRequest) = 0;

    virtual bool onBytesReceived(
        std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) = 0;
};

#endif // HIVEMINDBRIDGE_IHIVEMINDHOSTAPIREQUESTHANDLER_H
