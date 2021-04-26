#ifndef HIVEMINDBRIDGE_USERCALLBACKMAP_H
#define HIVEMINDBRIDGE_USERCALLBACKMAP_H

#include "UserCallRequestHandler.h"
#include "IUserCallbackMap.h"

class UserCallbackMap : public IUserCallbackMap {

  public:
    bool registerCallback(std::string name,
                          CallbackFunction callback,
                          CallbackArgsManifest manifest) override;

    bool registerCallback(std::string name, CallbackFunction callback) override;

    std::optional<CallbackFunction> getCallback(const std::string& name) override;

    uint32_t getLength() override;

    std::optional<CallbackArgsManifest> getManifestAt(uint32_t index) override;

    std::optional<std::string> getNameAt(uint32_t index) override;

  private:
    CallbackMap m_callbacks;
    std::vector<std::string> m_callbackNames; // Association between callbacks' names and their id
};

#endif // HIVEMINDBRIDGE_USERCALLBACKMAP_H
