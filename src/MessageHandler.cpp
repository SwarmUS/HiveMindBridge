#include "hivemind-bridge/MessageHandler.h"
#include <future>

MessageHandler::MessageHandler(ILogger& logger, IUserCallRequestManager& userCallRequestManager) :
    m_logger(logger), m_userCallRequestManager(userCallRequestManager) {}

MessageHandler::~MessageHandler() {}

std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> MessageHandler::
    handleMessage(MessageDTO message) {
    uint32_t msgSourceId = message.getSourceId();
    uint32_t msgDestinationId = message.getDestinationId();

    InboundRequestHandle result;

    // Message
    auto request = message.getMessage();

    // Request
    if (std::holds_alternative<RequestDTO>(request)) {
        auto userCallRequest = std::get<RequestDTO>(request).getRequest();
        uint32_t requestId = std::get<RequestDTO>(request).getId();

        // UserCallRequest
        if (const auto* ucRequest = std::get_if<UserCallRequestDTO>(&userCallRequest)) {
            return m_userCallRequestManager.handleMessage(message, *ucRequest);
        }
        return result;
    } else if (std::holds_alternative<ResponseDTO>(request)) {
        ResponseDTO response = std::get<ResponseDTO>(request);
        auto vResponse = response.getResponse();

        if (std::holds_alternative<GenericResponseDTO>(vResponse)) {
            GenericResponseDTO genericResponse = std::get<GenericResponseDTO>(vResponse);

            return InboundResponseHandle(response.getId(), genericResponse.getStatus(),
                                         genericResponse.getDetails());
        } else if (std::holds_alternative<UserCallResponseDTO>(vResponse)) {
            UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(vResponse);

            auto vUserCallResponse = userCallResponse.getResponse();
            if (std::holds_alternative<GenericResponseDTO>(vUserCallResponse)) {
                GenericResponseDTO genericResponse =
                    std::get<GenericResponseDTO>(vUserCallResponse);

                return InboundResponseHandle(response.getId(), genericResponse.getStatus(),
                                             genericResponse.getDetails());
            } else if (std::holds_alternative<FunctionCallResponseDTO>(vUserCallResponse)) {
                FunctionCallResponseDTO functionCallResponse =
                    std::get<FunctionCallResponseDTO>(vUserCallResponse);

                return InboundResponseHandle(response.getId(),
                                             functionCallResponse.getResponse().getStatus(),
                                             functionCallResponse.getResponse().getDetails());
            } else {
                m_logger.log(LogLevel::Warn, "Cannot handle user call response : "
                                             "unknown user call response type");
                return {};
            }
        } else {
            m_logger.log(LogLevel::Warn, "Cannot handle response : unknown response type");
            return {};
        }
    } else {
        m_logger.log(LogLevel::Warn, "Cannot handle message : unknown message type");
        return {};
    }
}

std::optional<uint32_t> MessageHandler::handleGreet(MessageDTO greetMessage) {
    auto greeting = greetMessage.getMessage();

    if (std::holds_alternative<GreetingDTO>(greeting)) {
        return std::get<GreetingDTO>(greeting).getId();
    }

    return {};
}
