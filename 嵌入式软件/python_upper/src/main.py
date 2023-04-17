import sys
from PyQt5.QtWidgets import QApplication
from src.Network.net_init import NetworkLogic
from src.PictureProcess.img_trans import ImgTransLogic
from src.UiLogic import UiWidgetLogic
from PIL import Image

# 打包指令 auto-py-to-exe

class CommonHelper:
    def __init__(self):
        pass

    @staticmethod
    def read_qss(style):
        """读取QSS样式表的方法"""
        with open(style, "r") as f:
            return f.read()


class MainWindow(UiWidgetLogic, NetworkLogic, ImgTransLogic):

    def __init__(self, parent=None):
        super().__init__(parent)
        ImgTransLogic.__init__(self)
        self.mainwindow_img_size = None
        self.trans_flag = None

        self.link_signal.connect(self.link_signal_handle)
        self.disconnect_signal.connect(self.disconnect_signal_handle)
        self.send_signal.connect(self.send_signal_handle)

        self.doudong_signal.connect(self.floyd_signal_handle)
        self.brightness_signal.connect(self.brightness_handle)
        self.contrast_signal.connect(self.contrast_handle)
        self.sharpness_signal.connect(self.sharpness_handle)
        self.btow_signal.connect(self.btow_handle)
        self.transpose_signal.connect(self.transpose_handle)
        self.send_imglist_signal.connect(self.send_imglist_handle)

    # 网络连接槽函数
    def link_signal_handle(self, signal) -> None:
        link_type, my_ip, port = signal
        # if link_type == self.ServerTCP:
        self.tcp_server_start(my_ip, port)

    # 断开连接槽函数
    def disconnect_signal_handle(self) -> None:
        self.tcp_close()

    # 发送数据槽函数
    def send_signal_handle(self, msg: str) -> None:
        self.tcp_send(msg)
        self.send_counter += 1
        self.counter_signal.emit(self.send_counter, self.receive_counter)

    def closeEvent(self, event) -> None:
        """
        重写closeEvent方法，实现MainWindow窗体关闭时执行一些代码
        :param event: close()触发的事件
        """
        self.disconnect_signal_handle()

    """----------------图片函数--------------------"""
    # 抖动算法槽函数
    def floyd_signal_handle(self) -> None:
        self.font_to_process_handle()
        self.floyd_steinberg()
        self.processed_img_handle()

    def brightness_handle(self, num: int):
        self.brightness_process(num)
        self.enhance_all()
        self.processed_img_handle()

    def contrast_handle(self, num: int):
        self.contrast_process(num)
        self.enhance_all()
        self.processed_img_handle()

    def sharpness_handle(self, num: int):
        self.sharpness_process(num)
        self.enhance_all()
        self.processed_img_handle()

    def btow_handle(self):
        self.btow_process()
        self.processed_img_handle()

    def transpose_handle(self, mode: int):
        self.transpose_picture(mode)
        self.processed_img_handle()

    # 回传图像到前端展示
    def processed_img_handle(self) -> None:
        self.process_to_font_handle()
        self.showimg_signal_handle(self.crop_img)

    # 前端传图像到后端
    def font_to_process_handle(self):
        self.mainwindow_img_size = self.crop_img.size
        self.trans_flag = False
        if self.crop_img.width < self.crop_img.height:
            self.trans_flag = True
            self.crop_img = self.crop_img.transpose(Image.ROTATE_90)
        self.pre_img = self.crop_img.resize((400, 300))
        self.my_img = self.pre_img

    # 后端传图像到前端
    def process_to_font_handle(self):
        self.crop_img = self.my_img
        if self.trans_flag:
            self.crop_img = self.crop_img.transpose(Image.ROTATE_270)

    # 将图片list发送到网络发送区
    def send_imglist_handle(self):
        out_str = ""
        self.encode_img()
        self.myui.SendPlainTextEdit.clear()
        for i in self.out_list:
            out_str += '{:02x}'.format(i)
        self.myui.SendPlainTextEdit.setPlainText(out_str)


if __name__ == "__main__" :
    app = QApplication(sys.argv)

    window = MainWindow()

    # styleFile = '../QSS/MacOS.qss'
    # qssStyle = CommonHelper.read_qss(styleFile)
    # window.setStyleSheet(qssStyle)

    window.show()

    sys.exit(app.exec_())