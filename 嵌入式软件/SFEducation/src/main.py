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
from src.Chat.chat import Chat
from src.Chat.voice_reg import VoiceRecognize
from src.Ocr.ocr_run import VoiceOfOcr
import threading
from src.Network import StopThreading
# 打包指令 auto-py-to-exe


class MainWindow(UiWidgetLogic, NetworkLogic, HandReg):
    def __init__(self, parent=None):
        super().__init__(parent)
        HandReg.__init__(self)
        self.now_page = 'Menu'
        self.construct = 'Enter'
        self.num_count = [0, "None"]
        self.last_page = 'Menu'
        self.last_construct = 'none'
        self.last_time = time.time()
        self.frame = None
        self.chat_th = None  # 服务端线程

        styleFile = '../QSS/MacOS.qss'
        qssStyle = CommonHelper.read_qss(styleFile)
        self.setStyleSheet(qssStyle)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.while_func)

        self.link_signal.connect(self.link_signal_handle)
        self.disconnect_signal.connect(self.disconnect_signal_handle)
        self.send_signal.connect(self.send_signal_handle)

        # 系统一启动就执行
        # 连接网络
        self.net_link_handle()
        self.start()
        self.news_get = NewsGet()
        self.chat = Chat()
        self.voice_recognize = VoiceRecognize()
        self.pp_voice = VoiceOfOcr()
        self.send_signal.emit('head ' + self.now_page + ' ' + self.construct + '\n')

    def start(self):
        self.timer.start(10)

    # 网络连接槽函数
    def link_signal_handle(self, signal) -> None:
        link_type, my_ip, port = signal
        self.tcp_server_start(my_ip, port)
        # time.sleep(2)


    # 断开连接槽函数
    def disconnect_signal_handle(self) -> None:
        self.tcp_close()

    # 发送数据槽函数
    def send_signal_handle(self, msg: str) -> None:
        self.show_message_signal.emit("尝试发送=> " + msg, False)
        return_msg = self.tcp_send(msg)
        self.show_message_signal.emit(return_msg, False)
        if return_msg == "发送失败":
            # time.sleep(0.5)
            # 重试
            self.show_message_signal.emit("重新发送", False)
            self.send_signal_handle(msg)

    def closeEvent(self, event) -> None:
        """
        重写closeEvent方法，实现MainWindow窗体关闭时执行一些代码
        :param event: close()触发的事件
        """
        self.disconnect_signal_handle()
        self.hand_reg.close_hand_reg()
        self.timer.stop()
        StopThreading.stop_thread(self.chat_th)

    def news_get_signal_handle(self):
        pass

    def now_page_signal_handle(self, hand_num: str):
        # 设定三秒才能更新
        if (time.time() - self.last_time) > 2:
            self.last_time = time.time()
            self.last_construct = self.construct
            self.construct = "Enter"

            if self.now_page == "Menu":
                if hand_num == "one":
                    self.now_page = "News"
                elif hand_num == "two":
                    self.now_page = "Ocr"
                elif hand_num == "three":
                    self.now_page = "Chat"

            elif self.now_page == "News":
                if hand_num == "one":
                    self.construct = "Next"
                elif hand_num == "two":
                    self.construct = "Back"
                    self.now_page = "Menu"
                elif self.last_construct == "Next" and hand_num == "none":
                    self.send_signal.emit('head ' + self.now_page + ' ' + "wait" + '\n')

            elif self.now_page == "Ocr":
                if hand_num == "two":
                    self.construct = "Back"
                    self.now_page = "Menu"
                elif self.last_construct == "Retry" and hand_num == "none":
                    self.send_signal.emit('head ' + self.now_page + ' ' + "wait" + '\n')

            elif self.now_page == "Chat":
                self.construct = "Enter"
                if hand_num == "one":
                    self.construct = "Start"
                elif hand_num == "two":
                    self.construct = "Back"
                    self.now_page = "Menu"
                elif self.last_construct == "Start" and hand_num == "none":

                    self.send_signal.emit('head ' + self.now_page + ' ' + "wait" + '\n')

            if hand_num != "none":
                if self.now_page != "Chat":
                    self.send_signal.emit('head ' + self.now_page + ' ' + self.construct + '\n')
                else:
                    if (self.last_construct != self.construct and self.construct == "Start") or self.construct != "Start":
                        self.send_signal.emit('head ' + self.now_page + ' ' + self.construct + '\n')

            self.now_page_signal.emit(self.now_page)
            self.construct_signal.emit(self.construct)

    def th_chat_func(self):
        voice_text = self.voice_recognize.run()
        response_txt = self.chat.english_chat(voice_text)
        print("Robot=> " + response_txt)
        self.pp_voice.sayaddtext(response_txt)
        self.pp_voice.saystart()
        self.show_message_signal.emit("Usrer=> " + voice_text, False)
        self.show_message_signal.emit("Robot=> " + response_txt, True)
        self.send_signal.emit(voice_text + '\n')
        self.send_signal.emit(response_txt + '\n')

    def action_judge(self):

        if self.last_page != self.now_page:
            self.last_page = self.now_page
            if self.now_page == "News":
                # 发送新闻
                self.show_message_signal.emit("获取新闻中。。。", False)
                news = self.news_get.getNewsStr()
                self.send_signal.emit(news + '\n')
        else:
            if self.now_page == "Ocr":
                # ocr处理
                text = self.recognize()
                if text != "" and text is not None:
                    self.construct = "Retry"
                    self.send_signal.emit('head ' + self.now_page + ' ' + "Retry" + '\n')
                    self.send_signal.emit(text + '\n')
            elif self.now_page == "Chat":
                if self.last_construct != self.construct and self.construct == "Start":
                    self.last_construct = "Start"
                    self.chat_th = threading.Thread(target=self.th_chat_func)
                    self.chat_th.start()


    def while_func(self):
        # 计算刷新率
        fpsTime = time.time()
        hand_num = self.detect()

        if self.num_count[1] == hand_num:
            self.num_count[0] += 1
        else:
            self.num_count[1] = hand_num
            self.num_count[0] = 0
        if self.num_count[0] == 5:
            self.num_count[0] = 0
            self.now_page_signal_handle(hand_num)
            self.hand_mode_signal.emit(hand_num)

        self.action_judge()
        if self.frame is not None:
            self.show_video_signal_handle(self.frame)

        cTime = time.time()
        fps_text = 1 / (cTime - fpsTime)
        self.frame_num_signal.emit(int(fps_text))


if __name__ == "__main__" :
    app = QApplication(sys.argv)

    window = MainWindow()

    window.show()

    sys.exit(app.exec_())