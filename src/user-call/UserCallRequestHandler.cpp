#include "hivemind-bridge/user-call/UserCallRequestHandler.h"

UserCallRequestHandler::UserCallRequestHandler(ILogger& logger, IUserCallbackMap& callbackMap) :
    m_logger(logger), m_callbackMap(callbackMap) {}

std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> UserCallRequestHandler::
    handleMessage(MessageDTO message, UserCallRequestDTO ucRequest) {
    InboundRequestHandle result;

    if (std::holds_alternative<FunctionListLengthRequestDTO>(ucRequest.getRequest())) {
        result.setResponse(handleFunctionListLengthRequest(message, ucRequest));
    } else if (std::holds_alternative<FunctionDescriptionRequestDTO>(ucRequest.getRequest())) {
        result.setResponse(handleFunctionDescriptionRequest(message, ucRequest));
    } else if (std::holds_alternative<FunctionCallRequestDTO>(ucRequest.getRequest())) {
        handleFunctionCallRequest(message, ucRequest, &result);
    } else {
        result.setResponse(handleUnknownUserCallRequest(message, ucRequest));
    }

    return result;
}

MessageDTO UserCallRequestHandler::handleFunctionListLengthRequest(MessageDTO message,
                                                                   UserCallRequestDTO ucRequest) {
    uint32_t length = m_callbackMap.getLength();

    uint32_t msgSourceId = message.getSourceId();
    uint32_t msgDestinationId = message.getDestinationId();

    auto request = message.getMessage();
    uint32_t requestId = std::get<RequestDTO>(request).getId();

    UserCallTargetDTO sourceModule = ucRequest.getDestination();

    return MessageUtils::createFunctionListLengthResponseMessage(requestId, msgDestinationId,
                                                                 msgSourceId, sourceModule, length);
}

MessageDTO UserCallRequestHandler::handleFunctionDescriptionRequest(MessageDTO message,
                                                                    UserCallRequestDTO ucRequest) {
    uint32_t msgSourceId = message.getSourceId();
    uint32_t msgDestinationId = message.getDestinationId();

    auto request = message.getMessage();
    uint32_t requestId = std::get<RequestDTO>(request).getId();

    UserCallTargetDTO sourceModule = ucRequest.getDestination();
    FunctionDescriptionRequestDTO fdRequest =
        std::get<FunctionDescriptionRequestDTO>(ucRequest.getRequest());

    uint32_t index = fdRequest.getIndex();

    if (auto manifest = m_callbackMap.getManifestAt(index)) {
        std::string name = m_callbackMap.getNameAt(index).value();
        std::vector<FunctionDescriptionArgumentDTO> args;

        for (auto arg : manifest.value()) {
            FunctionDescriptionArgumentDTO argument(arg.getName().c_str(), arg.getType());
            args.push_back(argument);
        }

        FunctionDescriptionDTO functionDescription(name.c_str(), args.data(), args.size());

        return MessageUtils::createFunctionDescriptionResponseMessage(
            requestId, msgDestinationId, msgSourceId, sourceModule, functionDescription);
    }

    return MessageUtils::createResponseMessage(requestId, msgDestinationId, msgSourceId,
                                               sourceModule, GenericResponseStatusDTO::BadRequest,
                                               "Index out of bounds.");
}

void UserCallRequestHandler::handleFunctionCallRequest(MessageDTO message,
                                                       UserCallRequestDTO ucRequest,
                                                       InboundRequestHandle* result) {
    uint32_t msgSourceId = message.getSourceId();
    uint32_t msgDestinationId = message.getDestinationId();

    auto request = message.getMessage();
    uint32_t requestId = std::get<RequestDTO>(request).getId();

    UserCallTargetDTO sourceModule = ucRequest.getSource();

    FunctionCallRequestDTO function = std::get<FunctionCallRequestDTO>(ucRequest.getRequest());
    std::string functionName = function.getFunctionName();
    CallbackArgs functionArgs = function.getArguments();

    uint16_t argsLength = function.getArgumentsLength();

    auto callback = m_callbackMap.getCallback(functionName);

    // Call the right callback
    GenericResponseStatusDTO responseStatus;
    if (callback) {
        std::shared_future<std::optional<CallbackReturn>> ret =
            std::async(std::launch::async, callback.value(), functionArgs, argsLength).share();
        result->setCallbackReturnContext(ret);
        result->setCallbackName(functionName);
        result->setMessageSourceId(msgSourceId);
        result->setMessageDestinationId(msgDestinationId);
        result->setSourceModule(sourceModule);

        responseStatus = GenericResponseStatusDTO::Ok;
    } else {
        responseStatus = GenericResponseStatusDTO::Unknown;
        m_logger.log(LogLevel::Warn, "Function name \"%s\" was not registered as a callback",
                     functionName.c_str());
    }

    result->setResponse(MessageUtils::createResponseMessage(
        requestId, msgDestinationId, msgSourceId, sourceModule, responseStatus, ""));
}

MessageDTO UserCallRequestHandler::handleUnknownUserCallRequest(MessageDTO message,
                                                                UserCallRequestDTO ucRequest) {
    m_logger.log(LogLevel::Error, "Unknown UserCallRequest");

    uint32_t msgSourceId = message.getSourceId();
    uint32_t msgDestinationId = message.getDestinationId();

    auto request = message.getMessage();
    uint32_t requestId = std::get<RequestDTO>(request).getId();

    UserCallTargetDTO sourceModule = ucRequest.getDestination();

    GenericResponseStatusDTO responseStatus = GenericResponseStatusDTO::Unknown;
    return MessageUtils::createResponseMessage(requestId, msgDestinationId, msgSourceId,
                                               sourceModule, responseStatus,
                                               "Unknown UserCallRequest");
}