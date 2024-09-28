import os
import numpy as np
from numpy import ndarray
import pickle
from typing import Any, Union, Literal
from tqdm import tqdm

class InvalidClass(Exception):
    ...

class InputError(Exception):
    ...


class InvalidActivationFunction(Exception):...
np.random.seed(0)

# REGULARIZER
class L1(object):
    def __init__(self, L1: float = .01) -> None:
        self.L1 = L1
    
    def __reg__(self, weights: ndarray): 
        return self.L1 * np.where(weights > 0, 1, -1)

class L2(object):
    def __init__(self, L2: float = .01) -> None:
        self.L2 = L2
        
    def __reg__(self, weights: ndarray):
        return self.L2 * weights * 2

class L1L2(object):
    def __init__(self, L1: float = .01, L2: float = .01) -> None:
        self.L1 = L1
        self.L2 = L2
    
    def __reg__(self, weights: ndarray):
        return self.L1 * np.where(weights > 0, 1, -1) + self.L2 * weights * 2

# ACTIVATION FUNCTION
class Funct(object):

    @staticmethod
    def tanhs():
        def funct(z: ndarray) -> ndarray:
            return np.tanh(z)

        def derivative(z: ndarray) -> ndarray:
            return - funct(z) ** 2 + 1

        return funct, derivative

    @staticmethod
    def sigmoids():
        def funct(z: ndarray) -> ndarray:
            return 1 / (1 + np.exp(-z))

        def derivative(z: ndarray) -> ndarray:
            sigma = funct(z)
            return sigma * (1 - sigma)

        return funct, derivative

    @staticmethod
    def sigmoidWithLosss():
        def funct(z: ndarray) -> ndarray:
            return 1 / (1 + np.exp(-z))

        def derivative(z: ndarray) -> ndarray:
            return np.ones_like(z)

        return funct, derivative

    @staticmethod
    def softmaxs():
        def funct(z: ndarray) -> ndarray:
            exp_z = np.exp(z)
            return exp_z / np.sum(exp_z, axis = 0)

        def derivative(z: ndarray) -> ndarray:
            return np.ones_like(z)

        return funct, derivative
    
    @staticmethod
    def swishs():
        def funct(z: ndarray) -> ndarray:
            return z / (1 + np.exp(-z))

        def derivative(z: ndarray) -> ndarray:
            fun = funct(z)
            return fun * ((1 / z) + np.exp(-z) / (1 + np.exp(-z)))

        return funct, derivative

    @staticmethod
    def leaky_relus():
        def funct(z: ndarray) -> ndarray:
            return np.where(z > 0, z, .01 * z)

        def derivative(z: ndarray) -> ndarray:
            return np.where(z > 0, 1, .01)

        return funct, derivative

    @staticmethod
    def relus():
        def funct(z: ndarray) -> ndarray:
            return (z + np.abs(z)) / 2
        
        def derivative(z: ndarray) -> ndarray:
            return np.where(z > 0, 1, 0)
        
        return funct, derivative
    
    @staticmethod
    def softpluss():
        def funct(z: ndarray) -> ndarray:
            return np.log(1 + np.exp(z))
        
        def derivative(z: ndarray) -> ndarray:
            exponent = np.exp(z)
            return exponent / (1 + exponent)
        
        return funct, derivative

    @staticmethod
    def elus():
        def funct(z: ndarray) -> ndarray:
            return np.where(z > 0, z, np.exp(z) - 1)

        def derivative(z: ndarray) -> ndarray:
            return np.where(z > 0, 1, np.exp(z))
        
        return funct, derivative

    @staticmethod
    def linears():
        def funct(z: ndarray) -> ndarray:
            return z

        def derivative(z: ndarray) -> ndarray:
            return np.ones_like(z)

        return funct, derivative
        
    leaky_relu = leaky_relus()
    swish = swishs()
    tanh = tanhs()
    sigmoid = sigmoids()
    sigmoidWithLoss = sigmoidWithLosss()
    softmax = softmaxs()
    relu = relus()
    softplus = softpluss()
    elu = elus()
    linear = linears()


