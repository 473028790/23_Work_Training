# Edge Impulse - OpenMV Object Detection Example

import sensor, image, time, os, tf, math, uos, gc
from pyb import UART


uart = UART(3, 115200)
uart.init(115200, bits=8, parity=None, stop=1)

sensor.reset()                         # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (320x240)
sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.

net = None
labels = None
min_confidence = 0.5

# 用于存储接收到的数据位
received_data = bytearray()
# 接收状态标志
receiving = False
# 使用字典来统计数字出现的次数
count_dict = {}
my_array = []

motor_flag=1;
motor_flag_last=1;
try:
    # load the model, alloc the model file on the heap if we have at least 64K free after loading
    net = tf.load("trained.tflite", load_to_fb=uos.stat('trained.tflite')[6] > (gc.mem_free() - (64*1024)))
except Exception as e:
    raise Exception('Failed to load "trained.tflite", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

try:
    labels = [line.rstrip('\n') for line in open("labels.txt")]
except Exception as e:
    raise Exception('Failed to load "labels.txt", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

colors = [ # Add more colors if you are detecting more than 7 types of classes at once.
    (255,   0,   0),
    (  0, 255,   0),
    (255, 255,   0),
    (  0,   0, 255),
    (255,   0, 255),
    (  0, 255, 255),
    (255, 255, 255),
]

clock = time.clock()
while(True):
    clock.tick()
    img = sensor.snapshot()

    if uart.any():
        data = uart.readchar()
        if data == 0x81:
            # 收到帧头，开始接收数据
            received_data = bytearray()
            received_data.append(data)
            receiving = True
        elif receiving:
            # 继续接收数据位
            received_data.append(data)
            if data == 0x7C:
                # 收到帧尾，数据接收完成
                # 数据位有效性校验（可以根据需要进行更复杂的校验）
                if len(received_data) == 3:
                    # 打印接收到的数据
                    motor_flag_last=motor_flag;
                    motor_flag=received_data[1];
                    #print("Received Data:", received_data[1])
                # 重置接收状态
                receiving = False

    if motor_flag == 2:
        for i, detection_list in enumerate(net.detect(img, thresholds=[(math.ceil(min_confidence * 255), 255)])):
            if (i == 0): continue # background class
            if (len(detection_list) == 0): continue # no detections for this class?

            my_array.append(i)
            if len(my_array) == 5:
                for num in my_array:
                    if num in count_dict:
                        count_dict[num] += 1
                    else:
                        count_dict[num] = 1
                # 找到出现次数最多的数字
                most_common_number = max(count_dict, key=count_dict.get)
                print("Most common i:", most_common_number)

                if motor_flag_last==1 and motor_flag==2 :
                    data = bytearray([0x80,most_common_number,0x7E])
                    uart.write(data)
                    motor_flag=1
                    # 清空数组
                    my_array.clear()
                    most_common_number=0

            for d in detection_list:
                [x, y, w, h] = d.rect()
                center_x = math.floor(x + (w / 2))
                center_y = math.floor(y + (h / 2))

                img.draw_circle((center_x, center_y, 12), color=colors[i], thickness=2)
