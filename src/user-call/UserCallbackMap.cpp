#include "hivemind-bridge/user-call/UserCallbackMap.h"

bool UserCallbackMap::registerCallback(std::string name,
                                       CallbackFunction callback,
                                       CallbackArgsManifest manifest) {
    bool overwriting = m_callbacks.find(name) != m_callbacks.end();

    UserCallbackFunctionWrapper cb(callback, manifest);
    m_callbacks[name] = cb;

    if (!overwriting) {
        m_callbackNames.push_back(name);
    }

    return overwriting;
}

bool UserCallbackMap::registerCallback(std::string name, CallbackFunction callback) {
    CallbackArgsManifest manifest;
    return registerCallback(name, callback, manifest);
}

std::optional<CallbackFunction> UserCallbackMap::getCallback(const std::string& name) {
    auto callback = m_callbacks.find(name);
    if (callback != m_callbacks.end()) {
        return callback->second.getFunction();
    }

    return {};
}

uint32_t UserCallbackMap::getLength() { return m_callbackNames.size(); }

std::optional<CallbackArgsManifest> UserCallbackMap::getManifestAt(uint32_t index) {
    if (index > m_callbackNames.size()) {
        return {};
    }

    // m_callbacks and m_callbackNames grow together: the latter is a lookup table
    // for the former. No need to check if the name exists in m_callbacks.
    std::string name = m_callbackNames[index];
    UserCallbackFunctionWrapper cb = m_callbacks[name];

    return cb.getManifest();
}

std::optional<std::string> UserCallbackMap::getNameAt(uint32_t index) {
    if (index > m_callbackNames.size()) {
        return {};
    }

    return m_callbackNames[index];
}