# 导入urlopen函数
from urllib.request import urlopen
# 导入BeautifulSoup
from bs4 import BeautifulSoup as bf

class NewsGet:
    def __init__(self):
        # 发出请求，获取html
        html = urlopen("https://www.globaltimes.cn/world/")
        # 用BeautifulSoup解析html
        obj = bf(html.read(), 'html.parser')
        # 从标签head、title里提取标题
        self.new_info = obj.find_all('p')

    def getNewsStr(self):
        ret_news_str = ''
        flag = 0
        # 打印
        for i in self.new_info:
            flag += 1
            # 获取的html内容是字节，将其转化为字符串
            ret_news_str += i.text
            if flag == 5:
                break
            # print(i)
        return ret_news_str

# 英文爬虫
if __name__ == "__main__" :
    mynews = NewsGet()
    print(mynews.getNewsStr())