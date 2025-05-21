import sys
from PyQt5.QtWidgets import QApplication, QWidget
from PyQt5.QtCore import Qt, QTimer, QPoint
from PyQt5.QtGui import QPainter, QColor, QRegion, QGuiApplication

class Overlay(QWidget):
    def __init__(self):
        super().__init__()

        # 设置无边框、最前、透明、鼠标穿透
        self.setWindowFlags(
            Qt.FramelessWindowHint |
            Qt.WindowStaysOnTopHint |
            Qt.Tool |
            Qt.WindowTransparentForInput
        )
        self.setAttribute(Qt.WA_TranslucentBackground)
        self.setAttribute(Qt.WA_NoSystemBackground, True)

        self.dot_size = 8
        self.move_to_center()

        # 定时器保持窗口位置
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.move_to_center)
        self.timer.start(1000)  # 每秒重新定位，避免多屏错位

        self.show()

    def move_to_center(self):
        screen = QGuiApplication.primaryScreen().geometry()
        center_x = screen.width() // 2 - self.dot_size // 2
        center_y = screen.height() // 2 -255  - self.dot_size // 2
        self.setGeometry(center_x, center_y, self.dot_size, self.dot_size)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setBrush(QColor(0, 255, 255))  
        painter.setPen(Qt.NoPen)
        painter.drawEllipse(0, 0, self.dot_size, self.dot_size)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    overlay = Overlay()
    sys.exit(app.exec_())
