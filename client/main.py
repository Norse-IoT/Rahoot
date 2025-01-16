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


ROOM_NUMBER = 621639

sio.connect("http://localhost:5505")
sio.emit("player:join", {"username": "python", "room": str(ROOM_NUMBER)})
sio.wait()
