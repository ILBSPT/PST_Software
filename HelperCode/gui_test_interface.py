import PySimpleGUI as sg
import time
import serial
import sys

#mac
#ser = serial.Serial('/dev/cu.usbserial-0001', 115200, timeout=0.015) #set read timeout of 1s
#ser = serial.Serial('/dev/cu.usbserial-3', 115200, timeout=0.015) #set read timeout of 1s
#windows
#ser = serial.Serial('COM3', 115200, timeout=0.01) #set read timeout of 1s
if(len(sys.argv) > 2):
    ser = serial.Serial(sys.argv[2], 115200, timeout=0.001) #set read timeout of 1s
else:
    ser = serial.Serial('COM3', 115200, timeout=0.001) #set read timeout of 1s

log_delay = 0.01
#log_delay = 1
message_timeout = 0.25

missed_packets = 0
log_speed = 0

command_map = {
    "STATUS" : 0,
    "ABORT" : 1,
    "EXEC_PROG" : 2, 
    "STOP_PROG" : 3,
    "FUELING" : 4,

#FLIGHT computer commands
    "READY" : 5,
    "ARM" : 6,

    "LED_ON" : 7,
    "LED_OFF" : 8,

    "IMU_CALIB" : 9,

#FILLING station commands
    "RESUME_PROG" : 10,

#used to get the number of commands 
    "cmd_size" : 11,

#ACKs
    "STATUS_ACK" : 12,
    "ABORT_ACK" : 13, 
    "EXEC_PROG_ACK" : 14,
    "STOP_PROG_ACK" : 15,
    "FUELING_ACK" : 16,
    "READY_ACK" : 17,
    "ARM_ACK" : 18,
    "LED_ON_ACK" : 19,
    "LED_OFF_ACK" : 20,
    "IMU_CALIB_ACK" : 21,
    "RESUME_PROG_ACK" : 22,
}

state_map_fill = {
    "IDLE" : 0,
    "FUELING" : 1,
    "PROG1" : 2,
    "PROG2" : 3,
    "PROG3" : 4,
    "STOP" : 5,
    "ABORT": 6,
}

state_map_rocket = {
    "IDLE" : 0,
    "FUELING" : 1,
    "PROG1" : 2,
    "PROG2" : 3,
    "SAFETY" : 4,
    "READY" : 5,
    "ARMED" : 6,
    "LAUNCH": 7,
    "ABORT": 8,
    "IMU_CALIB": 9
}

state_map_to_string_fill = {
    0 : "IDLE",
    1 : "FUELING",
    2 : "PROG1",
    3 : "PROG2",
    4 : "PROG3",
    5 : "SAFETY",
    6 : "ABORT",
}

state_map_to_string_rocket = {
    0 : "IDLE",
    1 : "FUELING",
    2 : "PROG1",
    3 : "PROG2",
    4 : "SAFETY",
    5 : "READY",
    6 : "ARMED",
    7 : "LAUNCH",
    8 : "ABORT",
    9 : "IMU_CALIB"
}



sync_state = 1
cmd_state = 2
size_state = 3
data_state = 4
crc_1_state = 5
crc_2_state = 6
end_state = 7

comm_state = sync_state
data_total = 0
data_recv = 0
begin = time.perf_counter()
end = time.perf_counter()
buff = bytearray()

sg.theme('DarkAmber')    # Keep things interesting for your users

