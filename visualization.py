import threading
from copy import copy

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import sysv_ipc
import json
import random

FIFO_KEY = "."
PROJECT_ID = "a"
TARGET_MESSAGE_TYPE = 3
need_update = True

place_position = {
    0: (None, None),
    1: ((0.30, 0.40), (0.5, 0.9)),
    2: ((0.30, 0.40), (0.5, 0.9)),
    3: ((1.2, 1.4), (0.9, 1.2)),
    4: (None, None),
    5: ((0.55, 1.05), (0.85, 1.05)),
    6: (None, None),
    7: ((0.55, 0.75), (0.55, 0.75)),
    8: (None, None),
    9: ((0.85, 1.05), (0.55, 0.75)),
    10: (None, None),
}


class Fan:
    status: str
    fan_pid: int
    queued_process_pid: int | None = None
    children_count: int = 0
    control_number: int = 0
    _position: tuple[float, float] = (-2, -2)

    def __init__(self, fan_pid):
        self.fan_pid = fan_pid
        self.status = "NEW"
        self.color = random.choice(["red", "blue", "green", "yellow", "orange"])

    def process_message(self, message: dict):
        print(f"Processing message: {message}")
        self.queued_process_pid = None
        match message.get("type"):
            case "join_to_queue":
                self.status = "IN_QUEUE"
            case "set_queued_process_pid":
                self.queued_process_pid = message.get("queued_process_pid")
            case "check_fan":
                self.status = "IN_CONTROL"
                self.children_count = message.get("children_count")
                self.control_number = message.get("control_number")
                self._position = random.uniform(0.35, 0.45), random.uniform(0.95 - self.control_number * 0.1, 1 - self.control_number * 0.1)
                print(f"Fan {self.fan_pid} is in control number {self.control_number} in place {self._position}.")
            case "moving":
                self.status = "MOVING"
                self.place_from = message.get("from")
                self.place_to = message.get("to")
                position_x_1 = sum(place_position[self.place_from][0])/2 if place_position[self.place_from][0] else None
                position_x_2 = sum(place_position[self.place_to][0])/2 if place_position[self.place_to][1] else None
                position_y_1 = sum(place_position[self.place_from][1])/2 if place_position[self.place_from][1] else None
                position_y_2 = sum(place_position[self.place_to][1])/2 if place_position[self.place_to][1] else None
                if position_x_1 and position_x_2 and position_y_1 and position_y_2:
                    self._position = (position_x_1 + position_x_2) / 2, (position_y_1 + position_y_2) / 2
            case "moved":
                self.status = "MOVED"
                self.place = message.get("place")
                if self.place in place_position and place_position[self.place][0] and place_position[self.place][1]:
                    self._position =  random.uniform(*place_position[self.place][0]), random.uniform(*place_position[self.place][1])
            case _:
                print("Unknown message type.")

    def draw(self, ax):
        scatter = ax.scatter(self.position[0], self.position[1], color=self.color, label=f"Fan {self.fan_pid}")
        tooltip = f"Fan PID: {self.fan_pid}\nStatus: {self.status}\nQueued Process PID: {self.queued_process_pid}\nChildren Count: {self.children_count}\nControl Number: {self.control_number}\nPosition: {self._position}"
        annot = ax.annotate(tooltip, xy=(self.position[0], self.position[1]), xycoords='data', fontsize=6, ha='center',
                            va='center', color='black',
                            bbox=dict(boxstyle="round", fc="w", ec="gray", alpha=0.8))
        annot.set_visible(False)

        def update_annot(event):
            if scatter.contains(event)[0]:
                annot.xy = (self.position[0], self.position[1])
                annot.set_visible(True)
                ax.figure.canvas.draw_idle()
            else:
                annot.set_visible(False)
                ax.figure.canvas.draw_idle()

        ax.figure.canvas.mpl_connect("motion_notify_event", update_annot)

    @property
    def position(self):
        match self.status:
            case "NEW":
                return (0.05, 0.65)
            case "IN_QUEUE":
                position_0 = next(
                    (fan.position[0] - 0.01 for fan in copy(fans).values() if
                     fan.status == "IN_QUEUE" and fan.queued_process_pid == self.fan_pid),
                    0.05
                )
                return position_0, 0.55
            case _:
                return self._position

fans: dict[int | Fan] = {}

def draw_base(ax):
    ax.add_patch(plt.Rectangle((0.5, 0.5), 0.6, 0.6, edgecolor='black', fill=False, lw=2, label="Stadion"))
    ax.add_patch(plt.Rectangle((0.55, 0.85), 0.5, 0.2, edgecolor='blue', fill=True, label="Trybuny", alpha=0.3))
    ax.add_patch(plt.Rectangle((0.55, 0.55), 0.2, 0.2, edgecolor='green', fill=True, label="Toaleta", alpha=0.3))
    ax.add_patch(plt.Rectangle((0.85, 0.55), 0.2, 0.2, edgecolor='orange', fill=True, label="Jedzenie", alpha=0.3))
    for i in range(3):
        ax.add_patch(plt.Rectangle((0.35, 0.85 - i * 0.1), 0.1, 0.05, edgecolor='red', fill=True, alpha=0.3))
        # ax.text(0.075, 0.565 - i * 0.1, f"Stanowisko {i+1}", ha='center', fontsize=8)
    # ax.text(0.35, 0.95, "Kolejka", fontsize=10, ha='center')
    ax.legend()
    ax.set_xlim(0, 2)
    ax.set_ylim(0, 1.5)
    ax.set_aspect('equal')
    ax.axis('off')

def receive_messages(queue):
    global need_update, fans
    while True:
        try:
            message, message_type = queue.receive(block=True, type=TARGET_MESSAGE_TYPE)
            message_text = "{" + message.decode("utf-8", errors="ignore").split("{", 1)[1].strip("\x00")
            print(f"Received message: {message_text}")
            data = json.loads(message_text)
            fan_pid = data.get("fan_pid")
            fan = fans[fan_pid] if fan_pid in fans else Fan(fan_pid)
            fan.process_message(data)
            fans[fan_pid] = fan
        except sysv_ipc.BusyError:
            pass
        except json.JSONDecodeError:
            print("Error decoding JSON.")
        need_update = True

def update(frame, queue, ax, fans: dict[int | Fan]):
    global need_update
    if need_update:
        ax.clear()
        draw_base(ax)
        for fan in copy(fans).values():
            fan.draw(ax)
        need_update = False

def setup_message_queue():
    try:
        return sysv_ipc.MessageQueue(sysv_ipc.ftok(FIFO_KEY, ord(PROJECT_ID)), sysv_ipc.IPC_CREAT)
    except sysv_ipc.ExistentialError:
        print("Message queue does not exist.")
        exit(1)

if __name__ == "__main__":
    mq = setup_message_queue()
    receive_thread = threading.Thread(target=receive_messages, args=(mq,))
    receive_thread.daemon = True
    receive_thread.start()
    fig, ax = plt.subplots(figsize=(12, 8))
    fans: dict[int | Fan] = {}
    ani = animation.FuncAnimation(
        fig, update, fargs=(mq, ax, fans), interval=500
    )
    plt.show()
