#ifndef HIVEMINDBRIDGE_USERCALLBACKMAPINTERFACEMOCK_H
#define HIVEMINDBRIDGE_USERCALLBACKMAPINTERFACEMOCK_H

#include "hivemind-bridge/user-call/IUserCallbackMap.h"
#include <gmock/gmock.h>

class UserCallbackMapInterfaceMock : public IUserCallbackMap {
  public:
    ~UserCallbackMapInterfaceMock() = default;

    MOCK_METHOD(bool,
                registerCallback,
                (std::string name,
                 CallbackFunction callback,

                 CallbackArgsManifest manifest));
    MOCK_METHOD(bool,
                registerCallback,
                (std::string name,

                 CallbackFunction callback));

    MOCK_METHOD(std::optional<CallbackFunction>, getCallback, (const std::string& name));

    MOCK_METHOD(uint32_t, getLength, ());

    MOCK_METHOD(std::optional<CallbackArgsManifest>, getManifestAt, (uint32_t index));

    MOCK_METHOD(std::optional<std::string>, getNameAt, (uint32_t index));
};

#endif // HIVEMINDBRIDGE_USERCALLBACKMAPINTERFACEMOCK_H
