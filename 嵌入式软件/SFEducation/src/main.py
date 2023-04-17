import sys
from PyQt5.QtWidgets import QApplication
from PIL import Image
from src.Tools.CommonHelper import CommonHelper
from src.UiLogic import UiWidgetLogic
# 打包指令 auto-py-to-exe


class MainWindow(UiWidgetLogic):
    def __init__(self, parent=None):
        super().__init__(parent)

if __name__ == "__main__" :
    app = QApplication(sys.argv)

    window = MainWindow()

    # styleFile = '../QSS/MacOS.qss'
    # qssStyle = CommonHelper.read_qss(styleFile)
    # window.setStyleSheet(qssStyle)

    window.show()

    sys.exit(app.exec_())