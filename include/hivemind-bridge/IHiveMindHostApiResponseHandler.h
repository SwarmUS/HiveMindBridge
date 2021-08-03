#ifndef HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H
#define HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H

#include "pheromones/HiveMindHostApiResponseDTO.h"
#include "pheromones/MessageDTO.h"

class IHiveMindHostApiResponseHandler {
  public:

    // TODO document this
    virtual void handleMessage(const MessageDTO& message,
                               const HiveMindHostApiResponseDTO& hmResponse) = 0;
};

#endif // HIVEMINDBRIDGE_IHIVEMINDHOSTAPIRESPONSEHANDLER_H
