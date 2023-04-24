import socket
import threading
from time import sleep
from src import StopThreading


# 获取本机ip地址
def get_host_ip() -> str:
    """获取本机IP地址"""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
    finally:
        s.close()
    return ip

'''
tcp相关解释：
地址簇 : AF_INET (IPv4)
类型: SOCK_STREAM (使用 TCP 传输控制协议)
AF_UNIX：地址格式为UNIX路径名
AF_INET：地址格式为主机和端口号
流格式套接字（SOCK_STREAM）--tcp
SOCK_STREAM 有以下几个特征：
1.数据在传输过程中不会消失；
2.数据是按照顺序传输的；
3.数据的发送和接收不是同步的（有的教程也称“不存在数据边界”）。
数据报格式套接字（SOCK_DGRAM）--udp
SOCK_DGRAM 有以下几个特征：
1.强调快速传输而非传输顺序；
2.传输的数据可能丢失也可能损毁；
3.限制每次传输的数据大小；
4.数据的发送和接收是同步的（有的教程也称“存在数据边界”）。
'''
class TcpLogic:
    NoLink = -1
    ServerTCP = 0
    ClientTCP = 1
    InfoSend = 0
    InfoRec = 1

    def __init__(self):
        self.tcp_socket = None
        self.sever_th = None  # 服务端线程
        self.client_socket_list = list()
        self.link_flag = self.NoLink

    # tcp服务端start函数
    def tcp_server_start(self,ip: str, port : int) -> None :
        # 创建一个socket
        self.tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 设置套接字为非阻塞式
        self.tcp_socket.setblocking(False)
        # 绑定地址到套接字
        self.tcp_socket.bind((ip, port)) # todo 当这句代码出问题时，检查端口是否被占用
        # 开始监听，并限制最大连接数
        self.tcp_socket.listen(5)
        # 创建线程
        self.sever_th = threading.Thread(target= self.tcp_server_concurrency)
        self.sever_th.start()

    # tcp服务端并发函数--子线程函数
    def tcp_server_concurrency(self):
        """
        功能函数，供创建线程的方法；
        使用子线程用于监听并创建连接，使主线程可以继续运行，以免无响应
        使用非阻塞式并发用于接收客户端消息，减少系统资源浪费，使软件轻量化
        :return: None
        """
        while True:
            # 接收客户端的套接字对象和地址
            try:  # 轮训看是否有能够连接的对象
                client_socket, client_address = self.tcp_socket.accept()
            except Exception as ret:
                sleep(0.002)
            else:  # 如果没有捕获到异常，说明连接成功了
                client_socket.setblocking(False)
                self.client_socket_list.append((client_socket, client_address))
                print(f"TCP服务端已连接IP:{client_address[0]}端口:{client_address[1]}")
            # 轮询客户端套接字列表，接收数据 --要收30000个字节
            for client, address in self.client_socket_list:
                try:
                    recv_msg = client.recv(4096)  # 缓冲区大小
                except Exception as ret:
                    pass
                else:
                    if recv_msg:
                        info = recv_msg.decode("utf-8")
                    else:
                        client.close()
                        self.client_socket_list.remove((client, address))

    # todo tcp客户端相关函数
    def tcp_client_start(self,ip : str, host: int) ->  None:
        pass

    def tcp_client_concurrency(self, address) -> None:
        pass

    # tcp发送
    def tcp_send(self, send_info: str) -> str:
        """
        功能函数，用于TCP服务端和TCP客户端发送消息
        """
        try:
            # send_info_encoded = bytes.fromhex(send_info)  # "bytes"发送十六进制可能这里改
            send_info_encoded = send_info.encode("utf-8")  # 发送ascii码
            # send_info_encoded = to_hex(send_info) .encode("UTF8")
            # if self.link_flag == self.ServerTCP:
            # 向所有连接的客户端发送消息
            for client,address in self.client_socket_list:
                client.send(send_info_encoded)
                # client.send(b'\xfb\xfa\x01\x01DC\x00\x01\x32\x34')
                return "TCP服务端已发送"
        except Exception as ret:
            return "发送失败"

    # tcp关闭
    def tcp_close(self) -> None:
        """
        功能函数，关闭网络连接的方法
        """
        # if self.link_flag == self.ServerTCP:
        for client, address in self.client_socket_list:
            client.shutdown(socket.SHUT_RDWR)
            client.close()
            self.client_socket_list = list()
            self.tcp_socket.close()
            print("已断开")

        try:
            StopThreading.stop_thread(self.sever_th)
        except Exception as ret:
            pass
