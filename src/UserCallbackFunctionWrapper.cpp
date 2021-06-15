#include "hivemind-bridge/UserCallbackFunctionWrapper.h"

UserCallbackFunctionWrapper::UserCallbackFunctionWrapper(CallbackFunction function,
                                                         CallbackArgsManifest manifest) :
    m_function(std::move(function)), m_manifest(std::move(manifest)) {}

CallbackFunction UserCallbackFunctionWrapper::getFunction() { return m_function; }

CallbackArgsManifest UserCallbackFunctionWrapper::getManifest() { return m_manifest; }
