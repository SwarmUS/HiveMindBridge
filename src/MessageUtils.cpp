#include "hivemind-bridge/MessageUtils.h"

MessageDTO MessageUtils::createResponseMessage(uint32_t responseId,
                                               uint32_t msgSourceId,
                                               uint32_t msgDestinationId,
                                               UserCallTargetDTO moduleDestination,
                                               GenericResponseStatusDTO status,
                                               const std::string& ackMessage) {
    FunctionCallResponseDTO functionCallResponse(status, ackMessage.c_str());
    UserCallResponseDTO userCallResponse(UserCallTargetDTO::HOST, moduleDestination,
                                         functionCallResponse);
    ResponseDTO response(responseId, userCallResponse);
    MessageDTO responseMessage(msgSourceId, msgDestinationId, response);

    return responseMessage;
}

MessageDTO MessageUtils::createFunctionListLengthResponseMessage(
    uint32_t responseId,
    uint32_t msgSourceId,
    uint32_t msgDestinationId,
    UserCallTargetDTO moduleDestination,
    uint32_t length) {

    FunctionListLengthResponseDTO functionListLengthResponse(length);
    UserCallResponseDTO userCallResponse(UserCallTargetDTO::HOST, moduleDestination,
                                         functionListLengthResponse);
    ResponseDTO response(responseId, userCallResponse);
    MessageDTO responseMessage(msgSourceId, msgDestinationId, response);

    return responseMessage;
}

MessageDTO MessageUtils::createFunctionDescriptionResponseMessage(
    uint32_t responseId,
    uint32_t msgSourceId,
    uint32_t msgDestinationId,
    UserCallTargetDTO moduleDestination,
    FunctionDescriptionDTO functionDescription) {

    FunctionDescriptionResponseDTO functionDescriptionResponse(functionDescription);
    UserCallResponseDTO userCallResponse(UserCallTargetDTO::HOST, moduleDestination,
                                         functionDescriptionResponse);
    ResponseDTO response(responseId, userCallResponse);
    MessageDTO responseMessage(msgSourceId, msgDestinationId, response);

    return responseMessage;
}

MessageDTO MessageUtils::createFunctionCallRequest(uint32_t msgSourceId,
                                                   uint32_t msgDestinationId,
                                                   uint32_t requestId,
                                                   UserCallTargetDTO moduleDestination,
                                                   const std::string& callbackName,
                                                   const CallbackArgs& args) {
    FunctionCallRequestDTO functionCallRequest(callbackName.c_str(), args.data(), args.size());

    UserCallRequestDTO userCallRequest(UserCallTargetDTO::HOST, moduleDestination,
                                       functionCallRequest);
    RequestDTO requestDTO(requestId, userCallRequest);
    MessageDTO message(msgSourceId, msgDestinationId, requestDTO);

    return message;
}

MessageDTO MessageUtils::createFunctionCallRequest(uint32_t msgSourceId,
                                                   uint32_t msgDestinationId,
                                                   uint32_t requestId,
                                                   UserCallTargetDTO moduleDestination,
                                                   const std::string& callbackName) {

    FunctionCallRequestDTO functionCallRequest(callbackName.c_str(), nullptr, 0);

    UserCallRequestDTO userCallRequest(UserCallTargetDTO::HOST, moduleDestination,
                                       functionCallRequest);
    RequestDTO requestDTO(requestId, userCallRequest);
    MessageDTO message(msgSourceId, msgDestinationId, requestDTO);

    return message;
}

std::optional<MessageDTO> MessageUtils::createBytesMessage(uint32_t msgSourceId,
                                                           uint32_t msgDestinationId,
                                                           uint32_t requestId,
                                                           uint32_t byteReqId,
                                                           uint32_t packetNumber,
                                                           bool lastPacket,
                                                           uint8_t* payload,
                                                           uint16_t payloadLength) {
    if (payloadLength > BytesDTO::PAYLOAD_MAX_SIZE) {
        return {};
    }

    BytesDTO bytes(byteReqId, packetNumber, lastPacket, payload, payloadLength);
    HiveMindHostApiRequestDTO hmReq(bytes);
    RequestDTO req(requestId, hmReq);
    return MessageDTO(msgSourceId, msgDestinationId, req);
}

MessageDTO MessageUtils::createGreetMessage() { return MessageDTO(0, 0, GreetingDTO(0)); }

uint32_t MessageUtils::generateRandomId() { return rand() % UINT32_MAX; }
