#pragma once
#define USE_SERIAL Serial

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

enum GameStatus {
  /**
     * Likely connected to WiFi, disconnected from Socket.IO
     */
  DISCONNECTED,
  /**
     * Connected to Socket.IO, but the room has not started
     */
  WAITING_FOR_ROOM,
  /**
     * We've joined the room, and we're waiting for the
     * game to start.
     */
  WAITING_TO_START,
  /**
     * We're playing the game.
     */
  PLAYING,
};


/**
 * GameLogic is structured like a state machine.
 * The states are primarily updated by the server, communicated via
 * GameLogic::socketIOEvent and GameLogic::updateGameState. Then, the
 * game acts upon the current state through the GameLogic::loop function.
 */
class GameLogic {

public:
  GameLogic() = delete;  // Delete the default constructor
  GameLogic(const GameLogic &) = delete;

  GameLogic &operator=(const GameLogic &) = delete;

  GameLogic(WiFiMulti &wifi, const char *host, const int port, const String gameUsername, const String roomCode)
    : gameStatus(DISCONNECTED), wifiMulti(wifi), socketIO(), host(host), port(port),
      timeNextMessageIsAllowed(0), gameUsername(gameUsername), roomCode(roomCode) {}

  void setGameStatus(GameStatus gameStatus);

  GameStatus getGameStatus();

  void start();

  void loop();

  void trySubmitAnswer(uint8_t answerChoice);

private:

  void connectToServer();

  void tryJoinGame();
  /**
     * This updates the game state to match the server state.
     * Note: This only uses the event name at the moment.
     */
  void updateGameState(DynamicJsonDocument &document);

  void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length);

  const String gameUsername;
  const String roomCode;
  const char *host;
  const int port;
  GameStatus gameStatus;
  SocketIOclient socketIO;
  WiFiMulti &wifiMulti;
  unsigned long timeNextMessageIsAllowed;
  const unsigned long SHORT_TIMEOUT_MS = 1000;   // in miliseconds
  const unsigned long MEDIUM_TIMEOUT_MS = 5000;  // in miliseconds
};
