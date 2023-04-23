import sys
import numpy as np
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtGui import QPixmap, QImage
from PyQt5.QtWidgets import QWidget, QApplication, QGraphicsScene
from src.Network.Tcp import get_host_ip
from src.Tools.CommonHelper import CommonHelper
from src.UI.SFEducation import Ui_Form
# 更新命令
# pyuic5 -o src/UI/SFEducation.py src/UI/SFEducation.ui



class UiWidgetLogic(QWidget):
    # 定义信号
    link_signal = pyqtSignal(tuple)  # 连接发送消息信号--包含连接类型，本地ip，本机/目标端口
    disconnect_signal = pyqtSignal()
    send_signal = pyqtSignal(str)  # 发送数据改为16进制
    now_page_signal = pyqtSignal(str)
    frame_num_signal = pyqtSignal(int)
    hand_mode_signal = pyqtSignal(str)
    construct_signal = pyqtSignal(str)
    show_message_signal = pyqtSignal(str, bool)

    # 定义状态
    NoLink = -1
    ServerTcp = 0

    # 定义端口 todo 修改端口这里修改
    PORT = 8266


    def __init__(self, parent=None):
        super().__init__(parent)
        self.myui = Ui_Form()
        self.myui.setupUi(self)
        self.myui.retranslateUi(self)
        self.my_ip = get_host_ip()

        self.net_type = "TCP"
        self.link_flag = self.NoLink
        self.scene = QGraphicsScene()

        # 信号与槽
        self.show_message_signal.connect(self.show_message_handle)
        self.now_page_signal.connect(self.now_page_handle)
        self.frame_num_signal.connect(self.frame_num_handle)
        self.hand_mode_signal.connect(self.hand_mode_handle)
        self.construct_signal.connect(self.construct_handle)




    def show_message_handle(self, text, if_endl):
        if if_endl:
            text += '\n'
        self.myui.MessageShow.appendPlainText(text)

    # 连接函数
    def net_link_handle(self):
        # 只写TCP服务端
        self.show_message_signal.emit("TCP服务端", False)
        self.show_message_signal.emit("连接的ip为=>" + self.my_ip + ' 端口为=>' + str(self.PORT), True)
        self.link_signal.emit((self.ServerTcp, self.my_ip, self.PORT))  # 注意是以元组形式发送，包含连接类型，本地ip，本机/目标端口
        self.link_flag = self.ServerTcp

    # 断开连接函数
    def click_discount(self):
        self.disconnect_signal.emit()
        self.link_flag = self.NoLink
        self.show_message_signal.emit("未连接", True)

    # 发送函数 todo 可能没用， 可以删掉
    def send_link_handle(self):
        if self.link_flag != self.NoLink:
            send_msg = self.myui.SendPlainTextEdit.toPlainText()
            self.send_signal.emit(send_msg)

    def now_page_handle(self, text: str):
        self.myui.now_page.setText(text)

    def frame_num_handle(self, frame_num: int):
        self.myui.frame_lcdNumber.setNum(frame_num)

    def hand_mode_handle(self, hand_mode: str):
        self.myui.hand_mod.setText(hand_mode)

    def construct_handle(self, construct: str):
        self.myui.construct.setText(construct)

    # 视频显示槽函数
    def show_video_signal_handle(self, my_img):
        img_array = np.array(my_img)
        pix_img = QImage(img_array, img_array.shape[1], img_array.shape[0], img_array.shape[1] * 3,
                               QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(pix_img)
        # pixitem = QGraphicsPixmapItem()
        # self.scene.addItem(pixitem.setPixmap(pixmap))
        self.myui.ShowImage_.setScene(self.scene)
        self.scene.addPixmap(pixmap)


if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = UiWidgetLogic()
    styleFile = '../QSS/MacOS.qss'
    qssStyle = CommonHelper.read_qss(styleFile)
    window.setStyleSheet(qssStyle)

    window.show()

    sys.exit(app.exec_())
