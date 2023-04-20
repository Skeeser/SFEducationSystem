import cv2 as cv
import numpy as np
import mediapipe as mp
from numpy import linalg

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

        mp_drawing_styles = mp.solutions.drawing_styles
        self.handLmsStyle = mp_drawing_styles.get_default_hand_landmarks_style()
        self.handConStyle = mp_drawing_styles.get_default_hand_connections_style()

        self.figure = np.zeros(5)
        self.landmark = np.empty((21, 2))
        self.gesture_result = 'none'

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

        frame = cv.cvtColor(frame, cv.COLOR_RGB2BGR)
        return frame, self.gesture_result


    def close_hand_reg(self):
        self.cap.release()
        cv.destroyAllWindows()


if __name__ == '__main__':
    pass
# detect()