# OPTIMIZERS
class Adam(object):
    def __init__(self,
                 learning_rate: float = 0.001,
                 beta1: float = .9,
                 beta2: float = .999,
                 epsilon: float = 1e-5) -> None:
        self.lr = learning_rate
        self.beta1 = beta1
        self.beta2 = beta2
        self.epsilon = epsilon

        self.mw = []
        self.vw = []
        self.mb = []
        self.vb = []
        self.t = 1

    def __build__(self, weights, russian_bias):
        for w, b in zip(weights, russian_bias):
            if type(w) == int: w = np.array(w)
            if type(b) == int: b = np.array(b)
            self.mw += [np.zeros_like(w)]
            self.vw += [np.zeros_like(w)]
            self.mb += [np.zeros_like(b)]
            self.vb += [np.zeros_like(b)]

    def __update__(self, weights, russian_biases, nabla_ws, nabla_bs):
        beta1 = self.beta1; beta2 = self.beta2
        epsilon = self.epsilon
        for index, (_, _) in enumerate(zip(weights, russian_biases)):
            if not isinstance(weights[index], int):
                self.mw[index] = beta1 * self.mw[index] + (1 - beta1) * nabla_ws[index]
                self.vw[index] = beta2 * self.vw[index] + (1 - beta2) * (nabla_ws[index] ** 2)
                mw_hat = self.mw[index] / (1 - (beta1 ** self.t))
                vw_hat = self.vw[index] / (1 - (beta2 ** self.t))
                weights[index] -= self.lr * mw_hat / (np.sqrt(vw_hat) + epsilon)

                self.mb[index] = beta1 * self.mb[index] + (1 - beta1) * nabla_bs[index]
                self.vb[index] = beta2 * self.vb[index] + (1 - beta2) * (nabla_bs[index] ** 2)
                mb_hat = self.mb[index] / (1 - (beta1 ** self.t))
                vb_hat = self.vb[index] / (1 - (beta2 ** self.t))
                russian_biases[index] -= self.lr * mb_hat / (np.sqrt(vb_hat) + epsilon)
        self.t += 1
        return weights, russian_biases
    

class SGD(object):
    def __init__(self, learning_rate: float = 0.001) -> None:
        self.lr = learning_rate
        ...
    
    def __build__(self, weights, russian_bias):
        ...
    
    def __update__(self, weights, russian_biases, nabla_ws, nabla_bs):
        for index, _ in enumerate(weights):
            if not isinstance(weights[index], int):
                weights[index] -= self.lr * nabla_ws[index]
                russian_biases[index] -= self.lr * nabla_bs[index]
        
        return weights, russian_biases

class RMSProp(object):
    def __init__(self,
                 learning_rate: float = 0.001,
                 rho: float = .9,
                 epsilon=1e-7) -> None:
        self.lr = learning_rate
        self.rho = rho
        self.epsilon = epsilon

        self.Sdw = []
        self.Sdb = []

    def __build__(self, weights, russian_biases):
        for w, b in zip(weights, russian_biases):
            self.Sdw += [np.zeros_like(w, dtype=np.float64)]
            self.Sdb += [np.zeros_like(b, dtype=np.float64)]

    def __update__(self, weights, russian_biases, nabla_ws, nabla_bs):
        for index in range(len(weights)):
            if not isinstance(weights[index], int):
                self.Sdw[index] += self.rho * self.Sdw[index] + (1 - self.rho) * weights[index] ** 2
                self.Sdb[index] += self.rho * self.Sdb[index] + (1 - self.rho) * russian_biases[index] ** 2
                weights[index] -= self.lr * (nabla_ws[index] / (np.sqrt(self.Sdw[index] + self.epsilon)))
                russian_biases[index] -= self.lr * (nabla_bs[index] / (np.sqrt(self.Sdb[index] + self.epsilon)))

        return weights, russian_biases

# LAYERS
class Dense(Funct):
    def __init__(self,
                 units: int,
                 activation: Literal['sigmoid', 'tanh', 'softmax', 'swish', 'leaky_relu', 'relu', 'linear'] = 'sigmoid',
                 kernel_regularizers: Any | None = None,
                 **optional: tuple[Any, ...]) -> None:
        self.units = units
        self.optional = optional
        self.activation = activation
        self.kernel_regularizer = L1(0) if kernel_regularizers is None else kernel_regularizers

    def __build__(self, input_size) -> tuple[Any, Any, int]:

        if self.optional and input_size:
            raise InputError("Middle layer should not have input shape")

        if self.optional and 'input_shape' in self.optional.keys():
            weight = np.random.randn(
                self.units,
                int(np.prod(np.array(self.optional["input_shape"])))
            )
            weight *= np.sqrt(2 / (2 * np.prod(np.array(self.optional["input_shape"])) + 1))

        else:
            weight = np.random.randn(
                self.units,
                int(input_size)
            )
            weight *= np.sqrt(2 / (2 * input_size + 1))

        russian_bias_weight = np.atleast_2d(np.random.randn(self.units) * np.sqrt(1 / self.units))
        return weight, russian_bias_weight, self.units

    def __forward__(self, input, weight, russian_bias):
        expanded_weights = np.hstack([weight, np.atleast_2d(russian_bias).T])
        expanded_inputs = np.hstack([input, np.atleast_2d(np.ones(input.shape[0])).T]).T

        z = expanded_weights @ expanded_inputs

        try:
            h = getattr(self, self.activation)[0](z).T
        except AttributeError:
            raise InvalidActivationFunction(f"Activation function '{self.activation}' not found. If you're using bceloss, use sigmoid instead")

        return h, z

    def __backward__(self, dy, dot_prod, batch_input, weight):
        nabla_z = dy * getattr(self, self.activation)[1](dot_prod)
        nabla_w = ((nabla_z @ batch_input) - self.kernel_regularizer.__reg__(weight))
        nabla_b = np.sum(nabla_z, axis=1, keepdims=True).T
        dy = weight.T @ nabla_z
        return nabla_w, nabla_b, dy


