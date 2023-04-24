# 依赖：

```
pip install pytesseract

sudo apt-get install tesseract
```



```
pip install mediapipe
```



```
pip install pyttsx3
import pyttsx3
```



```
pip install bs4
```

```
pip install aiml
```

找到Kernel.py，我的Kernel.py在
F:\anaconda3\lib\site-packages\aiml下。将第335行

start = time.clock()
1
改为:

start = time.time()
1
然后把第356行

print( "done (%.2f seconds)" % (time.clock() - start) )
1
改为:

print( "done (%.2f seconds)" % (time.time() - start) )























语音识别是计算机软件识别口语中的单词和短语，并将其转换为可读文本的能力。那么如何在 Python 中将语音转换为文本？如何使用 SpeechRecognition 库在 Python 中将语音转换为文本？我们不需要从头开始构建任何机器学习模型，该库为我们提供了各种著名的公共语音识别 API 的便捷包装。

使用 pip 安装库：

```
pip3 install SpeechRecognition
```

Okey，打开一个新的 Python 文件并导入它：

```
import speech_recognition as sr
```

从文件读取

确保当前目录中有一个包含英语语音的音频文件 (如果您想跟我一起学习，请在此处获取音频文件)：

```
filename = “speech.wav”
```

该文件是从 LibriSpeech 数据集中获取的，但是您可以带上任何想要的东西，只需更改文件名，就可以初始化语音识别器：

```
# initialize the recognizer

r = sr.Recognizer()
```

以下代码负责加载音频文件，并使用 Google Speech Recognition 将语音转换为文本：

```
# open the filewith sr.AudioFile(filename) as source:www.zpedu.com/

# listen for the data (load audio to memory)

audio_data = r.record(source)

# recognize (convert from speech to text)

text = r.recognize_google(audio_data)

print(text)
```

这需要几秒钟才能完成，因为它将文件上传到 Google 并获取了输出，这是我的结果：

I believe you’re just talking nonsense



从麦克风读取

这需要在您的计算机中安装 PyAudio，以下是取决于您的操作系统的安装过程：

视窗

您可以点安装它：

```
pip3 install pyaudio
```

Linux

您需要首先安装依赖项：

```
sudo apt-get install python-pyaudio python3-pyaudio

pip3 install pyaudio
```

苹果系统

您需要先安装 portaudio，然后才可以点安装它：

brew install portaudio

pip3 install pyaudio

现在，让我们使用麦克风转换语音：

```
with sr.Microphone() as source:

# read the audio data from the default microphone

audio_data = r.record(source, duration=5)

print(“Recognizing…”)

# # convert speech to text

text = r.recognize_google(audio_data)

print(text)
```

这会从您的麦克风听到 5 秒钟，然后尝试将该语音转换为文本！

它与先前的代码非常相似，但是我们在这里使用 Microphone () 对象从默认麦克风读取音频，然后在 record () 函数中使用 duration 参数在 5 秒后停止读取，然后上传音频数据向 Google 获取输出文本。

您还可以在 record () 函数中使用 offset 参数在偏移几秒钟后开始记录。

另外，您可以通过将语言参数传递给 accept_google () 函数来识别不同的语言。例如，如果您想识别西班牙语语音，则可以使用：

text = r.recognize_google(audio_data, language=”es-ES”)
