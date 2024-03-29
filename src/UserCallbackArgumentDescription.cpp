#include "hivemind-bridge/UserCallbackArgumentDescription.h"
#include <pheromones/FunctionDescriptionArgumentTypeDTO.h>
#include <string>

UserCallbackArgumentDescription::UserCallbackArgumentDescription(
    std::string name, FunctionDescriptionArgumentTypeDTO type) :
    m_name(std::move(name)), m_type(type) {}

std::string UserCallbackArgumentDescription::getName() { return m_name; }

FunctionDescriptionArgumentTypeDTO UserCallbackArgumentDescription::getType() { return m_type; }
