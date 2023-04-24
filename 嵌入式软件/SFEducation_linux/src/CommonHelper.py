class CommonHelper:
    def __init__(self):
        pass

    @staticmethod
    def read_qss(style):
        """读取QSS样式表的方法"""
        with open(style, "r") as f:
            return f.read()