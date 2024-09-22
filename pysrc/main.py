import subprocess
import os
import threading
from listener import Listener



if __name__ == "__main__":

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
    listen = Listener(process)
    listen_thread = threading.Thread(target = listen.listen)
    listen_thread.start()

    while True:
        data = listen.data
        if data is not None:
            print(data, end = '                \r', flush = True)

