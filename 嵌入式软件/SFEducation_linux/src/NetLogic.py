import sys
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QWidget, QApplication, QMessageBox, QFileDialog
from src.Tcp import get_host_ip
from src.UI.queele_ui import Ui_Form


# 网络收发界面逻辑
class NetWidgetLogic(QWidget):
    # 定义信号
    link_signal = pyqtSignal(tuple)  # 连接发送消息信号--包含连接类型，本地ip，本机/目标端口
    disconnect_signal = pyqtSignal()
    send_signal = pyqtSignal(str)  # 发送数据改为16进制
    counter_signal = pyqtSignal(int, int)  # send/receive

    # 定义状态
    NoLink = -1
    ServerTcp = 0

    def __init__(self, parent = None):
        super().__init__(parent)
        self.myui = Ui_Form()
        self.myui.setupUi(self)
        self.myui.retranslateUi(self)

        # self.setWindowFlags(Qt.WindowStaysOnTopHint)  # 保持窗口最前
        self.myui.MyHostAddrLineEdit.setText(get_host_ip())  # 显示本机IP地址

        self.net_type = "TCP"
        self.link_flag = self.NoLink
        self.send_counter = 0
        self.receive_counter = 0
        self.dir = None

        # 信号与槽的连接
        self.myui.ConnectButton.toggled.connect(self.connect_button_toggled_handler)
        self.myui.SendButton.clicked.connect(self.send_link_handle)
        self.counter_signal.connect(self.counter_signal_handle)
        self.myui.OpenFilePushButton.clicked.connect(self.open_file_handle)
        self.myui.CounterResetButton.clicked.connect(self.counter_reset_button_handle)

    # 连接按钮，实现切换
    def connect_button_toggled_handler(self, state):
        if state:
            # 连接函数
            self.click_link_handle()
        else:
            # 断开函数
            self.click_discount()
            self.editable(True)

    # 连接按钮函数
    def click_link_handle(self):
        server_flag = False

        # 用户未输入返回为一,转换端口字符串位数字
        def get_int_port(port):
            if port == "":
                return -1
            else:
                return int(port)

        # 数据读取
        my_ip = str(self.myui.MyHostAddrLineEdit.text())
        my_port = get_int_port(self.myui.MyPortLineEdit.text())
        target_ip = str(self.myui.TargetIPLineEdit.text())
        target_port = get_int_port(self.myui.TargetPortLineEdit.text())
        self.editable(False)

        if my_port == -1 and target_port == -1 and target_ip == "":
            mb = QMessageBox(QMessageBox.Critical, "错误", "请输入信息", QMessageBox.Ok, self)
            mb.open()
            self.editable(True)
            self.myui.ConnectButton.setChecked(False)
            return None
        elif my_port != -1 and target_port != -1 and target_ip != "":
            mb = QMessageBox(
                QMessageBox.Critical, "错误", "输入的信息过多", QMessageBox.Ok, self
            )
            mb.open()
            self.editable(True)
            self.myui.ConnectButton.setChecked(False)
            return None
        elif my_port != -1 and target_ip == "":
            server_flag = True

        # 网络类型判断
        '''只写TCP服务端'''
        if self.net_type == "TCP" and server_flag:
            self.link_signal.emit((self.ServerTcp, my_ip, my_port)) # 注意是以元组形式发送，包含连接类型，本地ip，本机/目标端口
            self.link_flag = self.ServerTcp
            self.myui.StateLabel.setText("TCP服务端")

    # 断开连接函数
    def click_discount(self):
        self.disconnect_signal.emit()
        self.link_flag = self.NoLink
        self.myui.StateLabel.setText("未连接")

    # 限制修改函数
    def editable(self, able : bool = True):
        # 限制只读
        self.myui.ProtocolTypeComboBox.setDisabled(not able)
        self.myui.MyHostAddrLineEdit.setReadOnly(not able)
        self.myui.MyPortLineEdit.setReadOnly(not able)
        self.myui.TargetIPLineEdit.setReadOnly(not able)
        self.myui.TargetPortLineEdit.setReadOnly(not able)

    # 发送函数
    def send_link_handle(self):
        if self.link_flag != self.NoLink:
            # 循环还是非循环发送
            loop_flag = self.myui.LoopSendCheckBox.checkState()
            send_msg = self.myui.SendPlainTextEdit.toPlainText()
            if loop_flag == 0:
                self.send_signal.emit(send_msg)
            # TODO 循环发送
            elif loop_flag == 2:
                '''暂不需要'''

    # TODO 接收函数
    def msg_write(self, msg: str):
        pass

    def info_write(self, info: str, mode: int):
        pass

    # 收发计数器函数
    def counter_signal_handle(self, send_count, receive_count):
        self.myui.SendCounterLabel.setText(str(send_count))
        self.myui.ReceiveCounterLabel.setText(str(receive_count))

    # 清空收发计数器函数
    def counter_reset_button_handle(self):
        self.send_counter = 0
        self.receive_counter = 0
        self.counter_signal.emit(self.send_counter, self.receive_counter)

    # 打开文件并加载到窗口函数
    def open_file_handle(self):
        #  TODO 不同状态下的模式切换，判断语句
        # if self.link_flag != self.NoLink:
        def read_file(file_dir):
            if file_dir:
                try:
                    with open(file_dir, "r", encoding="UTF8") as f:  # 此处可能要修改为十六进制
                        self.myui.SendPlainTextEdit.clear()
                        self.myui.SendPlainTextEdit.appendPlainText(f.read())
                except UnicodeDecodeError:
                    #  如果不能用UTF8解码
                    mb = QMessageBox(
                        QMessageBox.Critical,
                        "无法读取文件",
                        "无法读取文件，请检查输入",
                        QMessageBox.Ok,
                        self,
                    )
                    mb.open()

        # 实现读取路径
        fd = QFileDialog(self, "选择文件", "./", "文本文件(*, *)")
        fd.setAcceptMode(QFileDialog.AcceptOpen)  # 打开模式
        fd.setFileMode(QFileDialog.ExistingFile)  # 设置要操作的对象是文件
        fd.fileSelected.connect(read_file)
        fd.open()

        # elif self.link_flag == self.NoLink: 待实现

    # TODO 接收设置保存数据实现函数
    def r_save_data_button_handler(self):
        pass

    def receive_pause_checkbox_toggled_handler(self, ste: bool):
        pass


if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = NetWidgetLogic()
    window.show()

    sys.exit(app.exec_())

