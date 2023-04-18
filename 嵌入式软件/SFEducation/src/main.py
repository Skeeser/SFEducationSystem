import sys
from PyQt5.QtWidgets import QApplication
from PIL import Image
from src.Tools.CommonHelper import CommonHelper
from src.UiLogic import UiWidgetLogic
# 打包指令 auto-py-to-exe


class MainWindow(UiWidgetLogic):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.link_signal.connect(self.link_signal_handle)
        self.disconnect_signal.connect(self.disconnect_signal_handle)
        self.send_signal.connect(self.send_signal_handle)

    # 网络连接槽函数
    def link_signal_handle(self, signal) -> None:
        link_type, my_ip, port = signal
        self.tcp_server_start(my_ip, port)

    # 断开连接槽函数
    def disconnect_signal_handle(self) -> None:
        self.tcp_close()

    # 发送数据槽函数
    def send_signal_handle(self, msg: str) -> None:
        self.show_message_signal("尝试发送=> " + msg, True)
        self.tcp_send(msg)

    def closeEvent(self, event) -> None:
        """
        重写closeEvent方法，实现MainWindow窗体关闭时执行一些代码
        :param event: close()触发的事件
        """
        self.disconnect_signal_handle()

    def now_page_signal_handle(self):
        # now_page_signal.emit()
        pass






if __name__ == "__main__" :
    app = QApplication(sys.argv)

    window = MainWindow()

    styleFile = '../QSS/MacOS.qss'
    qssStyle = CommonHelper.read_qss(styleFile)
    window.setStyleSheet(qssStyle)

    window.show()

    sys.exit(app.exec_())