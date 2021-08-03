#ifndef HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H
#define HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H

#include "pheromones/HiveMindHostApiResponseDTO.h"
#include "pheromones/MessageDTO.h"
#include <functional>

class IHiveMindHostApiResponseHandler {
  public:
    static constexpr uint16_t NEIGHBORS_MAX_SIZE = NEIGHBORS_LIST_SIZE;

    // TODO document this
    virtual void handleMessage(const MessageDTO& message,
                               const HiveMindHostApiResponseDTO& hmResponse) = 0;

    // TODO document this
    virtual bool onNeighborListUpdated(
        std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)> callback) = 0;

    // TODO document this
    virtual bool onNeighborUpdated(
        std::function<void(uint16_t neighborId, float distance, float relativeOrientation, bool inLOS)> callback) = 0;
};

#endif // HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H
