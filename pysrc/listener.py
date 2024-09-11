import subprocess
from pynput import keyboard

process = subprocess.Popen(
    ["build/bin/PendulumExec"],  
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    text=True
)

def on_press(key):
    try:
        if key == keyboard.Key.up:
            process.stdin.write("1\n")
            process.stdin.flush()
        elif key == keyboard.Key.down:
            process.stdin.write("2\n")
            process.stdin.flush()
        elif key == keyboard.Key.left:
            process.stdin.write("3\n")
            process.stdin.flush()
        elif key == keyboard.Key.right:
            process.stdin.write("4\n")
            process.stdin.flush()

    except AttributeError:
        pass

def on_release(key):
    try:
        if key in {keyboard.Key.up, keyboard.Key.down, keyboard.Key.left, keyboard.Key.right}:
            process.stdin.write("0\n")  # Send reset signal on key release
            process.stdin.flush()
    except AttributeError:
        pass
    
    if key == keyboard.Key.esc:
        print("Exiting...")
        return False

with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()

process.terminate()
