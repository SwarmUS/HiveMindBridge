#ifndef HIVEMINDBRIDGE_IHIVEMINDHOSTAPIREQUESTHANDLER_H
#define HIVEMINDBRIDGE_IHIVEMINDHOSTAPIREQUESTHANDLER_H

#include "pheromones/HiveMindHostApiRequestDTO.h"
#include "pheromones/MessageDTO.h"
#include <functional>

class IHiveMindHostRequestHandler {
  public:
    /**
     * Handle a HiveMindHostRequest message type
     * @param message The encloising message
     * @param hmRequest The HiveMindHostApiRequest enclosed in the message
     */
    virtual void handleMessage(const MessageDTO& message,
                               const HiveMindHostApiRequestDTO& hmRequest) = 0;

    /**
     * Register a user-defined callback to be incoked when the reception of a bytes payload is
     * completed.
     * @param callback The user-defined callback to be invoked.
     * @return True if the callback was ovewritten, false otherwise.
     */
    virtual bool onBytesReceived(
        std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) = 0;
};

#endif // HIVEMINDBRIDGE_IHIVEMINDHOSTAPIREQUESTHANDLER_H
