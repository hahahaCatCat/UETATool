import Singleton
import unreal

class SlateWindow(object):
    def __init__(self):
        __metaclass__ = Singleton
        unreal.PythonSlateTool.init_slate_tool("F:/Unral_projects/PythonPlg/Content/Script/Slate.json")

    def ClickButton_event(self):
        print("helo")
 