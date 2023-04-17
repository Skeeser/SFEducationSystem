from PIL import Image, ImageChops, ImageEnhance
import numpy as np
from typing import List

class ImgTransLogic:
    NOTRANSPOSE = 0
    LEFT_RIGHT = 1
    TOP_BOTTOM = 2
    DIAGONAL = 3
    OUTUPPER = 4
    OUTESP = 5

    def __init__(self):
        self.pre_img = None  # 彩色
        self.my_img = None  # 黑白
        self.filename = None
        self.btow_flag = 0
        # 亮度、对比度、锐度系数
        self.brightness = 1
        self.contrast = 1
        self.sharpness = 1
        # 旋转角度
        self.rotate_angle = 0

        # 图像大小，裁剪前
        self.PIX_H = 300  # 图像高68
        self.PIX_W = 400  # 图像宽68
        # 输出格式
        self.MODE = 'W'  # H/W，纵向扫描/横向扫描 todo 先定死之后再拓展
        self.NUM_W = 16  # 每行数据个数
        self.out_list = []

    # 打开图片
    def open_picture(self, img_path: str):
        # 创建大小缩放图片
        self.my_img = Image.open(img_path).rotate(self.rotate_angle).resize((self.PIX_W, self.PIX_H)) # todo 打开后还要对大小进行处理
        print('img_file:', self.my_img.format, self.my_img.mode, self.my_img.size, self.my_img.palette)

    # 抖动算法
    def floyd_steinberg(self):
        self.my_img = self.my_img.convert('1')

    # 亮度调整
    def brightness_process(self, num: int):
        self.brightness = num / 50

    # 对比度调整
    def contrast_process(self, num: int):
        self.contrast = num / 50

    # 锐度调整
    def sharpness_process(self, num: int):
        self.sharpness = num / 50 + 1

    # 总的图片调整
    def enhance_all(self):
        enh_bri = ImageEnhance.Brightness(self.pre_img)
        self.my_img = enh_bri.enhance(self.brightness)
        enh_con = ImageEnhance.Contrast(self.my_img)
        self.my_img = enh_con.enhance(self.contrast)
        enh_sha = ImageEnhance.Sharpness(self.my_img)
        self.my_img = enh_sha.enhance(self.sharpness)
        self.floyd_steinberg()

    # 图形翻转函数
    def transpose_picture(self, mode: int):
        if mode == self.NOTRANSPOSE:
            pass
        elif mode == self.LEFT_RIGHT:
            self.my_img = self.my_img.transpose(Image.FLIP_LEFT_RIGHT)
        elif mode == self.TOP_BOTTOM:
            self.my_img = self.my_img.transpose(Image.FLIP_TOP_BOTTOM)
        elif mode == self.DIAGONAL:
            self.my_img = self.my_img.transpose(Image.FLIP_LEFT_RIGHT)
            self.my_img = self.my_img.transpose(Image.FLIP_TOP_BOTTOM)

    # 黑白翻转
    def btow_process(self):
        self.btow_flag = 1
        self.my_img = ImageChops.invert(self.my_img)

    def to_hex(self, pp: List[int]):
        temp111 = np.uint8(0);
        for i in range(0, 8, 1):
            temp111 = temp111 << 1
            if i < (len(pp)):
                temp111 += pp[i]
            else:
                temp111 += 1 - self.btow_flag
        # print(temp111)
        return np.uint8(temp111)

    # 编码函数
    def encode_img(self):
        self.out_list.clear()
        # 创建图像转数组
        img_array = np.array(self.my_img.transpose(Image.FLIP_TOP_BOTTOM))  # 默认上下翻转一次
        # 创建%8扩容存储二值化的数组np.empty
        height, width = img_array.shape
        width_2 = width % 8
        width_8 = int(width / 8) + (1 if width_2 != 0 else 0)
        # 输出C51格式
        if self.MODE == 'W':
            for i in range(0, height):
                for k in range(0, width_8):
                    self.out_list.append(int(self.to_hex(img_array[i][k * 8:k * 8 + 8 if k * 8 + 8 < width else width])))

    # 保存编码函数
    def print_c51(self, save_path, out_mode:int):
        count = 0
        if out_mode == self.OUTESP:
            # todo 之后可以自由控制
            with open(save_path, 'w') as file_object:
                file_object.write("const unsigned char gImage_three_fingers[618] PROGMEM = { \n")
                for i in self.out_list:
                    file_object.write('0x{:02x},'.format(i))
                    count += 1
                    if (count % self.NUM_W == 0):
                        file_object.write('\n')
                file_object.write('};\n')
        elif out_mode == self.OUTUPPER:
            filename = 'C:\\Desktop\\data.txt'
            with open(filename, 'w') as file_object:
                for i in self.out_list:
                    file_object.write('0x{:02x},'.format(i))

    # 解码函数
    def decode_img(self):
        width_t = self.PIX_W
        height_t = self.PIX_H
        bmp_list = self.out_list
        if self.MODE == 'W':
            if width_t % 8 != 0:
                width_t = width_t + (8 - width_t % 8)
            ck_array = np.empty(shape=(height_t, width_t), dtype=np.uint8)
            width_t = int(width_t / 8)
            for i in range(0, height_t):
                for k in range(0, width_t):
                    for p in range(0, 8):
                        ck_array[i][(k + 1) * 8 - p - 1] = bmp_list[i * width_t + k] % 2
                        bmp_list[i * width_t + k] /= 2
        ck_img = Image.fromarray(255 * ck_array)
        # ck_img.save('E:\cv_projects\\CkeckBMP.bmp')  # 预览 todo 图像显示在这里之后去掉
        ck_img.show()

    # 关闭图像
    def close_picture(self):
        self.my_img.close()


if __name__ == "__main__":
    # IMG_PATH = 'C:\\Desktop\\hand-three-fingers(1).jpg'
    # SAVE_PATH = 'D:\AllMyProject\ElectricDesign_project\嵌入式国赛\SFEducationSystem\终端\ESP-12F\queedle\include\\three_finger.h'
    imgc = ImgTransLogic()
    imgc.open_picture(IMG_PATH)
    imgc.floyd_steinberg()
    imgc.encode_img()
    imgc.print_c51(SAVE_PATH, 5)
    imgc.decode_img()
    imgc.close_picture()
