import sys
import time
import cv2
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QApplication
from PIL import Image
from src.AllFunc.NewsGet import NewsGet
from src.Network.net_init import NetworkLogic
from src.Ocr.hand_reg import HandReg
from src.Tools.CommonHelper import CommonHelper
from src.UiLogic import UiWidgetLogic
from src.Ocr.ocr_run import FingerOcr2Voice
# 打包指令 auto-py-to-exe


class MainWindow(UiWidgetLogic, NetworkLogic):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.now_page = 'Menu'
        self.construct = 'Enter'
        self.num_count = [0, "None"]
        self.last_page = 'none'
        self.last_construct = 'none'
        self.last_time = time.time()
        self.frame = None

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.while_func)

        self.link_signal.connect(self.link_signal_handle)
        self.disconnect_signal.connect(self.disconnect_signal_handle)
        self.send_signal.connect(self.send_signal_handle)

        # 系统一启动就执行
        # 连接网络
        self.net_link_handle()
        self.start()
        self.hand_reg = HandReg()
        self.news_get = NewsGet()
        self.ocr = FingerOcr2Voice()

    def start(self):
        self.timer.start(10)

    # 网络连接槽函数
    def link_signal_handle(self, signal) -> None:
        link_type, my_ip, port = signal
        self.tcp_server_start(my_ip, port)

    # 断开连接槽函数
    def disconnect_signal_handle(self) -> None:
        self.tcp_close()

    # 发送数据槽函数
    def send_signal_handle(self, msg: str) -> None:
        self.show_message_signal.emit("尝试发送=> " + msg, True)
        return_msg = self.tcp_send(msg)
        self.show_message_signal.emit(return_msg, True)
        if return_msg == "发送失败":
            time.sleep(0.5)
            # 重试
            self.show_message_signal.emit("重新发送", True)
            self.send_signal_handle(msg)

    def closeEvent(self, event) -> None:
        """
        重写closeEvent方法，实现MainWindow窗体关闭时执行一些代码
        :param event: close()触发的事件
        """
        self.disconnect_signal_handle()
        self.hand_reg.close_hand_reg()
        self.timer.stop()

    def news_get_signal_handle(self):
        pass

    def now_page_signal_handle(self, hand_num: str):
        # 设定三秒才能更新
        if (time.time() - self.last_time) > 2:
            self.last_time = time.time()
            if self.now_page == "Menu":
                self.construct = "Enter"
                if hand_num == "one":
                    self.now_page = "News"
                elif hand_num == "two":
                    self.now_page = "Ocr"
                elif hand_num == "three":
                    self.now_page = "Chat"
            elif self.now_page == "News":
                self.construct = "Enter"
                if hand_num == "one":
                    self.construct = "Next"
                elif hand_num == "two":
                    self.construct = "Back"
                    self.now_page = "Menu"
            elif self.now_page == "Ocr":
                self.construct = "Enter"
                if hand_num == "one":
                    self.construct = "Next"
                elif hand_num == "two":
                    self.construct = "Back"
                    self.now_page = "Menu"
            elif self.now_page == "Chat":
                self.construct = "Enter"
                if hand_num == "one":
                    self.construct = "Next"
                elif hand_num == "two":
                    self.construct = "Back"
                    self.now_page = "Menu"

            self.now_page_signal.emit(self.now_page)
            self.construct_signal.emit(self.construct)

    def action_judge(self):
        if self.last_construct != self.construct:
            self.last_construct = self.construct
            self.send_signal.emit('head ' + self.last_page + ' ' + self.construct + '\n')

        if self.last_page != self.now_page:
            self.last_page = self.now_page
            if self.now_page == "News":
                # 发送新闻
                news = self.news_get.getNewsStr()
                self.send_signal.emit(news + '\n')
            elif self.now_page == "Ocr":
                # ocr处理
                self.frame, text = self.ocr.recognize(self.frame)
                self.send_signal.emit(text + '\n')
        else:
            if self.now_page == "Ocr" and self.construct == "Retry":
                self.frame, text = self.ocr.recognize(self.frame)
                self.send_signal.emit(text + '\n')

    def while_func(self):
        self.frame, hand_num = self.hand_reg.detect()

        if self.num_count[1] == hand_num:
            self.num_count[0] += 1
        else:
            self.num_count[1] = hand_num
            self.num_count[0] = 0
        if self.num_count[0] == 5:
            self.num_count[0] = 0
            self.now_page_signal_handle(hand_num)
            self.hand_mode_signal.emit(hand_num)

        self.action_judge(self.frame)
        self.show_video_signal_handle(self.frame)



if __name__ == "__main__" :
    app = QApplication(sys.argv)

    window = MainWindow()

    styleFile = '../QSS/MacOS.qss'
    qssStyle = CommonHelper.read_qss(styleFile)
    window.setStyleSheet(qssStyle)

    window.show()

    sys.exit(app.exec_())