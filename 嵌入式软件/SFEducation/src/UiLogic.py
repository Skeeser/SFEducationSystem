import sys
import numpy as np
from PIL.PpmImagePlugin import PpmImageFile
from PyQt5.QtCore import QSize, Qt, pyqtSignal, QRect
from PyQt5.QtGui import QPixmap, QImage, QColor
from PyQt5.QtWidgets import QWidget, QApplication, QFileDialog, QGraphicsScene, QMessageBox
from src.Tools.CommonHelper import CommonHelper

from src.UI.SFEducation import Ui_Form
# from PIL import Image, ImageChops,ImageQt
# from src.PictureProcess.mygraphics import GraphicsView, GraphicsPixmapItem
# from src.Network.Tcp import get_host_ip
# from src.UI.queele_ui import Ui_Form

# 更新命令
# pyuic5 -o src/UI/SFEducation.py src/UI/SFEducation.ui



class UiWidgetLogic(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.myui = Ui_Form()
        self.myui.setupUi(self)
        self.myui.retranslateUi(self)


if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = UiWidgetLogic()
    styleFile = '../QSS/MacOS.qss'
    qssStyle = CommonHelper.read_qss(styleFile)
    window.setStyleSheet(qssStyle)

    window.show()

    sys.exit(app.exec_())
