<p align="center">
  <img width="450" height="120" align="center" src="https://raw.githubusercontent.com/Ralex91/Rahoot/main/.github/logo.svg">
  <br>
  <img align="center" src="https://api.visitorbadge.io/api/visitors?path=https://github.com/Ralex91/Rahoot/edit/main/README.md&countColor=%2337d67a">
</p>

## TODO:

- [x] Make the system work with esp-32 clients
  - [x] ~~Remove~~ Hardcode the PIN system
  - [x] Capture and document the flow of Websocket stuff, and see what we specifically need to re-create
- [x] ESP-32 Client Software
  - [x] When it turns on, it tries to connect to the web server
    - [x] It then creates/signs in to with a predefined name
    - [x] Every ESP32 has a predefined name
      - [x] We should put these names as labels on bags, and have people scramble at the beginning to pick the names they like the most
  - [x] When a button is pushed, submit a websocket instance with a player id.
  - [x] We need to submit an event like this from the esp32:
    - ```
      socket.on("player:selectedAnswer", (answerKey) =>
        Player.selectedAnswer(gameState, io, socket, answerKey),
      )
      ```
  - [x] We need to emit something like this, where `answer` is a number '0-3'
    - ```
        socket.emit("player:selectedAnswer", answer)
      ```

## 🧩 What is this project?

This is a fork of Rahoot, intended for use with ESP32 clients.

Rahoot is a straightforward and open-source clone of the Kahoot! platform, allowing users to host it on their own server for smaller events.

> ⚠️ This project is still under development, please report any bugs or suggestions in the [issues](https://github.com/Ralex91/Rahoot/issues)

## ⚙️ Prerequisites

- Node.js version 20 or higher

## 📖 Getting Started

1.  #### Clone the GitHub repository of your project.
    ```bash
    git clone https://github.com/Ralex91/Rahoot.git
    cd ./Rahoot
    ```
2.  #### Install the dependencies using your preferred package manager

    ```bash
    npm install
    ```

    <br>
    <hr>

## 📦 Running the Application in Production Mode:

1. #### Check websocket connfiguration in [config.mjs](config.mjs)

   If you want the client to connect directly to the websocket server, edit the [config.mjs](config.mjs) file and change the localhost to your public IP address.

   ```js
   export const WEBSOCKET_PUBLIC_URL = "http://1.2.3.4:3100/"
   export const WEBSOCKET_SERVER_PORT = 3100

   // Rest of the config ...
   ```

2. #### Start the application

   ```bash
   npm run all
   ```

## ⚙️ Running the Application in Development Mode:

```bash
npm run all-dev
```

## 🔧 Configuration

Configuration can be found in [config.mjs](config.mjs)

```js
const QUIZZ_CONFIG = {
  password: "PASSWORD", // Manager password
  subject: "Adobe", // Subject of the quiz
  questions: [
    { // Example question
      question: "What is good answer ?", // Question
      answers: [ // Possible answers
        "No",
        "Yes",
        "No",
        "No",
      ],
      image:
        "https://images.unsplash.com/....", // Image URL (optional)
      solution: 1, // Index of the correct answer (index starts at 0)
      cooldown: 5, // Show question cooldown in seconds
      time: 15, // Time to answer in seconds
    },
    ...
  ],
}
```

## 🤔 How to use

- Go to [https://localhost:3000/manager](https://localhost:3000/manager) enter manager password.

- Share link [https://localhost:3000/](https://localhost:3000/) and code on manager screen with your friends and get ready to play.

- Once everyone is ready, start the game with button on the top left of the screen of manager.

## 📝 Contributing

- Create a fork

- Create work branch (Example: githubUsername/featureName).

- Commit and push your changes in the work branch.

- Open a pull request.

- Your pull request would be merged and changes will be reflected in the main repository.
