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
        temp = msgInterface.GetCpp2PyStruct().posX + msgInterface.GetCpp2PyStruct().posY
        msgInterface.PyRecvEnd()

        # send to C++ side
        msgInterface.PySendBegin()
        msgInterface.GetPy2CppStruct().mcsPredicted = temp
        msgInterface.PySendEnd()

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
