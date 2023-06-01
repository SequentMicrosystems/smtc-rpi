import sm_tc
import time
import threading as th

keep_going = True
def key_capture_thread():
    global keep_going
    input()
    keep_going = False

t = sm_tc.SMtc(0)
print("Read all thermocouples themperature in a loop, hit ENTER to exit")
th.Thread(target=key_capture_thread, args=(), name='key_capture_thread', daemon=True).start()
while keep_going:
    for i in range(8):
        temp = t.get_temp(i + 1)
        print(str(i+1) + "->" + str(temp) + chr(176) + "C  ", end=" ")
    print(' ')
    time.sleep(1)
print(' ')


