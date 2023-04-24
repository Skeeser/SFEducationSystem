import sys
import os
import aiml

class Chat:
    def __init__(self):
        _alice_path = self._get_module_dir('aiml') + '\\botdata\\alice'
        os.chdir(_alice_path)
        self._alice = aiml.Kernel()
        self._alice.learn("startup.xml")
        self._alice.respond('LOAD ALICE')

    def _get_module_dir(self, name):
        if os.environ.get('show-info', False):
            print('module', sys.modules[name])
        path = getattr(sys.modules[name], '__file__', None)
        if os.environ.get('show-info', False):
            print(path)
        if not path:
            raise AttributeError('module %s has not attribute __file__' % name)
        return os.path.dirname(os.path.abspath(path))

    def english_chat(self, message: str):
        return self._alice.respond(message)


if __name__ == '__main__':
    chat = Chat()
    while True:
        msg = input('Please input, input q to quit: ')
        if msg == 'q':
            sys.exit(0)
        print('Robot:', chat.english_chat(msg))
