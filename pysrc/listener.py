import subprocess
from pynput import keyboard
import os
import threading
import numpy as np


build_dir = "build"
exec_path = os.path.join(build_dir, "bin", "PendulumExec")

try:
    os.makedirs(build_dir, exist_ok=True)
    subprocess.run(["cmake", ".."], cwd=build_dir, check=True)
    subprocess.run(["cmake", "--build", ".", f"-j{os.cpu_count()}"], cwd=build_dir, check=True)
    print("Build successful.")
except subprocess.CalledProcessError as e:
    print(f"Error during CMake build: {e}")
    exit(1)

process = subprocess.Popen(
    [exec_path],  
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    text=True
)


def parse_matrix(matStr):
    data = np.fromiter((float(x) for x in matStr.replace(';', ',').split(',')) , dtype = float)

    rows = matStr.count(';') + 1
    cols = matStr.split(';')[0].count(',') + 1
    return data.reshape(rows, cols)


def read_output(process):
    while True:
        output = process.stdout.readline()
        if output == "" and process.poll() is not None:
            break
        if output:
            data = parse_matrix(output.strip())
            print(data, end = '     \r', flush = True)

def get_focused_window_title():
    try:
        result = subprocess.run(['xdotool', 'getwindowfocus', 'getwindowname'], capture_output=True, text=True)
        return result.stdout.strip()
    except Exception as e:
        print(f"Error getting focused window: {e}")
        return ""

def on_press(key):
    if get_focused_window_title() == "Pendulum simulation":
        if process.stdin:
            try:
                if key == keyboard.Key.up:
                    process.stdin.write("1\n")
                elif key == keyboard.Key.down:
                    process.stdin.write("2\n")
                elif key == keyboard.Key.left:
                    process.stdin.write("3\n")
                elif key == keyboard.Key.right:
                    process.stdin.write("4\n")
                process.stdin.flush()
            except AttributeError:
                pass

def on_release(key) -> None:
    if get_focused_window_title() == "Pendulum simulation":
        try:
            if key in {keyboard.Key.up, keyboard.Key.down, keyboard.Key.left, keyboard.Key.right} and process.stdin:
                process.stdin.write("0\n")  # Send reset signal on key release
                process.stdin.flush()
        except AttributeError:
            pass
        
        if key == keyboard.Key.esc:
            print("Exiting...")
            return listener.stop()
        

output_thread = threading.Thread(target = read_output, args = (process, ))
output_thread.daemon = True
output_thread.start()



with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()

process.terminate()
