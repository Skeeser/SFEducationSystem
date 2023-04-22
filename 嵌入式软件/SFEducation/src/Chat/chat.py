import sys
import os
import warnings
import aiml
from src.Ocr.ocr_run import VoiceOfOcr

def _get_module_dir(name):
    if os.environ.get('show-info', False):
        print('module', sys.modules[name])
    path = getattr(sys.modules[name], '__file__', None)
    if os.environ.get('show-info', False):
        print(path)
    if not path:
        raise AttributeError('module %s has not attribute __file__' % name)
    return os.path.dirname(os.path.abspath(path))

_alice_path = _get_module_dir('aiml') + '\\botdata\\alice'
os.chdir(_alice_path)
_alice = aiml.Kernel()
_alice.learn("startup.xml")
_alice.respond('LOAD ALICE')

def english_chat(message: str):
    return _alice.respond(message)

if __name__ == '__main__':
    while True:
        msg = input('Please input, input q to quit: ')
        if msg == 'q':
            sys.exit(0)
        print('Robot:', english_chat(msg))
