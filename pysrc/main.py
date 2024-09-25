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


def calculateReward(degree, error):
    if 0 <= degree <= error:
        reward = 1 - np.sqrt(((0 - degree) / error) ** 2)
    elif 360 >= degree >= 360 - error:
        reward = 1 - np.sqrt(((360 - degree) / error) ** 2)
    else:
        reward = 0
    
    return reward 


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
    qModel.add(Dense(5, activation = 'linear'))
    qModel.compile('mse', Adam(learning_rate = 0.005))
    tdModel = copy.deepcopy(qModel) 


    actionMap = np.array([3, 5, 0, 6, 4])
    action = 0
    epsilon = .01
    tdStep = 4
    batchSize = 512

    updateIteration = 1000
    bufferSize = 50000
    replayBuffer = np.empty((bufferSize, 5), dtype = object)
    bufferIndex = 0
    # Store [state, action, reward, nextState, nextAction, doneFlag]
    while listener.process.poll() is None:
        output, err = listener.readAndWrite(str(action))
            
        try:
            state = listener.parse_matrix(output)
        except: state = None

        if state is not None:
            qVal = qModel.predict(state)

            # Epsilon policy
            if np.random.choice([0, 1], p = [1 - epsilon, epsilon]):
                actionIdx = np.random.randint(0, 5)
            else:
                actionIdx = np.argmax(qVal)
            action = actionMap[actionIdx]
            reward = calculateReward(state[0, -1], 90)

            # Replay buffer, moving border in circular manner
            if (replayBuffer.shape[0] - tdStep) >=0:
                idx_to_update = (bufferIndex - tdStep) % bufferSize
                replayBuffer[idx_to_update, 3] = state[0]
                replayBuffer[idx_to_update, 4] = actionIdx

            replayBuffer[bufferIndex % bufferSize] = [state[0], actionIdx, reward, None, None]
            bufferIndex += 1
