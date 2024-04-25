import ns3ai_rnn_py as interface
from ns3ai_utils import Experiment
import sys
import traceback


exp = Experiment("assignment", "./../",
                 interface, handleFinish=True)

msgInterface = exp.run(show_output=True)
try:
    while True:
        # receive from C++ side
        msgInterface.PyRecvBegin()
        if msgInterface.PyGetFinished():
            print("Something")
            break
        # calculate the sum
        x = msgInterface.GetCpp2PyStruct().posX
        y = msgInterface.GetCpp2PyStruct().posY
        decY = msgInterface.GetCpp2PyStruct().decimalY
        decX = msgInterface.GetCpp2PyStruct().decimalX
        msgInterface.PyRecvEnd()
        print(str(x) + " " + str(y))
        print(str(decX) + " " + str(decY))

        # send to C++ side
        msgInterface.PySendBegin()
        if msgInterface.PyGetFinished():
            print("Something")
            break
        msgInterface.GetPy2CppStruct().mcsPredicted = x+y
        msgInterface.PySendEnd()
        print(str(x) + " " + str(y))

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
