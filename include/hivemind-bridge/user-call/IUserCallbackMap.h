#ifndef HIVEMINDBRIDGE_IUSERCALLBACKMAP_H
#define HIVEMINDBRIDGE_IUSERCALLBACKMAP_H

class IUserCallbackMap {
  public:
    virtual bool registerCallback(std::string name,
                                  CallbackFunction callback,
                                  CallbackArgsManifest manifest) = 0;

    virtual bool registerCallback(std::string name, CallbackFunction callback) = 0;

    virtual std::optional<CallbackFunction> getCallback(const std::string& name) = 0;

    virtual uint32_t getLength() = 0;

    virtual std::optional<CallbackArgsManifest> getManifestAt(uint32_t index) = 0;

    virtual std::optional<std::string> getNameAt(uint32_t index) = 0;
};

#endif // HIVEMINDBRIDGE_IUSERCALLBACKMAP_H
