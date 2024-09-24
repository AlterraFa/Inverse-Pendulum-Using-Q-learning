import os
import copy
import fcntl
import select
import subprocess
import numpy as np
from modules import *



class Communicator():
    def __init__(self, execPath: str, mediumPath: str) -> None:
        self.mediumPath = mediumPath
        self.execPath = execPath
        self.process = subprocess.Popen(
            [execPath],
            stdout = subprocess.PIPE,
            stderr = subprocess.PIPE,
            text = True,
            bufsize = 1
        )

        if self.process.stderr == None or self.process.stdout == None:
            print("Failed to initialize program")
            exit(1)
        self.setNonBlocking(self.process.stdout.fileno())
        self.setNonBlocking(self.process.stderr.fileno())

    @staticmethod
    def parse_matrix(matStr):
        try:
            data = np.fromiter((float(x) for x in matStr.replace(';', ',').split(',')), dtype=float)
            rows = matStr.count(';') + 1
            cols = matStr.split(';')[0].count(',') + 1
            return data.reshape(rows, cols)
        except ValueError as e:
            print(f"Error parsing matrix: {e}")
            return None

    @staticmethod
    def setNonBlocking(fd):
        fl = fcntl.fcntl(fd, fcntl.F_GETFL)
        fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)


    def readAndWrite(self, text: str):
        if self.process.stderr == None or self.process.stdout == None:
            print("Failed to initialize program")
            exit(1)

        with open(self.mediumPath, "w") as file:
            fcntl.flock(file, fcntl.LOCK_EX)
            file.write(text)
        ready, _, _ = select.select([self.process.stdout.fileno(), self.process.stderr.fileno()], [], [], 0.1)

        output, error = None, None
        for fd in ready:
            if fd == self.process.stdout.fileno():
                output = self.process.stdout.readline()

            elif fd == self.process.stderr.fileno():
                error = self.process.stderr.readline()

        return output, error


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

    listener = Communicator(exec_path, "pysrc/medium")
    qModel = Sequential()
    qModel.add(Dense(24, activation = 'leaky_relu', input_shape = (4, )))
    qModel.add(Dense(24, activation = 'leaky_relu'))
    qModel.add(Dense(3, activation = 'linear'))
    tdModel = copy.deepcopy(qModel) 
    actionMap = np.array([3, 0, 4])
    
    action = 0
    while listener.process.poll() is None:
        output, err = listener.readAndWrite(str(action))
        
        state = np.array([[0, 0, 0, 0]])
        try:
            state = listener.parse_matrix(output)
        except: ...

        qVal = qModel.predict(state)
        action = actionMap[np.argmax(qVal)]