layout_rocket = [[sg.Button('LED_ON', key = '_LED_ON_', size = (10,5)), sg.Button('LED_OFF', key = '_LED_OFF_', size = (10,5)), sg.Text("Ax  Ay  Az  Gx  Gy  Gz", key = '_IMU_OUT_', size = (40, 5), auto_size_text=True, font=('Arial Bold', 16))],      
          [sg.Button('Start Fueling', key = '_FUELING_', size = (10, 5)),
           sg.Button('Stop', key = '_STOP_', size = (10, 5)),
           sg.Button('Exec prog', key = '_EXEC_', size = (10, 5)),
           #sg.Combo(values = (1,2), default_value = 1, key = '_PROG_', size = (10,5), font=('Arial Bold', 16)),
           sg.Input(key = '_PROG_', size = (20,5), font=('Arial Bold', 16))],
          [sg.Button('Ready', key = '_READY_', size = (10,5)),
           sg.Button('Arm', key = '_ARM_', size = (10,5)), 
           sg.Button('Abort', key = '_ABORT_', size = (10,5)), 
           sg.Button('Status', key = "_STATUS_", size = (10,5)),
           sg.Button('IMU_calib', key = "_IMU_calib_", size = (10,5)),
           sg.Button('Toggle log', key = '_LOG_TOGGLE_', size = (10,5)), sg.Exit()]]      

layout_fill = [[sg.Button('LED_ON', key = '_LED_ON_', size = (10,5)), sg.Button('LED_OFF', key = '_LED_OFF_', size = (10,5)), sg.Text("Ax  Ay  Az  Gx  Gy  Gz", key = '_IMU_OUT_', size = (40, 5), auto_size_text=True, font=('Arial Bold', 16))],      
          [sg.Button('Exec prog', key = '_EXEC_', size = (10, 5)),
           #sg.Combo(values = (1,2,3), default_value = 1, key = '_PROG_', size = (10,5), font=('Arial Bold', 16)),
           sg.Input(key = '_PROG_', size = (20,5), font=('Arial Bold', 16))],
          [sg.Button('Start Fueling', key = '_FUELING_', size = (10, 5)),
           sg.Button('Stop', key = '_STOP_', size = (10, 5)),
           sg.Button('Resume', key = '_RESUME_', size = (10, 5)), sg.Button('Abort', key = '_ABORT_', size = (10,5))],
          [ sg.Button('Status', key = "_STATUS_", size = (10,5)),
           sg.Button('Toggle log', key = '_LOG_TOGGLE_', size = (10,5)), sg.Exit()]]  

#layout = layout_rocket
if(len(sys.argv) > 1):
    if(sys.argv[1] == 'r'):
        layout = layout_rocket
        state_map = state_map_rocket
        state_map_to_string = state_map_to_string_rocket
    elif(sys.argv[1] == 'f'):
        layout = layout_fill
        state_map = state_map_fill
        state_map_to_string = state_map_to_string_fill
else:
    layout = layout_rocket
    state_map = state_map_rocket
    state_map_to_string = state_map_to_string_rocket

window = sg.Window('Window that stays open', layout)      


log_status = 0
def toggle_status():
    global log_status
    if log_status == 0: log_status = 1
    elif log_status == 1: log_status = 0

def arm():
    cmd = bytearray([0x55, command_map['ARM'], 1, 1, 2, 3])
    ser.write(cmd)
    read_cmd()
    if(len(buff) != 6 or int(buff[1]) != command_map['ARM_ACK'] or int(buff[3]) != 1):
        print("ARM1 error", buff)
        return
    else:
        print("ARM 1 ok")

    cmd = bytearray([0x55, command_map['ARM'], 1, 2, 2, 3])
    ser.write(cmd)
    read_cmd()
    if(len(buff) != 6 or int(buff[1]) != command_map['ARM_ACK'] or int(buff[3]) != 2):
        print("ARM2 error", buff)
        return
    else:
        print("ARM 2 ok")
    

    cmd = bytearray([0x55, command_map['ARM'], 1, 3, 2, 3])
    ser.write(cmd)
    read_cmd()
    if(len(buff) != 6 or int(buff[1]) != command_map['ARM_ACK'] or int(buff[3]) != 3):
        print("ARM3 error", buff)
        return
    else:
        print("ARM 3 ok")


