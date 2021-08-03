#include "hivemind-bridge/MessageHandler.h"
#include <future>

MessageHandler::MessageHandler(ILogger& logger,
                               IUserCallRequestHandler& userCallRequestManager,
                               IHiveMindHostRequestHandler& hmRequestHandler,
                               IHiveMindHostApiResponseHandler& hmResponseHandler) :
    m_logger(logger),
    m_userCallRequestHandler(userCallRequestManager),
    m_hmRequestHandler(hmRequestHandler),
    m_hmResponseHandler(hmResponseHandler) {}

std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> MessageHandler::
    handleMessage(MessageDTO message) {
    // Message
    auto request = message.getMessage();

    // Request
    if (std::holds_alternative<RequestDTO>(request)) {
        auto userCallRequest = std::get<RequestDTO>(request).getRequest();

        if (const auto* ucRequest = std::get_if<UserCallRequestDTO>(&userCallRequest)) {
            return m_userCallRequestHandler.handleMessage(message, *ucRequest);
        }
        if (const auto* hmRequest = std::get_if<HiveMindHostApiRequestDTO>(&userCallRequest)) {
            m_hmRequestHandler.handleMessage(message, *hmRequest);
            return {};
        }
    } else if (std::holds_alternative<ResponseDTO>(request)) {
        ResponseDTO response = std::get<ResponseDTO>(request);
        auto vResponse = response.getResponse();

        if (std::holds_alternative<GenericResponseDTO>(vResponse)) {
            GenericResponseDTO genericResponse = std::get<GenericResponseDTO>(vResponse);

            return InboundResponseHandle(response.getId(), genericResponse.getStatus(),
                                         genericResponse.getDetails());
        }
        if (std::holds_alternative<UserCallResponseDTO>(vResponse)) {
            UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(vResponse);

            auto vUserCallResponse = userCallResponse.getResponse();
            if (std::holds_alternative<GenericResponseDTO>(vUserCallResponse)) {
                GenericResponseDTO genericResponse =
                    std::get<GenericResponseDTO>(vUserCallResponse);

                return InboundResponseHandle(response.getId(), genericResponse.getStatus(),
                                             genericResponse.getDetails());
            }
            if (std::holds_alternative<FunctionCallResponseDTO>(vUserCallResponse)) {
                FunctionCallResponseDTO functionCallResponse =
                    std::get<FunctionCallResponseDTO>(vUserCallResponse);

                return InboundResponseHandle(response.getId(),
                                             functionCallResponse.getResponse().getStatus(),
                                             functionCallResponse.getResponse().getDetails());
            }
            m_logger.log(LogLevel::Warn, "Cannot handle user call response : "
                                         "unknown user call response type");
            return {};
        }
        if (std::holds_alternative<HiveMindHostApiResponseDTO>(vResponse)) {
            HiveMindHostApiResponseDTO hmResponse = std::get<HiveMindHostApiResponseDTO>(vResponse);

            m_hmResponseHandler.handleMessage(message, hmResponse);

            return {};
        }
    }
    m_logger.log(LogLevel::Warn, "Cannot handle message : unknown message type");
    return {};
}

std::optional<uint32_t> MessageHandler::handleGreet(MessageDTO greetMessage) {
    auto greeting = greetMessage.getMessage();

    if (std::holds_alternative<GreetingDTO>(greeting)) {
        return std::get<GreetingDTO>(greeting).getId();
    }

    return {};
}
