#ifndef HIVEMIND_BRIDGE_IHIVEMINDBRIDGE_H
#define HIVEMIND_BRIDGE_IHIVEMINDBRIDGE_H

#include "IMessageHandler.h"
#include "pheromones/PheromonesSettings.h"
#include <string.h>

/**
 * The HiveMindBridge is the main entrypoint to the bridge's library. It includes everything
 * necessary to define a custom swarm API and provides some hooks to control the robot when some
 * usual events occur, such as connection or disconnection of a HiveMind.
 */
class IHiveMindBridge {
  public:
    static constexpr uint16_t NEIGHBORS_MAX_SIZE = NEIGHBORS_LIST_SIZE;

    /**
     * Spin the bridge's applicative loop
     */
    virtual void spin() = 0;

    /**
     * Register a callback to be run when a TCP connection is established with a client HiveMind
     * @param callback The function to be run
     */
    virtual void onConnect(std::function<void()> callback) = 0;

    /**
     * Register a callback to be run as soon as the TCP Server notices that the connection was lost.
     * @param callback The function to be run
     */
    virtual void onDisconnect(std::function<void()> callback) = 0;

    /**
     * Register a callback to be run upon reception of an arbitrary bytes payload. The callback
     * only fires when the whole payload was received.
     * @param callback The callback to be called. This function takes a pointer to the bytes
     * payload.
     * @return True if an existing callback function was overwritten, false otherwise
     */
    virtual bool onBytesReceived(
        std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) = 0;

    // TODO document this
    virtual bool onNeighborListUpdated(
        std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)> callback) = 0;

    // TODO document
    virtual bool onNeighborUpdated(
        std::function<void(uint16_t neighborId, float distance, float relativeOrientation, bool inLOS)> callback) = 0;

    /**
     * Register a custom action that this robot can accomplish. This is meant to be used with
     * functions that require arguments (specified in manifest). With void functions, use
     * registerCustomAction(string, CallbackFunction).
     * @param name The name of the action (must match across the swarm components)
     * @param callback The function to be run. This is where the custom robot behaviour is meant to
     * be defined.
     * @param manifest A list describing the callback's expected arguments name and type
     * @return True if an existing callback function was overwritten, false otherwise
     */
    virtual bool registerCustomAction(std::string name,
                                      CallbackFunction callback,
                                      CallbackArgsManifest manifest) = 0;

    /**
     * Register a custom action that this robot can accomplish.This is meant to be used with
     * functions that do NOT require arguments. With functions requiring arguments, use
     * registerCustomAction(string, CallbackFunction, manifest).
     * @param name The name of the action (must match across the swarm components)
     * @param callback The function to be run. This is where the custom robot behaviour is meant to
     * be defined.
     * @return True if an existing callback function was overwritten, false otherwise
     */
    virtual bool registerCustomAction(std::string name, CallbackFunction callback) = 0;

    /**
     * Send a message asynchronously
     * @param message The message to send
     * @return true if the operation succeded.
     */
    virtual bool queueAndSend(MessageDTO message) = 0;

    /**
     * Send an array of bytes asynchronously
     * @param destinationId The Agent to which the data is to be sent
     * @param payload An array of bytes
     * @param payloadSize The length of the array of bytes
     * @return
     */
    virtual bool sendBytes(uint32_t destinationId,
                           const uint8_t* const payload,
                           uint16_t payloadSize) = 0;
};

#endif // HIVEMIND_BRIDGE_IHIVEMINDBRIDGE_H
