import sys
import numpy as np
from PIL.PpmImagePlugin import PpmImageFile
from PyQt5.QtCore import QSize, Qt, pyqtSignal, QRect
from PyQt5.QtGui import QPixmap, QImage, QColor
from PyQt5.QtWidgets import QWidget, QApplication, QFileDialog, QGraphicsScene, QMessageBox
from src.UI.queele_ui import Ui_Form
from PIL import Image, ImageChops,ImageQt
from src.PictureProcess.mygraphics import GraphicsView, GraphicsPixmapItem
from src.Network.Tcp import get_host_ip
from src.UI.queele_ui import Ui_Form


class UiWidgetLogic(QWidget):
    # 定义信号
    link_signal = pyqtSignal(tuple)  # 连接发送消息信号--包含连接类型，本地ip，本机/目标端口
    disconnect_signal = pyqtSignal()
    send_signal = pyqtSignal(str)  # 发送数据改为16进制
    counter_signal = pyqtSignal(int, int)  # send/receive
    send_imglist_signal = pyqtSignal()
    brightness_signal = pyqtSignal(int)
    contrast_signal = pyqtSignal(int)
    sharpness_signal = pyqtSignal(int)

    # 定义状态
    NoLink = -1
    ServerTcp = 0

    doudong_signal = pyqtSignal()
    btow_signal = pyqtSignal()
    transpose_signal = pyqtSignal(int)

    NOTRANSPOSE = 0
    LEFT_RIGHT = 1
    TOP_BOTTOM = 2
    DIAGONAL = 3
    OUTUPPER = 4
    OUTESP = 5

    def __init__(self, parent=None):
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

        self.scene = None
        self.mul_color_flag = None
        self.bin_color_flag = None
        self.crop_img = None

        self.myui.OpenPicFileButton.clicked.connect(self.get_picdir_handle)
        self.myui.CroppushButton.clicked.connect(self.picture_crop_handle)
        self.myui.PicProcessConfirm.clicked.connect(self.confirm_signal_handle)
        self.myui.DouDongpushButton.clicked.connect(self.doudong_signal_handle)
        self.myui.BtoWcheckBox.clicked.connect(self.btow_signal_handle)
        self.myui.UpDowncheckBox.clicked.connect(self.up_down_handle)
        self.myui.LeftRightcheckBox.clicked.connect(self.left_right_handle)
        self.myui.DiagonalcheckBox.clicked.connect(self.diagonal_handle)
        self.myui.BrightnesshorizontalSlider.valueChanged.connect(self.brightness_signal_handle)
        self.myui.ContrasthorizontalSlider.valueChanged.connect(self.contrast_signal_handle)
        self.myui.SharpnesshorizontalSlider.valueChanged.connect(self.sharpness_signal_handle)

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

    """----------------图片函数--------------------"""
    # resize判断函数
    def showimg_resize(self, my_img):
        win_width = self.myui.ShowImage.width()
        win_height = self.myui.ShowImage.height()
        imgProportion = my_img.width / my_img.height
        # resize造成图片损坏
        if win_height * imgProportion < win_width:
            my_img = my_img.resize((int(win_height * imgProportion * 0.9), int(win_height * 0.9)), Image.ANTIALIAS)
        else:
            my_img = my_img.resize((int(win_width * 0.9), int(win_width / imgProportion * 0.9)), Image.ANTIALIAS)
        return my_img

    # 获取图片路径槽函数
    def get_picdir_handle(self):
        def send_filedir(file_dir):
            self.return_status()
            self.mul_color_flag = True
            self.bin_color_flag = False
            self.crop_img = Image.open(file_dir)
            self.crop_img = self.showimg_resize(self.crop_img)
            self.showimg_signal_handle(self.crop_img)  # .resize((400, 300))

        # 实现读取路径
        fd = QFileDialog(self, "选择图片", "./", "图片文件(*, *)")
        fd.setAcceptMode(QFileDialog.AcceptOpen)  # 打开模式
        fd.setFileMode(QFileDialog.ExistingFile)  # 设置要操作的对象是文件
        fd.fileSelected.connect(send_filedir)
        fd.open()

    # 图片裁剪槽函数
    def picture_crop_handle(self):
        if self.myui.ShowImage.image_item.is_start_cut:
            self.myui.ShowImage.image_item.is_start_cut = False
            self.myui.ShowImage.image_item.setCursor(Qt.ArrowCursor)  # 箭头光标
        else:
            self.myui.ShowImage.image_item.is_start_cut = True
            self.myui.ShowImage.image_item.setCursor(Qt.CrossCursor)  # 十字光标

    # 图片显示槽函数
    def showimg_signal_handle(self, my_img):
        img_array = np.array(my_img)
        if self.mul_color_flag:
            pix_img = QImage(img_array, img_array.shape[1], img_array.shape[0], img_array.shape[1] * 3,
                                   QImage.Format_RGB888)
        elif self.bin_color_flag:
            pix_img = QImage(img_array, img_array.shape[1], img_array.shape[0], img_array.shape[1],
                                   QImage.Format_Grayscale8)
        pixmap = QPixmap.fromImage(pix_img)
        self.myui.ShowImage.graphicsview_setting(pixmap)

    # 确定按钮槽函数
    def confirm_signal_handle(self):
        if self.myui.ShowImage.save_signal:
            self.myui.ShowImage.save_signal = False
            rect = QRect(self.myui.ShowImage.image_item.start_point.toPoint(),
                         self.myui.ShowImage.image_item.end_point)
            new_pixmap = self.myui.ShowImage.image_item.pixmap().copy(rect)
            # new_pixmap.save(r'E:\ElectricDesign_project\queedle\python_upper\queedle_pyqt\picture\test.png')
            self.crop_img = ImageQt.fromqpixmap(new_pixmap)
            self.showimg_signal_handle(self.crop_img)  # 更新图片
        else:
            self.send_imglist_signal.emit()
            # self.send_link_handle()  # 发送槽函数

    # 抖动算法槽函数
    def doudong_signal_handle(self):
        self.mul_color_flag = False
        self.bin_color_flag = True
        self.doudong_signal.emit()

    # 亮度调整槽函数
    def brightness_signal_handle(self, num: int):
        self.brightness_signal.emit(num)

    # 对比度调整槽函数
    def contrast_signal_handle(self, num: int):
        self.contrast_signal.emit(num)

    # 锐度调整函数
    def sharpness_signal_handle(self, num: int):
        self.sharpness_signal.emit(num)

    # 黑白翻转槽函数
    def btow_signal_handle(self):
        self.btow_signal.emit()

    # 翻转槽函数
    def up_down_handle(self):
        self.transpose_signal.emit(self.TOP_BOTTOM)

    def left_right_handle(self):
        self.transpose_signal.emit(self.LEFT_RIGHT)

    def diagonal_handle(self):
        self.transpose_signal.emit(self.DIAGONAL)

    def return_status(self):
        # 归位
        self.myui.BtoWcheckBox.setCheckState(False)
        self.myui.LeftRightcheckBox.setCheckState(False)
        self.myui.DiagonalcheckBox.setCheckState(False)
        self.myui.UpDowncheckBox.setCheckState(False)
        self.myui.BrightnesshorizontalSlider.setSliderPosition(50)
        self.myui.ContrasthorizontalSlider.setSliderPosition(50)
        self.myui.SharpnesshorizontalSlider.setSliderPosition(0)


if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = UiWidgetLogic()
    window.show()

    sys.exit(app.exec_())
