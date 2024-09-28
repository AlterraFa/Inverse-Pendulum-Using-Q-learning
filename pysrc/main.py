import os
import copy
import fcntl
import select
import random
import subprocess
import numpy as np
from modules import *



class Communicator():
    def __init__(self, execPath: str, mediumPath: str) -> None:
        self.mediumPath = mediumPath
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

class VelocityNormalizer:
    def __init__(self):
        self.mean = 0.0
        self.std = 1.0  # Initialize to 1 to avoid division by zero
        self.n = 0      # Count of observed velocities

    def update(self, new_velocity):
        """Update running mean and standard deviation with new velocity."""
        self.n += 1
        old_mean = self.mean
        self.mean += (new_velocity - self.mean) / self.n  # Update mean
        self.std = np.sqrt(((self.std ** 2) * (self.n - 1) + (new_velocity - old_mean) * (new_velocity - self.mean)) / self.n)  # Update std

    def normalize(self, velocity):
        """Normalize the velocity using the current mean and std."""
        if self.n < 2:  # Avoid dividing by zero for very few samples
            return velocity
        return (velocity - self.mean) / self.std

def calculateReward(degree, error):
    if 0 <= degree <= error:
        reward = 1 - np.sqrt(((0 - degree) / error) ** 2)
    elif 1 >= degree >= 1 - error:
        reward = 1 - np.sqrt(((1 - degree) / error) ** 2)
    else:
        reward = 0
    
    return reward 

def stateNorm(state, cartNorm, pendulumNorm):
    normState = state.copy()
    normState[0] = (normState[0] - 365) / (1485 - 365) 
    normState[3] = (normState[3]) / 360 
    cartNorm.update(normState[1])
    pendulumNorm.update(normState[2])
    normState[1] = cartNorm.normalize(normState[1])
    normState[2] = pendulumNorm.normalize(normState[2])
    return normState[None, :]


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
    qModel.add(Dense(24, activation = 'linear', input_shape = (4, )))
    qModel.add(Dense(24, activation = 'linear'))
    qModel.add(Dense(2, activation = 'linear'))
    qModel.compile('tdloss', Adam(learning_rate = 0.0001))
    tdModel = copy.deepcopy(qModel) 

    cartNorm = VelocityNormalizer()
    pendulumNorm = VelocityNormalizer()


    actionMap = np.array([3, 4])
    action = 0
    epsilon = .05
    tdStep = 4
    batchSize = 256
    discountVal = .95

    updateIteration = 1000
    iteration = 0
    bufferSize = 30000 
    discountArray = np.power(discountVal, np.arange(0, tdStep - 1, 1))

    stateBuffer = np.empty((0, 4), dtype = np.float16)
    actionBuffer = np.empty(0, dtype = int)
    rewardBuffer = np.empty(0, dtype = np.float16)
    seqRows = np.arange(0, tdStep, 1)
    tdRow = np.arange(0, batchSize, 1)
    # Store [state, action, reward, nextState, nextAction, doneFlag]
    while listener.process.poll() is None:
        output, err = listener.readAndWrite(str(action))
            
        try:
            state = listener.parse_matrix(output)
        except: state = None

        if state is not None:
            # extract the flag and normalize the state
            doneFlag = state[0, -1]
            state = stateNorm(state[0, :-1], cartNorm, pendulumNorm).astype(np.float16)

            qVal = qModel.predict(state)

            # Epsilon policy
            if np.random.choice([0, 1], p = [1 - epsilon, epsilon]):
                actionIdx = np.random.randint(0, 2)
            else:
                actionIdx = np.argmax(qVal)
            action = actionMap[actionIdx]
            reward = calculateReward(state[0, -1], 0.25)

            # First in First out
            stateBuffer = np.r_[stateBuffer, state] 
            actionBuffer = np.r_[actionBuffer, actionIdx]
            rewardBuffer = np.r_[rewardBuffer, reward]
            if (stateBuffer.shape[0] > bufferSize):
                stateBuffer = np.delete(stateBuffer, 0, 0)
                actionBuffer = np.delete(actionBuffer, 0, 0)
                rewardBuffer = np.delete(rewardBuffer, 0, 0)

            currentBufferSize = stateBuffer.shape[0]

            if currentBufferSize - tdStep > batchSize:
                samplesIdx = np.array(random.sample(range(0, currentBufferSize - tdStep), k = batchSize)) 
                sequenceIdx = samplesIdx[:, None] + seqRows
                # (batch, tdStep, 4(states))
                trainState = stateBuffer[sequenceIdx] 
                # (batch, tdStep)
                trainAction = actionBuffer[sequenceIdx] # integer
                trainReward = rewardBuffer[sequenceIdx] # double

                tdQval = tdModel.predict(trainState[:, -1, :])
                tdEstimate = trainReward[:, :-1] @ discountArray + tdQval[tdRow, trainAction[:, -1]] * (discountVal ** tdStep) # y_truth
                qModel.fit(x = trainState[:, 0, :], y = np.c_[tdEstimate[:, None], trainAction[:, 0:1]], batch_size = batchSize)
                iteration += 1 
                if iteration % updateIteration == 0:
                    tdModel = copy.deepcopy(qModel)

