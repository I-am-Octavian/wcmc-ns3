import ns3ai_rnn_py as interface
from ns3ai_utils import Experiment
import sys
import traceback


exp = Experiment("assignment", "./../",
                 interface, handleFinish=True)

msgInterface = exp.run(show_output=True)
totalCount = 12
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

            x = x + (decX/1000)
            y = y + (decY/1000)
            print(str(x) + " " + str(y) + "\n")
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
            print(str(x) + " " + str(y) + "\n")

            # send to C++ side
            msgInterface.PySendBegin()
            if msgInterface.PyGetFinished():
                print("Something")
                break
            msgInterface.GetPy2CppStruct().mcsPredicted = 10
            msgInterface.PySendEnd()
        currCount += 1;

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
