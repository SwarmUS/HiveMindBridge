#ifndef HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H
#define HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H

#include "pheromones/HiveMindHostApiResponseDTO.h"
#include "pheromones/MessageDTO.h"

typedef struct {
    float distance;
    float azimuth;
    bool inLOS;
} Position;

#include <functional>

class IHiveMindHostApiResponseHandler {
  public:
    static constexpr uint16_t NEIGHBORS_MAX_SIZE = NEIGHBORS_LIST_SIZE;

    virtual void handleMessage(const MessageDTO& message,
                               const HiveMindHostApiResponseDTO& hmResponse) = 0;

    virtual bool onNeighborListUpdated(std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>,
                                                          uint64_t bytesLength)> callback) = 0;

    virtual bool onNeighborUpdated(
        std::function<void(uint16_t neighborId, std::optional<Position> position)> callback) = 0;
};

#endif // HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H
