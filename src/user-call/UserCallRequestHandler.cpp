#include "hivemind-bridge/UserCallRequestHandler.h"

UserCallRequestHandler::UserCallRequestHandler(ILogger& logger, IUserCallbackMap& callbackMap) :
    m_logger(logger), m_callbackMap(callbackMap) {}

std::variant<std::monostate, InboundRequestHandle, InboundResponseHandle> UserCallRequestHandler::
    handleMessage(const MessageDTO& message, const UserCallRequestDTO& ucRequest) {
    InboundRequestHandle result;

    uint32_t msgSourceId = message.getSourceId();
    uint32_t msgDestinationId = message.getDestinationId();

    auto request = message.getMessage();
    uint32_t requestId = std::get<RequestDTO>(request).getId();

    UserCallTargetDTO sourceModule = ucRequest.getSource();

    std::unique_ptr<UserCallResponseDTO> ucResponse;
    if (const auto* fcRequest =
            std::get_if<FunctionListLengthRequestDTO>(&ucRequest.getRequest())) {
        ucResponse = std::make_unique<UserCallResponseDTO>(
            UserCallTargetDTO::HOST, sourceModule,
            handleFunctionListLengthRequest(message, *fcRequest));
    } else if (const auto* fcRequest =
                   std::get_if<FunctionDescriptionRequestDTO>(&ucRequest.getRequest())) {
        ucResponse = std::make_unique<UserCallResponseDTO>(
            UserCallTargetDTO::HOST, sourceModule,
            handleFunctionDescriptionRequest(message, *fcRequest));
    } else if (const auto* fcRequest =
                   std::get_if<FunctionCallRequestDTO>(&ucRequest.getRequest())) {
        ucResponse = std::make_unique<UserCallResponseDTO>(
            UserCallTargetDTO::HOST, sourceModule,
            handleFunctionCallRequest(message, *fcRequest, &result));

        result.setMessageSourceId(msgSourceId);
        result.setMessageDestinationId(msgDestinationId);
        result.setSourceModule(sourceModule);
    } else {
        ucResponse = std::make_unique<UserCallResponseDTO>(
            UserCallTargetDTO::HOST, sourceModule,
            GenericResponseDTO(GenericResponseStatusDTO::Unknown, "Unknown UserCallRequest"));
    }

    MessageDTO responseMessage(msgDestinationId, msgSourceId, ResponseDTO(requestId, *ucResponse));
    result.setResponse(responseMessage);

    return result;
}

FunctionListLengthResponseDTO UserCallRequestHandler::handleFunctionListLengthRequest(
    MessageDTO message, FunctionListLengthRequestDTO fcRequest) {
    (void)message;
    (void)fcRequest;

    return FunctionListLengthResponseDTO(m_callbackMap.getLength());
}

FunctionDescriptionResponseDTO UserCallRequestHandler::handleFunctionDescriptionRequest(
    MessageDTO message, FunctionDescriptionRequestDTO fcRequest) {
    (void)message;

    uint32_t index = fcRequest.getIndex();

    if (auto manifest = m_callbackMap.getManifestAt(index)) {
        std::string name = m_callbackMap.getNameAt(index).value();
        std::vector<FunctionDescriptionArgumentDTO> args;

        for (auto arg : manifest.value()) {
            FunctionDescriptionArgumentDTO argument(arg.getName().c_str(), arg.getType());
            args.push_back(argument);
        }

        FunctionDescriptionDTO functionDescription(name.c_str(), args.data(), args.size());

        return FunctionDescriptionResponseDTO(functionDescription);
    }

    return FunctionDescriptionResponseDTO(
        GenericResponseDTO(GenericResponseStatusDTO::BadRequest, "Index out of bounds."));
}

FunctionCallResponseDTO UserCallRequestHandler::handleFunctionCallRequest(
    MessageDTO message, FunctionCallRequestDTO fcRequest, InboundRequestHandle* result) {
    (void)message;

    std::string functionName = fcRequest.getFunctionName();
    CallbackArgs functionArgs = fcRequest.getArguments();

    uint16_t argsLength = fcRequest.getArgumentsLength();

    if (m_callbackMap.getCallback(functionName)) {

        std::shared_future<std::optional<CallbackReturn>> ret =
            std::async(std::launch::async, this->callbackWrapper, this, functionArgs, argsLength,
                       functionName)
                .share();

        result->setCallbackReturnContext(ret);
        result->setCallbackName(functionName);

        return FunctionCallResponseDTO(GenericResponseStatusDTO::Ok, "");
    }

    m_logger.log(LogLevel::Warn, "Function name \"%s\" was not registered as a callback",
                 functionName.c_str());

    return FunctionCallResponseDTO(GenericResponseStatusDTO::BadRequest, "Unknown function.");
}

std::optional<CallbackReturn> UserCallRequestHandler::callbackWrapper(UserCallRequestHandler* _this,
                                                                      CallbackArgs args,
                                                                      uint16_t argsLenght,
                                                                      std::string functionName) {
    try {
        auto callback = _this->m_callbackMap.getCallback(functionName);
        if (callback) {
            return callback.value()(args, argsLenght);
        }
        _this->m_logger.log(LogLevel::Warn, "Function name \"%s\" was not registered as a callback",
                            functionName.c_str());
        return {};

    } catch (const std::exception& ex) {
        _this->m_logger.log(LogLevel::Warn, "Callback %s has thrown an exception: %s",
                            functionName.c_str(), ex.what());
    } catch (...) {
        _this->m_logger.log(LogLevel::Warn, "Callback %s has thrown an unknown exception",
                            functionName.c_str());
    }
    return {};
}