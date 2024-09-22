from pynput import keyboard
import threading
import numpy as np
import subprocess


class Listener():
    def __init__(self, process) -> None:
        self.process = process
        self.data = None
        output_thread = threading.Thread(target = self.read_output, args = ())
        output_thread.daemon = True
        output_thread.start()

    def get_focused_window_title(self):
        try:
            result = subprocess.run(['xdotool', 'getwindowfocus', 'getwindowname'], capture_output=True, text=True)
            return result.stdout.strip()
        except Exception as e:
            print(f"Error getting focused window: {e}")
            return ""

    def on_press(self, key):
        if self.get_focused_window_title() == "Pendulum simulation":
            if self.process.stdin:
                try:
                    if key == keyboard.Key.up:
                        self.process.stdin.write("1\n")
                    elif key == keyboard.Key.down:
                        self.process.stdin.write("2\n")
                    elif key == keyboard.Key.left:
                        self.process.stdin.write("3\n")
                    elif key == keyboard.Key.right:
                        self.process.stdin.write("4\n")
                    self.process.stdin.flush()
                except AttributeError:
                    pass

    def on_release(self, key) -> None:
        if self.get_focused_window_title() == "Pendulum simulation":
            try:
                if key in {keyboard.Key.up, keyboard.Key.down, keyboard.Key.left, keyboard.Key.right} and self.process.stdin:
                    self.process.stdin.write("0\n")  # Send reset signal on key release
                    self.process.stdin.flush()
            except AttributeError:
                pass
            
            if key == keyboard.Key.esc:
                print("Exiting...")
                return self.listener.stop()

    @staticmethod
    def parse_matrix(matStr):
        data = np.fromiter((float(x) for x in matStr.replace(';', ',').split(',')) , dtype = float)

        rows = matStr.count(';') + 1
        cols = matStr.split(';')[0].count(',') + 1
        return data.reshape(rows, cols)


    def read_output(self):
        while True:
            output = self.process.stdout.readline()
            if output == "" and self.process.poll() is not None:
                break
            if output:
                self.data = self.parse_matrix(output.strip())

    def read_data(self): return self.data

    def listen(self):
        with keyboard.Listener(on_press=self.on_press, on_release=self.on_release) as self.listener:
            self.listener.join()

        self.process.terminate()
