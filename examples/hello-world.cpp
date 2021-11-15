#include "hivemind-bridge/HiveMindBridge.h"
#include "BasicLogger.h"

int main(int argc, char** argv) {
    int port = 7001;
    Logger logger;

    HiveMindBridge bridge(port, logger);

    // Register a "hello world" custom action"
    CallbackFunction helloWorldCallback = [&](CallbackArgs args) -> std::optional<CallbackReturn> {
        logger.log(LogLevel::Info, "Hello, I am C-3PO, human-cyborg relations.");

        return {}; // This function does not return any payload, so we return an empty struct
    };

    // Register the function's name and pointer in our `HiveMindBridge` object. This function will
    // then be discoverable by anyone in the swarm.
    bridge.registerCustomAction("helloWorld", helloWorldCallback);

    // Register event hooks. HiveMindBridge provides a few event hooks that allow
    // the user to control the execution flow.
    bridge.onConnect([&]() { logger.log(LogLevel::Info, "Client connected."); });

    bridge.onDisconnect([&]() { logger.log(LogLevel::Info, "Client disconnected."); });

    // The main application loop
    while (true) {
        bridge.spin();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}