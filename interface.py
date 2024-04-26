import ns3ai_rnn_py as interface
from ns3ai_utils import Experiment
import sys
import traceback
import tensorflow as tf
import keras
from keras import layers,losses
import numpy as np

CUDA_VISIBLE_DEVICES=""

sequence_length = 100
#model = keras.Sequential()
#model.add(layers.SimpleRNN(40, kernel_regularizer=keras.regularizers.l1_l2(l1=2e-5, l2=2e-4),
#                    recurrent_regularizer=keras.regularizers.l1_l2(l1=1e-5, l2=2e-4),
#                    bias_regularizer=keras.regularizers.l2(2e-4),
#                    activity_regularizer=keras.regularizers.l2(2e-5),activation='relu', input_shape=(sequence_length, 2)))
#model.add(layers.Dense(9, activation='softmax'))
#model.compile(optimizer='adam', loss=keras.losses.SparseCategoricalCrossentropy(from_logits=True), metrics=['accuracy'])
checkpoint_path = "ns3/checkpoint"
model = keras.models.load_model('firstModel.Keras')


lookup = np.array([28, 18, 22, 26, 14, 20, 16, 24,  0])

X_max = np.array([5726.29, 5916.92])
X_min = np.array([-641.929, -810.928])

X_scaled = []

exp = Experiment("assignment", "./../",
                 interface, handleFinish=True)

msgInterface = exp.run(show_output=True)
totalCount = 100
currCount = 1
try:
    while True:
        # receive from C++ side

        if totalCount > currCount:
            msgInterface.PyRecvBegin()
            if msgInterface.PyGetFinished():
                print("Something")
                break
            x = msgInterface.GetCpp2PyStruct().posX
            y = msgInterface.GetCpp2PyStruct().posY
            decY = msgInterface.GetCpp2PyStruct().decimalY
            decX = msgInterface.GetCpp2PyStruct().decimalX
            msgInterface.PyRecvEnd()

            x = float(x) + float(decX)/1000
            y = float(y) + float(decY)/1000

            X = np.array([x,y])

            X_scaled.append((X - X_min) / (X_max - X_min))

            if currCount == totalCount - 1:
                X_scaled = np.array([X_scaled])
                print(X_scaled.shape)

        else:
            msgInterface.PyRecvBegin()
            if msgInterface.PyGetFinished():
                print("Something")
                break
            x = msgInterface.GetCpp2PyStruct().posX
            y = msgInterface.GetCpp2PyStruct().posY
            decY = msgInterface.GetCpp2PyStruct().decimalY
            decX = msgInterface.GetCpp2PyStruct().decimalX
            msgInterface.PyRecvEnd()

            x = float(x) + float(decX)/1000
            y = float(y) + float(decY)/1000

            #X_scaled = np.append(X_scaled, np.array([x,y]))

            dummy = []
            counter = 0
            for i in range(0, 99):
                dummy.append(X_scaled[0][i])
            dummy.append(np.array([x,y]))
            X_scaled = np.array([dummy])

            print(X_scaled.shape)

            prediction = model.predict(X_scaled)

            index = np.where(prediction[:][0] == max(prediction[:][0]))[0][0]

            dummy = []
            counter = 0
            for i in range(1, 100):
                dummy.append(X_scaled[0][i])
            X_scaled = np.array([dummy])

            # send to C++ side
            msgInterface.PySendBegin()
            if msgInterface.PyGetFinished():
                print("Something")
                break
            msgInterface.GetPy2CppStruct().mcsPredicted = lookup[index]
            print("Sending")
            msgInterface.PySendEnd()
        currCount += 1

except Exception as e:
    exc_type, exc_value, exc_traceback = sys.exc_info()
    print("Exception occurred: {}".format(e))
    print("Traceback:")
    traceback.print_tb(exc_traceback)
    exit(1)

else:
    pass

finally:
    print("Finally exiting...")
    del exp
