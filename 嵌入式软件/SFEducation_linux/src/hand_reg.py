import cv2 as cv
import numpy as np
import mediapipe as mp
from numpy import linalg
from src.ocr_run import DrawInFrame
import math
# 视频设备号
DEVICE_NUM = 0

class HandReg:
    def __init__(self):
        # 是否镜像
        self.if_filp = False
        self.result = None
        # 接入USB摄像头时，注意修改cap设备的编号
        self.cap = cv.VideoCapture(DEVICE_NUM)
        # 加载手部检测函数
        self.mpHands = mp.solutions.hands
        self.hands = self.mpHands.Hands(min_detection_confidence=0.7,
                                 min_tracking_confidence=0.5,
                                 max_num_hands=2)
        # 加载绘制函数，并设置手部关键点和连接线的形状、颜色
        self.mpDraw = mp.solutions.drawing_utils

        # self.resize_w = 960
        # self.resize_h = 720
        self.drawInfo = None
        self.frame = None

        mp_drawing_styles = mp.solutions.drawing_styles
        self.handLmsStyle = mp_drawing_styles.get_default_hand_landmarks_style()
        self.handConStyle = mp_drawing_styles.get_default_hand_connections_style()

        self.figure = np.zeros(5)
        self.landmark = np.empty((21, 2))
        self.gesture_result = 'none'
        self.drawInfo = DrawInFrame()

        if not self.cap.isOpened():
            print("Can not open camera.")
            exit()

    # 手指检测
    # point1-手掌0点位置，point2-手指尖点位置，point3手指根部点位置
    def finger_stretch_detect(self, point1, point2, point3):
        result = 0
        # 计算向量的L2范数
        dist1 = np.linalg.norm((point2 - point1), ord=2)
        dist2 = np.linalg.norm((point3 - point1), ord=2)
        if dist2 > dist1:
            result = 1
        return result

    def get_result(self):
        return self.result

    # 检测手势
    def detect_hands_gesture(self, result):
        if (result[0] == 0) and (result[1] == 1) and (result[2] == 0) and (result[3] == 0) and (result[4] == 0):
            gesture = "one"
        elif (result[0] == 0) and (result[1] == 1) and (result[2] == 1) and (result[3] == 0) and (result[4] == 0):
            gesture = "two"
        elif (result[0] == 0) and (result[1] == 1) and (result[2] == 1) and (result[3] == 1) and (result[4] == 0):
            gesture = "three"
        else:
            gesture = "none"
        return gesture

    # 循环调用这个函数
    def detect(self):
        ret, frame = self.cap.read()
        if not ret:
            print("Can not receive frame (stream end?). Exiting...")
            self.close_hand_reg()

        if self.if_filp:
            self.frame = cv.flip(self.frame, 0)
            self.frame = cv.flip(self.frame, 1)

        # mediaPipe的图像要求是RGB，所以此处需要转换图像的格式
        frame_RGB = cv.cvtColor(frame, cv.COLOR_BGR2RGB)
        self.result = self.hands.process(frame_RGB)
        # 读取视频图像的高和宽
        frame_height = frame.shape[0]
        frame_width = frame.shape[1]

        # print(result.multi_hand_landmarks)
        # 如果检测到手
        if self.result.multi_hand_landmarks:
            # 为每个手绘制关键点和连接线
            for i, handLms in enumerate(self.result.multi_hand_landmarks):
                self.mpDraw.draw_landmarks(frame,
                                      handLms,
                                      self.mpHands.HAND_CONNECTIONS,
                                      landmark_drawing_spec=self.handLmsStyle,
                                      connection_drawing_spec=self.handConStyle)

                for j, lm in enumerate(handLms.landmark):
                    xPos = int(lm.x * frame_width)
                    yPos = int(lm.y * frame_height)
                    landmark_ = [xPos, yPos]
                    self.landmark[j, :] = landmark_

                # 通过判断手指尖与手指根部到0位置点的距离判断手指是否伸开(拇指检测到17点的距离)
                for k in range(5):
                    if k == 0:
                        figure_ = self.finger_stretch_detect(self.landmark[17], self.landmark[4 * k + 2], self.landmark[4 * k + 4])
                    else:
                        figure_ = self.finger_stretch_detect(self.landmark[0], self.landmark[4 * k + 2], self.landmark[4 * k + 4])

                    self.figure[k] = figure_
                # print(self.figure, '\n')

                self.gesture_result = self.detect_hands_gesture(self.figure)
                # cv.putText(frame, f"{gesture_result}", (30, 60 * (i + 1)), cv.FONT_HERSHEY_COMPLEX, 2, (255, 255, 0), 5)
        else:
            self.gesture_result = 'none'

        self.frame = cv.cvtColor(frame, cv.COLOR_RGB2BGR)
        return self.gesture_result


    def close_hand_reg(self):
        self.cap.release()
        cv.destroyAllWindows()

    def checkHandsIndex(self, handedness):
        if len(handedness) == 1:
            handedness_list = ['Left' if handedness[0].classification[0].label == 'Right' else 'Right']
        else:
            handedness_list = [handedness[1].classification[0].label, handedness[0].classification[0].label]
        return handedness_list

    def recognize(self):
        text = ""
        if self.frame is None:
            print("空帧")
            return text

        resize_w = self.frame.shape[1]
        resize_h = self.frame.shape[0]


        # todo 需要根据镜头位置来调整,动态调整文本角度， 注意要隔一段时间调用
        # self.image = cv2.rotate( self.image, cv2.ROTATE_180)
        # 保存缩略图
        # if isinstance(self.drawInfo.last_thumb_img, np.ndarray):
        #     self.frame = self.drawInfo.generateThumb(self.drawInfo.last_thumb_img, self.frame)

        # 判断是否有手掌
        if self.result.multi_hand_landmarks:

            # 记录左右手index
            handedness_list = self.checkHandsIndex(self.result.multi_handedness)
            hand_num = len(handedness_list)

            self.drawInfo.hand_num = hand_num

            if hand_num == 2:
                # 复制一份干净的原始frame
                frame_copy = self.frame.copy()
                # 遍历每个手掌
                for hand_index, hand_landmarks in enumerate(self.result.multi_hand_landmarks):
                    # 容错
                    if hand_index > 1:
                        hand_index = 1

                    # 解析手指，存入各个手指坐标
                    landmark_list = []

                    # 用来存储手掌范围的矩形坐标
                    paw_x_list = []
                    paw_y_list = []
                    for landmark_id, finger_axis in enumerate(
                            hand_landmarks.landmark):
                        landmark_list.append([
                            landmark_id, finger_axis.x, finger_axis.y,
                            finger_axis.z
                        ])
                        paw_x_list.append(finger_axis.x)
                        paw_y_list.append(finger_axis.y)
                    if landmark_list:
                        # 比例缩放到像素
                        ratio_x_to_pixel = lambda x: math.ceil(x * resize_w)
                        ratio_y_to_pixel = lambda y: math.ceil(y * resize_h)

                        # 设计手掌左上角、右下角坐标
                        paw_left_top_x, paw_right_bottom_x = map(ratio_x_to_pixel,
                                                                 [min(paw_x_list), max(paw_x_list)])
                        paw_left_top_y, paw_right_bottom_y = map(ratio_y_to_pixel,
                                                                 [min(paw_y_list), max(paw_y_list)])

                        # 获取食指指尖坐标
                        index_finger_tip = landmark_list[8]
                        index_finger_tip_x = ratio_x_to_pixel(index_finger_tip[1])
                        index_finger_tip_y = ratio_y_to_pixel(index_finger_tip[2])

                        # 获取中指指尖坐标
                        middle_finger_tip = landmark_list[12]
                        middle_finger_tip_x = ratio_x_to_pixel(middle_finger_tip[1])
                        middle_finger_tip_y = ratio_y_to_pixel(middle_finger_tip[2])

                        # 画x,y,z坐标
                        label_height = 30
                        label_wdith = 130
                        cv.rectangle(self.frame, (paw_left_top_x - 30, paw_left_top_y - label_height - 30),
                                      (paw_left_top_x + label_wdith, paw_left_top_y - 30), (0, 139, 247), -1)

                        l_r_hand_text = handedness_list[hand_index][:1]

                        cv.putText(self.frame,
                                    "{hand} x:{x} y:{y}".format(hand=l_r_hand_text, x=index_finger_tip_x,
                                                                y=index_finger_tip_y),
                                    (paw_left_top_x - 30 + 10, paw_left_top_y - 40),
                                    cv.FONT_HERSHEY_PLAIN, 1, (255, 255, 255), 2)

                        # 给手掌画框框
                        cv.rectangle(self.frame, (paw_left_top_x - 30, paw_left_top_y - 30),
                                      (paw_right_bottom_x + 30, paw_right_bottom_y + 30), (0, 139, 247), 1)

                        # 释放单手模式
                        line_len = math.hypot((index_finger_tip_x - middle_finger_tip_x),
                                              (index_finger_tip_y - middle_finger_tip_y))

                        # if line_len < 50 and handedness_list[hand_index] == 'Right':
                        #     self.drawInfo.clearSingleMode()
                        #     self.drawInfo.last_thumb_img = None

                        # 传给画图类，如果食指指尖停留超过指定时间（如0.3秒），则启动画图，左右手单独画
                        self.frame, last_text = self.drawInfo.checkIndexFingerMove(handedness_list[hand_index],
                                                                              [index_finger_tip_x, index_finger_tip_y],
                                                                              self.frame, frame_copy)
                        if last_text != "":
                            text = last_text

        return text


if __name__ == '__main__':
    pass
# detect()