def print_status():
    global missed_packets
    global log_speed
    #if(len(buff) < 16):
        #print("bad status")
        #return

    state = int.from_bytes(buff[3:4], byteorder='big', signed=True)
    if state < 0 or state >= len(state_map_to_string): 
        print("bad state decoding", state)
        return

    #ax = int.from_bytes(buff[4:6], byteorder='big', signed=True)
    #ay = int.from_bytes(buff[6:8], byteorder='big', signed=True)
    #az = int.from_bytes(buff[8:10], byteorder='big', signed=True)

    #ax = (ax * 0.244) / 1000 * 9.80665
    #ay = (ay * 0.244) / 1000 * 9.80665
    #az = (az * 0.244) / 1000 * 9.80665

    #gx = int.from_bytes(buff[10:12], byteorder='big', signed=True)
    #gy = int.from_bytes(buff[12:14], byteorder='big', signed=True)
    #gz = int.from_bytes(buff[14:16], byteorder='big', signed=True)

    print("state", state)
    s1 = "State: " + state_map_to_string[state] + "\n"
    
    #sa = "Ax: " + str(round(ax, 2)) + " Ay: " + str(round(ay, 2)) + " Az: " + str(round(az, 2)) + "\n"
    #sg = "Gx: " + str(round(gx, 2)) + " Gy: " + str(round(gy, 2)) + " Gz: " + str(round(gz, 2)) + "\n"

    sp = "Pressure: " + str(int.from_bytes(buff[4:6], byteorder='big', signed=False)) + "\n"
    sl = "Tank Liquid: " + str(int.from_bytes(buff[6:8], byteorder='big', signed=False)) + "\n"

    s2 = "Log Speed: " + str(round(log_speed, 0)) + "hz\nMissed packets: " + str(missed_packets) + "\n" 

    #s = s1 + sa + sg + s2
    s = s1 + sl + sp + s2

    window['_IMU_OUT_'].update(s)

    #print("ax:", ax, "ay:", ay, "az:", az)
    #print("gx:", gx, "gy:", gy, "gz:", gz)

def read_cmd():
    global comm_state, sync_state, cmd_state, size_state, data_state, crc_1_state, crc_2_state, end_state
    global buff
    global data_recv, data_total
    global begin, end
    global missed_packets

    buff = bytearray()
    data_recv = 0
    begin = time.perf_counter()
    
    while True: 
        while ser.in_waiting > 0:
            ch = ser.read(1)
            ch = int.from_bytes(ch, 'little')

            if comm_state == sync_state:
                if ch == 0x55:
                    #begin = time.perf_counter()
                    comm_state = cmd_state
                    buff.append(ch)
                else:
                    print(chr(ch), end="")
                    #print(str(ch).encode('utf-8'))

            elif comm_state == cmd_state:
                buff.append(ch)
                comm_state = size_state

            elif comm_state == size_state:
                buff.append(ch)
                data_total = int(ch)
                #print("data total", data_total)
                if data_total == 0:
                    comm_state = crc_1_state
                else:
                    comm_state = data_state

            elif comm_state == data_state:
                buff.append(ch)
                data_recv += 1
                if data_total == data_recv:
                    comm_state = crc_1_state

            elif comm_state == crc_1_state:
                buff.append(ch)
                comm_state = crc_2_state

            elif comm_state == crc_2_state: 
                buff.append(ch)
                comm_state = end_state
            
            #print("Cmd rs485 byte recv",ch, comm_state, ser.in_waiting)
        
        end = time.perf_counter()
        msec = (end - begin) 
        
        if comm_state == end_state:
            comm_state = sync_state
            print("ACK recieved:", len(buff))
            print("command time", msec)
            if(buff[1] == command_map['STATUS_ACK']):
                print_status()
            #print("Cmd_ACK: [",''.join('{:02x} '.format(x) for x in buff)[:-1], "]")
            time.sleep(0.005)
            #print(buff)
            return
        #if(comm_state != sync_state): print("ms: ", msec)
        elif comm_state != sync_state and msec > message_timeout:
            missed_packets += 1
            print("command timeout", msec, len(buff), data_recv, data_total)
            print("cmd state: ", comm_state)
            #print("buffer recieved:", buff) 
            print("Cmd fail: [",''.join('{:02x} '.format(x) for x in buff)[:-1], "]")
            comm_state = sync_state
            time.sleep(0.015)
            return

        elif msec > message_timeout:
            print("command timeout", msec)
            print("no cmd state: ", comm_state)
            print("Cmd fail: [",''.join('{:02x} '.format(x) for x in buff)[:-1], "]")
            missed_packets += 1
            time.sleep(0.015)
            return



