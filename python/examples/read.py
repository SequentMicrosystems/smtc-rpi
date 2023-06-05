import sm_tc
import time
import threading as th
import sys
from datetime import datetime

keep_going = True


def key_capture_thread():
    global keep_going
    input()
    keep_going = False


if __name__ == "__main__":
    t = sm_tc.SMtc(0)
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    else:
        filename = ''
    print("Read all thermocouples temperature in a loop, hit ENTER to exit")
    if filename != '':
        file = open(filename, "w")
        print("Save all values in the " + filename + " file")
        line = "Time, t1, t2, t3, t4, t5, t6, t7, t8\n"
        file.write(line)
    th.Thread(target=key_capture_thread, args=(), name='key_capture_thread', daemon=True).start()
    while keep_going:
        now = datetime.now()
        current_time = now.strftime("%H:%M:%S")
        line = str(current_time)
        for i in range(8):
            temp = t.get_temp(i + 1)
            if filename != '':
                line += ", " + str(temp)
            print(str(i + 1) + "->" + str(temp) + chr(176) + "C  ", end=" ")
        if filename != '':
            line += "\n"
            file.write(line)
        print(' ')
        time.sleep(1)
    if filename != '':
        file.close()
    print(' ')
