#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

enum GameStatus {
  DISCONNECTED,
  WAITING_TO_PLAY,
  PLAYING,
};


class GameLogic {

public:
  GameLogic() = delete;  // Delete the default constructor
  GameLogic(const GameLogic &) = delete;
  GameLogic &operator=(const GameLogic &) = delete;

  GameLogic(WiFiMulti &wifi, const char *host, const int port)
    : gameStatus(DISCONNECTED), wifiMulti(wifi), socketIO(), host(host), port(port) {}


  void setGameStatus(GameStatus gameStatus) {
    this->gameStatus = gameStatus;
  }

  void start() {
    connectToServer();
  }

  void loop() {
    this->socketIO.loop();
    if (this->gameStatus == WAITING_TO_PLAY) {
      tryJoinGame();
    }
  }

  GameStatus getGameStatus() {
    return this->gameStatus;
  }

private:

  void connectToServer() {
    // server address, port and URL
    this->socketIO.begin(this->host, this->port, "/socket.io/?EIO=4");

    // event handler
    this->socketIO.onEvent([this](socketIOmessageType_t type, uint8_t *payload, size_t length) {
      this->socketIOEvent(type, payload, length);
    });
  }


  void tryJoinGame() {
#define GAME_USERNAME "\"ESP32\""
#define ROOM_CODE "\"621639\""

    const char *output = "[\"player:join\", { \"username\":" GAME_USERNAME ", \"room\": " ROOM_CODE "}]";

    // Send event
    this->socketIO.send(sIOtype_EVENT, output, strlen(output));
  }

  void handleEvent(DynamicJsonDocument &doc) {
  }

  // TODO: move to cpp
  void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length) {
    switch (type) {
      case sIOtype_DISCONNECT:
        USE_SERIAL.printf("[IOc] Disconnected!\n");
        setGameStatus(DISCONNECTED);
        break;
      case sIOtype_CONNECT:
        USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);
        setGameStatus(WAITING_TO_PLAY);
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
          DynamicJsonDocument doc(2048);
          DeserializationError error = deserializeJson(doc, payload, length);
          if (error) {
            USE_SERIAL.print(F("deserializeJson() failed: "));
            USE_SERIAL.println(error.c_str());
            return;
          }

          String eventName = doc[0];
          USE_SERIAL.printf("[IOc] event name: %s\n", eventName.c_str());

          // Message Includes a ID for a ACK (callback)
          if (id) {  // https://socket.io/docs/v4/socket-io-protocol/#acknowledgement
            // create JSON message for Socket.IO (ack)
            DynamicJsonDocument docOut(1024);
            JsonArray array = docOut.to<JsonArray>();

            // add payload (parameters) for the ack (callback function)
            JsonObject param1 = array.createNestedObject();
            param1["now"] = millis();

            // JSON to String (serializion)
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



  const char *host;
  const int port;
  GameStatus gameStatus;
  SocketIOclient socketIO;
  WiFiMulti &wifiMulti;
};