log_begin = time.perf_counter()
log_end = time.perf_counter()

while True:           
    event, values = window.read(timeout=1) 
    if event != '__TIMEOUT__': print(event, values)       
    cmd_launched = 0

    if event == sg.WIN_CLOSED or event == 'Exit':
        break      
    elif event == '_LOG_TOGGLE_':
        toggle_status()    
    else:
        if event == '_ARM_':
            cmd_launched = 1
            arm()
            #read_cmd()
            pass
        elif event == '_READY_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['READY'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
            #rep = ser.read(5)
            #print(rep.decode('utf-8'))
            #print(bytearray(rep).hex())
            pass
        elif event == '_ABORT_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['ABORT'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
            #rep = ser.read(5)
            #print(rep.decode('utf-8'))
            #print(bytearray(rep).hex())
            pass
        elif event == '_LED_ON_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['LED_ON'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
            #rep = ser.read(5)
            #print(rep.decode('utf-8'))
            #print(bytearray(rep).hex())
        elif event == '_LED_OFF_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['LED_OFF'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
            #rep = ser.read(5)
            #print(rep.decode('utf-8'))
            #print(bytearray(rep).hex())
        elif event == '_STATUS_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['STATUS'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
            #rep = ser.read(6)
            #print(rep.decode('utf-8'))
            #print(bytearray(rep).hex())
        elif event == '_IMU_calib_':
            print("calibrate imu")
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['IMU_CALIB'], 0, 0x20, 0x21])
            print("Cmd: [",''.join('{:02x} '.format(x) for x in cmd)[:-1], "]")
            ser.write(cmd)
            read_cmd()
        elif event == '_FUELING_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['FUELING'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
        elif event == '_STOP_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['STOP_PROG'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
        elif event == '_RESUME_':
            cmd_launched = 1
            cmd = bytearray([0x55, command_map['RESUME_PROG'], 0, 0x20, 0x21])
            ser.write(cmd)
            read_cmd()
        elif event == '_EXEC_':
            cmd_launched = 1
            s = window['_PROG_'].get()
            arr = s.split(',')
            prog = int(arr[0])
            p1 = int(arr[1])
            p2 = int(arr[2])
            p3 = int(arr[3])
            l1 = int(arr[4])
            l2 = int(arr[5])
            cmd = bytearray([0x55, command_map['EXEC_PROG'], 11, prog, 
                             (p1 >> 8) & 0xff, (p1) & 0xff, 
                             (p2 >> 8) & 0xff, (p2) & 0xff, 
                             (p3 >> 8) & 0xff, (p3) & 0xff, 
                             (l1 >> 8) & 0xff, (l1) & 0xff, 
                             (l2 >> 8) & 0xff, (l2) & 0xff, 
                             0x20, 0x21])
            print("Comand sent", cmd)
            ser.write(cmd)
            read_cmd()

    log_end = time.perf_counter()
    if(cmd_launched == 0 and log_status == 1 and log_end - log_begin > log_delay):
        #send status cmd
        log_speed = 1 / (log_end - log_begin)
        log_begin = time.perf_counter()
        cmd = bytearray([0x55, command_map['STATUS'], 0, 0x20, 0x21])
        ser.write(cmd)
        read_cmd()
        #pass
    
    #while ser.in_waiting > 0:
        #print(ser.read(1).decode('utf-8'), end='')


    time.sleep(0.001)

window.close()
