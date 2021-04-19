#ifndef HIVEMINDBRIDGE_IUSERCALLBACKMAP_H
#define HIVEMINDBRIDGE_IUSERCALLBACKMAP_H

class IUserCallbackMap {
  public:
    /**
     * Register a callback
     * @param name Key of the callback
     * @param callback Callback function
     * @param manifest A list describing the callback's expected arguments name and type
     * @returns True if an existing callback function was overwritten, false otherwise
     */
    virtual bool registerCallback(std::string name,
                                  CallbackFunction callback,
                                  CallbackArgsManifest manifest) = 0;

    /**
     * Register a callback
     * @param name Key of the callback
     * @param callback Callback function
     * @returns True if an existing callback function was overwritten, false otherwise
     */
    virtual bool registerCallback(std::string name, CallbackFunction callback) = 0;

    /**
     * Get an instance of a callback, if it exists.
     * @param name Key under which the callback was registered
     * @return The callback function if it exists.
     */
    virtual std::optional<CallbackFunction> getCallback(const std::string& name) = 0;

    virtual uint32_t getLength() = 0;

    virtual std::optional<CallbackArgsManifest> getManifestAt(uint32_t index) = 0;

    virtual std::optional<std::string> getNameAt(uint32_t index) = 0;
};

#endif // HIVEMINDBRIDGE_IUSERCALLBACKMAP_H
