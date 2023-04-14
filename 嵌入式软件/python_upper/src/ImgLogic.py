import sys

import numpy as np
from PIL.PpmImagePlugin import PpmImageFile
from PyQt5.QtCore import QSize, Qt, pyqtSignal, QRect
from PyQt5.QtGui import QPixmap, QImage, QColor
from PyQt5.QtWidgets import QWidget, QApplication, QFileDialog, QGraphicsScene
from src.UI.queele_ui import Ui_Form
from PIL import Image, ImageChops,ImageQt
from src.PictureProcess.mygraphics import GraphicsView, GraphicsPixmapItem


# 图片处理界面逻辑
class ImgWidgetLogic(QWidget):
    send_filedir_signal = pyqtSignal(str)
    # picture_crop_signal = pyqtSignal(tuple)  # x起始值，y起始值，x结束值，y结束值
    doudong_signal = pyqtSignal()

    def __init__(self):
        super().__init__()
        self.scene = None
        self.mul_color_flag = None
        self.bin_color_flag = None
        self.crop_img = None
        self.myui = Ui_Form()
        self.myui.setupUi(self)
        self.myui.retranslateUi(self)

        self.myui.OpenPicFileButton.clicked.connect(self.get_picdir_handle)
        self.myui.CroppushButton.clicked.connect(self.picture_crop_handle)
        self.myui.PicProcessConfirm.clicked.connect(self.confirm_signal_handle)
        self.myui.DouDongpushButton.clicked.connect(self.doudong_signal_handle)
        # self.myui.BtoWcheckBox.clicked.connect()

    # 获取图片路径槽函数
    def get_picdir_handle(self):
        def send_filedir(file_dir):
            self.mul_color_flag = True
            self.bin_color_flag = False
            self.showimg_signal_handle(Image.open(file_dir))  # .resize((400, 300))
            self.send_filedir_signal.emit(file_dir)

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
        win_width = self.myui.ShowImage.width()
        win_height = self.myui.ShowImage.height()

        imgProportion = my_img.width / my_img.height
        if win_height * imgProportion < win_width:
            my_img = my_img.resize((int(win_height * imgProportion), win_height))
        else:
            my_img = my_img.resize((win_width, int(win_width / imgProportion)))

        img_array = np.array(my_img)
        if self.mul_color_flag:
            self.crop_img = QImage(img_array, img_array.shape[1], img_array.shape[0], img_array.shape[1]*3, QImage.Format_RGB888)
        elif self.bin_color_flag:
            self.crop_img = QImage(img_array, img_array.shape[1], img_array.shape[0], img_array.shape[1], QImage.Format_Grayscale8)
        pixmap = QPixmap.fromImage(self.crop_img)
        self.myui.ShowImage.graphicsview_setting(pixmap)

    # 确定按钮槽函数
    def confirm_signal_handle(self):
        if self.myui.ShowImage.save_signal:
            rect = QRect(self.myui.ShowImage.image_item.start_point.toPoint(),
                         self.myui.ShowImage.image_item.end_point)
            new_pixmap = self.myui.ShowImage.image_item.pixmap().copy(rect)
            # new_pixmap.save(r'E:\ElectricDesign_project\queedle\python_upper\queedle_pyqt\picture\test.png')
            self.crop_img = ImageQt.fromqpixmap(new_pixmap)
            # crop_img.show()
            self.showimg_signal_handle(self.crop_img)  # 更新图片

    # 抖动算法槽函数
    def doudong_signal_handle(self):
        self.mul_color_flag = False
        self.bin_color_flag = True
        self.doudong_signal.emit()


if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = ImgWidgetLogic()
    window.show()

    sys.exit(app.exec_())