class Flatten(Funct):
    def __init__(self, **optional: tuple[Any, ...]) -> None:
        self.optional = optional
        self.activation = "None"
        self.units = np.prod(np.asarray(self.optional["input_shape"]))

    def __build__(self, input_size):
        if self.optional:
            self.oldim = self.optional["input_shape"]
        else:
            self.oldim = input_size
        return 0, 0, np.prod(np.asarray(self.oldim))

    def __forward__(self, input, weight, russian_bias):
        return input.reshape(-1, int(np.prod(np.asarray(self.oldim)))), 0

    def __backward__(self, dy, dot_prod, batch_input, weight):
        return 0, 0, dy.reshape(-1, *self.oldim)


# MAIN MODEL 
class Sequential(object):

    def __init__(self, layers: Union[Any, None] = None) -> None:
        self.optimizer = None
        self.loss = None
        self.layers = []
        self.weights = []
        self.__bias_weights = []
        size = 0

        if layers is not None:
            self.layers = layers
            for layer_name in self.layers:
                if layer_name.__class__.__name__ not in ["Dense", "Flatten"]:
                    raise InvalidClass("Classes for Sequential must be: Dense, Flatten")

            for layer in self.layers:
                weight, russian_bias_weight, size = layer.__build__(size)
                self.weights += [weight]
                self.__bias_weights += [russian_bias_weight]

        self.compile_called = False

    def add(self, layer: Union[Dense, Flatten]):
        size = 0 if not self.layers or self.layers is None else self.layers[-1].units
        self.layers += [layer]
        weight, bias, _ = layer.__build__(size)
        self.weights += [weight];
        self.__bias_weights += [bias]

    def compile(self,
                loss: Literal['mse', 'celoss', 'bceloss', 'tdloss'] = 'mse',
                optimizer: Union[Adam, RMSProp, SGD] = Adam()):
        
        self.loss = loss
        self.optimizer = optimizer
        if loss == 'bceloss':
            self.layers[-1].activation += 'WithLoss'
        self.optimizer.__build__(self.weights, self.__bias_weights)
        self.compile_called = True

    def ForwardProp(self, input):

        batch_inputs = [input]
        batch_input = input.copy()
        dot_prods = []

        for index, layer in enumerate(self.layers):
            batch_input, dot_prod = layer.__forward__(
                input = batch_input,
                russian_bias = self.__bias_weights[index],
                weight = self.weights[index]
            )
            dot_prods += [dot_prod]
            batch_inputs += [batch_input]

        pred = batch_input

        return pred, batch_inputs[:-1], dot_prods

    def BackProp(self, dy, dot_prods, batch_inputs):
        nabla_bs = []
        nabla_ws = []
        for index in range(len(self.layers) - 1, -1, -1):
            layer = self.layers[index]
            nabla_w, nabla_b, dy = layer.__backward__(dy,
                                                      dot_prods[index],
                                                      batch_inputs[index],
                                                      self.weights[index])

            nabla_ws += [nabla_w]
            nabla_bs += [nabla_b]
        
        nabla_ws = list(reversed(nabla_ws))
        nabla_bs = list(reversed(nabla_bs))
        return nabla_ws, nabla_bs

    @staticmethod
    def mse(pred, y_truth):
        loss = np.mean((1 / 2) * np.sum((pred - y_truth) ** 2, axis=1))
        dy = (pred - y_truth)
        return loss, dy
        
    @staticmethod
    def celoss(pred, y_truth):
        loss = - np.mean(np.sum(y_truth * np.log(pred + 1e-5), axis = 1))
        dy = pred - y_truth
        return loss, dy
    
    @staticmethod
    def bceloss(pred, y_truth):
        loss = - np.mean(np.sum(y_truth * np.log(pred + 1e-5) + (1 - y_truth) * np.log(1 - pred + 1e-5), axis = 1))
        dy = pred - y_truth
        return loss, dy

    @staticmethod
    def tdloss(pred, y_truth):
        tdEstimate = y_truth[:, 0]
        actionIdx = y_truth[:, 1].astype(int)
        batchSize = actionIdx.shape[0]

        qSliced = pred[np.arange(batchSize), actionIdx]

        tdDifference = tdEstimate - qSliced

        loss = np.mean(tdDifference ** 2)
        dy = np.zeros_like(pred)
        dy[np.arange(batchSize), actionIdx] = 2 * tdDifference / batchSize
        return loss, dy

    def fit(self, x, y, epochs: int = 1, batch_size: int = 1, validation_split: float = 0, shuffle: bool = True):

        if not self.compile_called:
            self.compile()

        
        x = np.asarray(x)
        y = np.asarray(y)
        overall_size = len(x)
        placeholder = batch_size

        train_size = int(overall_size - overall_size * validation_split)
        val_size = overall_size - train_size

        permute = np.random.permutation(overall_size)
        x = x[permute]
        y = y[permute]

        x_train = x[: train_size]
        y_train = y[: train_size]
        x_val = x[train_size:]
        y_val = y[train_size:]


        for epoch in range(epochs):
            check = 0
            cost = 0
            total_pass = 0
            iteration = 1

            if shuffle:
                permute = np.random.permutation(train_size)
                x_train = x_train[permute]
                y_train = y_train[permute]

            batch_size = placeholder
            # print(f"Epoch {epoch + 1:>{len(str(epochs))}}/{epochs}")
            # pbar = tqdm(range(0, train_size, batch_size), dynamic_ncols = True, leave = True, ascii=".>=")
            for index in range(0, train_size, batch_size):
                if batch_size > train_size - index:
                    batch_size = train_size - index

                # Forward Feed
                pred, batch_inputs, dot_prods = self.ForwardProp(x_train[index: index + batch_size])
                # test_number = np.argmax(y_train[index: index + batch_size], axis = 1)

                # Array of y truth
                # check += np.sum(np.argmax(pred, axis=1) == test_number)

                # Derivative and loss
                loss, dy = getattr(Sequential, self.loss)(pred, y_train[index: index + batch_size])

                # Backpropagation
                nabla_ws, nabla_bs = self.BackProp(dy.T, dot_prods, batch_inputs)

                # Updating
                self.weights, self.__bias_weights = self.optimizer.__update__(self.weights,
                                                                              self.__bias_weights,
                                                                              nabla_ws,
                                                                              nabla_bs)

                cost += loss
                total_pass += batch_size
                print(f"{cost / iteration:.4f}", end = '                          \r', flush = True)
                # pbar.set_postfix_str(f"Loss: {cost / iteration:.4f} - Accuracy: {check / total_pass:.4f}")
                iteration += 1

            batch_size = placeholder
            # valPbar = tqdm(range(0, val_size, batch_size), leave = True, dynamic_ncols = True, ascii = ".>=")
            valPass = 0
            val_score = 0
            for index in range(0, val_size, batch_size):
                if batch_size > val_size - index:
                    batch_size = val_size - index

                valPass += batch_size
                pred = self.predict(x_val[index: index + batch_size], batch_size = batch_size)
                val_score += np.sum(np.argmax(pred, axis = 1) == np.argmax(y_val[index: index + batch_size], axis = 1))

                # valPbar.set_postfix_str(f"Val Accuracy {val_score / valPass:.4f}")
            # print('')
                

    def save(self, filePath: str, overide: bool = False):
        os.makedirs(filePath, exist_ok=overide)
        with open(filePath + r"/fingerprint.pkl", 'wb') as file:
            pickle.dump(self.layers, file)

        os.makedirs(filePath + r"/saved_model", exist_ok=overide)
        np.savez(filePath + r'/saved_model/weights.npz', *self.weights)
        np.savez(filePath + r'/saved_model/russian_biases.npz', *self.__bias_weights)
        print(f"Model has been saved into: {filePath}")

    def load_model(self, filePath):
        keys = np.load(filePath + r"/saved_model/weights.npz").files
        self.weights, self.__bias_weights = [], []
        
        for key in keys:
            weight = np.load(filePath + r"/saved_model/weights.npz")[key]
            bias = np.load(filePath + r"/saved_model/russian_biases.npz")[key]
            self.weights += [np.float64(weight)]
            self.__bias_weights += [np.float64(bias)]
            
        with open(filePath + r"/fingerprint.pkl", 'rb') as f:
            self.layers = list(pickle.load(f))

    def predict(self, x, batch_size: int = 1):
        res = np.array([])
        for index in range(0, len(x), batch_size):
            if batch_size > len(x) - index:
                batch_size = len(x) - index
            pred, _, _ = self.ForwardProp(x[index: index + batch_size])

            if len(res) == 0:
                res = pred
            else:
                res = np.concatenate([res, pred], axis=0)
        return res
