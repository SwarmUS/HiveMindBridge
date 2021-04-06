#include "../../utils/Logger.h"
#include "../../utils/TCPClient.h"
#include <chrono>
#include <hivemind-host/HiveMindHostDeserializer.h>
#include <hivemind-host/HiveMindHostSerializer.h>
#include <memory>
#include <string.h>
#include <thread>

int main(int argc, char** argv) {
    Logger logger;

    // Create a TCP socket client
    TCPClient tcpClient(8080);
    tcpClient.connect();
    HiveMindHostSerializer serializer(tcpClient);
    HiveMindHostDeserializer deserializer(tcpClient);

    // Wait for a greet message
    MessageDTO greetRequest;
    deserializer.deserializeFromStream(greetRequest);

    MessageDTO greetResponse(42, 42, GreetingDTO(42));
    serializer.serializeToStream(greetResponse);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Listen for request
    MessageDTO message;
    deserializer.deserializeFromStream(message);
    RequestDTO request = std::get<RequestDTO>(message.getMessage());
    UserCallRequestDTO userCallRequest = std::get<UserCallRequestDTO>(request.getRequest());
    FunctionCallRequestDTO functionCallRequest =
        std::get<FunctionCallRequestDTO>(userCallRequest.getRequest());
    std::string functionName = functionCallRequest.getFunctionName();

    logger.log(LogLevel::Info, "REQUEST FROM HOST (%s): \n", functionName.c_str());

    // Send ack
    GenericResponseDTO genericResponse(GenericResponseStatusDTO::Ok, "");
    ResponseDTO response(request.getId(), genericResponse);
    MessageDTO ackMessage(message.getDestinationId(), message.getSourceId(), response);
    serializer.serializeToStream(ackMessage);

    logger.log(LogLevel::Info, "SENT ACK");

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}