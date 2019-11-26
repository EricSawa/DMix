from kivy.app import App
from kivy.uix.button import Button
from kivy.uix.label import Label

class TestApp(App):
    def build(self):
        return Button(text='Hello World')

if __name__ == "__main__":
    TestApp().run()