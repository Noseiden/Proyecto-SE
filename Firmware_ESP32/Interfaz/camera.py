import threading #Para que siga procesando la cámara independientemente de la GUI
import cv2
from logger import Logger
import time

# Acá en camera.py se hace el procesamiento de imágenes:

class RunCamera(): # Obtener frames de la cámara en paralelo al funcionamiento de GUI
    def __init__(self, src=0, name="Camera_1"):
        self.loggerReport = Logger("logCamera")
        try:
            # Variables globales
            self.name = name
            self.src = src
            self.ret = None
            self.frame = None
            self.stopped = False
            self.loggerReport.logger.info("Init constructor RunCamera") #Para poner mensajes
        except Exception as e:
            self.loggerReport.logger.error("Error in RunCamera " + str(e)) #Poner el error en string
    
    def start(self):
        try:
            self.stream = cv2.VideoCapture(self.src)
            time.sleep(1) #Por si se demora en arrancar la cámara
            self.ret, self.frame = self.stream.read() #Guardar los frames del video
            if(self.stream.isOpened()):
                self.loggerReport.logger.info("Creating Thread in start camera")
                self.my_thread = threading.Thread(target=self.get, name=self.name, daemon=True)
                self.my_thread.start()
            else:
                self.loggerReport.logger.warning("start camera no initialized")
        except Exception as e:
            self.loggerReport.logger.error("Error in start Camera " + str(e)) #Poner el error en string
    
    def get(self):
        while not self.stopped:
            if not self.ret:
                pass
            else:
                try:
                    self.ret, self.frame = self.stream.read()
                    # if not self.ret or self.frame is None:
                    #     self.loggerReport.logger.warning("Frame vacío en RunCamera.get()")
                    #     return None
                    # self.count_pieces(self.frame) # Función donde esté el código para el procesamiento del video
                except Exception as e:
                    self.loggerReport.logger.error("Error in get Camera " + str(e))
