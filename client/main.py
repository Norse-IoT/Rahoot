#!/usr/bin/env python

# pip install "python-socketio[client]"

import socketio

sio = socketio.Client()


@sio.event
def connect():
    print("connection established")


@sio.on("*")
def catch_all(event, *args):
    print(f"Received event: {event}, with data: {args}")


@sio.event
def disconnect():
    print("disconnected from server")


def background_task():
    print("background task")

    while True:
        answer = input("What choice do you want? (0-3)")
        sio.emit("player:selectedAnswer", int(answer))


ROOM_NUMBER = 621639

sio.connect("http://localhost:5505")
sio.emit("player:join", {"username": "python", "room": str(ROOM_NUMBER)})
sio.start_background_task(background_task)
sio.wait()
