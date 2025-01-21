#include "GameLogic.h"


void GameLogic::setGameStatus(GameStatus gameStatus) {
  this->gameStatus = gameStatus;
}

GameStatus GameLogic::getGameStatus() {
  return this->gameStatus;
}

void GameLogic::start() {
  connectToServer();
}

void GameLogic::loop() {
  this->socketIO.loop();  // handle received events

  if (millis() < this->timeNextMessageIsAllowed) {
    return;
  }

  // send events:

  if (this->gameStatus == WAITING_FOR_ROOM) {
    tryJoinGame();
  }
}

void GameLogic::trySubmitAnswer(uint8_t answerChoice) {
  if (answerChoice > 3) {
    USE_SERIAL.println("invalid answer choice");
    return;  // invalid input
  }

  if (this->gameStatus != PLAYING) {
    USE_SERIAL.printf("chose %d but gameStatus is not PLAYING\n", answerChoice);
    return;  // can only submit while playing
  }

  DynamicJsonDocument payload(1024);
  JsonArray array = payload.to<JsonArray>();
  array.add("player:selectedAnswer");
  array.add(answerChoice);

  // JSON to String (serializion)
  String output;
  serializeJson(payload, output);

  // Send event
  this->socketIO.sendEVENT(output);
}

void GameLogic::connectToServer() {
  // server address, port and URL
  this->socketIO.begin(this->host, this->port, "/socket.io/?EIO=4");

  // event handler
  this->socketIO.onEvent([this](socketIOmessageType_t type, uint8_t *payload, size_t length) {
    this->socketIOEvent(type, payload, length);
  });
}

void GameLogic::tryJoinGame() {
  const String output =
    "[\"player:join\", { \"username\":\"" + this->gameUsername + "\", \"room\":\"" + this->roomCode + "\"}]";

  // Send event
  this->socketIO.sendEVENT(output.c_str(), output.length());
  this->timeNextMessageIsAllowed = millis() + SHORT_TIMEOUT_MS;
}

/**
 * This updates the game state to match the server state.
 * Note: This only uses the event name at the moment.
 */
void GameLogic::updateGameState(DynamicJsonDocument &document) {
  String eventName = document[0];
  if (eventName == "game:errorMessage") {
    if (this->gameStatus == WAITING_FOR_ROOM) {
      this->timeNextMessageIsAllowed = millis() + MEDIUM_TIMEOUT_MS;
    }
  } else if (eventName == "game:successJoin") {
    this->gameStatus = WAITING_TO_START;
  } else if (eventName == "game:reset") {
    this->gameStatus = WAITING_FOR_ROOM;
  } else if (eventName == "game:status" || eventName == "game:cooldown") {
    this->gameStatus = PLAYING;
  }
}

void GameLogic::socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
      USE_SERIAL.printf("[IOc] Disconnected!\n");
      setGameStatus(DISCONNECTED);
      break;
    case sIOtype_CONNECT:
      USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);
      setGameStatus(WAITING_FOR_ROOM);
      // join default namespace (no auto join in Socket.IO V3)
      this->socketIO.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT:
      {
        char *sptr = NULL;
        int id = strtol((char *)payload, &sptr, 10);
        USE_SERIAL.printf("[IOc] get event: %s id: %d\n", payload, id);
        if (id) {
          payload = (uint8_t *)sptr;
        }
        USE_SERIAL.printf("[IOc] recieved payload: %s\n", payload);
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload, length);
        if (error) {
          USE_SERIAL.print(F("deserializeJson() failed: "));
          USE_SERIAL.println(error.c_str());
          return;
        }

        String eventName = doc[0];
        USE_SERIAL.printf("[IOc] event name: %s\n", eventName.c_str());
        updateGameState(doc);

        // Message Includes a ID for a ACK (callback)
        if (id) {  // https://socket.io/docs/v4/socket-io-protocol/#acknowledgement
          // create JSON message for Socket.IO (ack)
          DynamicJsonDocument docOut(1024);
          JsonArray array = docOut.to<JsonArray>();

          // add payload (parameters) for the ack (callback function)
          JsonObject param1 = array.createNestedObject();
          param1["now"] = millis();

          // JSON to String (serialization)
          String output;
          output += id;
          serializeJson(docOut, output);

          // Send event
          this->socketIO.send(sIOtype_ACK, output);
        }
      }
      break;
    case sIOtype_ACK:
      USE_SERIAL.printf("[IOc] get ack: %u\n", length);
      break;
    case sIOtype_ERROR:
      USE_SERIAL.printf("[IOc] get error: %u\n", length);
      break;
    case sIOtype_BINARY_EVENT:
      USE_SERIAL.printf("[IOc] get binary: %u\n", length);
      break;
    case sIOtype_BINARY_ACK:
      USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
      break;
  }
}
