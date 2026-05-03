from logger import Logger
import tkinter as tk
from PIL import Image, ImageTk #pip install pillow
import tkinter.font as font
from tkinter import ttk
import cv2 #pip install opencv-python
import camera
import numpy as np

class Application(ttk.Frame):
    def __init__(self, master=None): #Todo lo gráfico irá en el master
        super().__init__(master)
        # variables globales:
        self.logReport = Logger("logGUI")
        self.logReport.logger.info("init GUI")
        self.frame = None
        self.imgTk = None # Video
        self.master = master

        self.master.attributes("-fullscreen", True)
        self.master.bind("<Escape>", lambda e: self.master.attributes("-fullscreen", False))
        
        self.widgetText("CONTROL DE LA CNC", 15, 10, 10) # Texto
        self.createFrame(10, 50, 750, 450) # Recuadro para el video
        self.createButton("Start", 10, 4, 15, 15, 550)
        self.createButton("Pause", 10, 4, 15, 215, 550)
        self.createButton("Stop", 10, 4, 15, 415, 550)
        self.createButton("Reset", 10, 4, 15, 615, 550)
        self.createButton("+Z", 6, 2, 15, 994, 70)
        self.createButton("-Z", 8, 2, 15, 980, 140)
        self.createButton("origin", 5, 2, 15, 1000, 220)
        self.createButton("-X", 8, 2, 15, 885, 300)
        self.createButton("+X", 6, 2, 15, 830, 370)
        self.createButton("-Y", 8, 2, 15, 1075, 300)
        self.createButton("+Y", 6, 2, 15, 1155, 370)
        self.createButton("-step", 8, 1, 15, 925, 460)
        self.createButton("+step", 8, 1, 15, 1035, 460)
        self.widgetText("INFO: ", 15, 800, 545)

        self.widgetText("WARN: ", 15, 800, 595)

        self.widgetText("ERROR: ", 15, 800, 645) 
        

        self.master.mainloop()
    
    def createFrame(self, xpos, ypos, width, height): # Video
        self.labelVideo_1 = tk.Label(
            self.master,
            borderwidth = 2,
            relief="solid" #Borde continuo
        )
        self.labelVideo_1.place(x=xpos, y=ypos)
        self.createImageZeros(width, height) #Imagen de ceros negra
        self.labelVideo_1.configure(image=self.imgTk) #Imágenes a color del mismo tamaño de imgTk
        self.labelVideo_1.image = self.imgTk
    
    def createImageZeros(self, w, h):
        self.frame = np.zeros([h, w, 3], dtype=np.uint8)  # imagen negra
        self.frame = cv2.cvtColor(self.frame, cv2.COLOR_BGR2RGB)
        imgArray = Image.fromarray(self.frame)
        self.imgTk = ImageTk.PhotoImage(image=imgArray)
    
    def widgetText(self, title, sz, xpos, ypos):
        self.fontLabelText = font.Font(
            family='Helvetica', size = sz, weight = 'bold' #Fuente para el texto
        )
        self.labelNameCamera = tk.Label(
            self.master, text = title, fg = '#000000'
        )
        self.labelNameCamera['font'] = self.fontLabelText
        self.labelNameCamera.place(x=xpos, y=ypos)

    def createButton(self, whichone, w, h, sz, posx, posy):
        # Botones:
        match whichone:
            case "Start":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="START",
                    bg = '#007A39',
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy) #Lugar del botón dependiendo del tamaño de la resolución dada width and height
            case "Pause":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="PAUSE",
                    bg = "#C97601",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "Stop":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="STOP",
                    bg = "#950707",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "Reset":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="RESET",
                    bg = "#1C12A0",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "+X":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="+ X",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "-X":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="- X",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "+Y":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="+ Y",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "-Y":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="- Y",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "+Z":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="+ Z",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "-Z":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="- Z",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "+step":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="+ STEP",
                    bg = "#762E01",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "-step":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="- STEP",
                    bg = "#762E01",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)
            case "origin":
                self.btnInitCamera = tk.Button(
                    self.master,
                    text="O",
                    bg = "#5F5F5F",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"), 
                    command = self.initCamera #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnInitCamera.place(x=posx, y=posy)

    def initCamera(self):
        self.camera_1 = camera.RunCamera() # Ruta del video, () se obtiene la cámara por defecto
        self.camera_1.start() #start de camera.py
        self.showVideo()
        print("Start Camera 1")
    
        
    def showVideo(self): #Actualiza cada frame para mostrar video continuo
        if(self.camera_1.frame is not None):
            imgresize = cv2.resize(self.camera_1.frame, (500, 500))
            imgTk = self.convertToFrameTk(1, imgresize) # Ventana convertida a Tkinter 
            self.labelVideo_1.configure(image=imgTk) #Imágenes a color del mismo tamaño de imgTk
            self.labelVideo_1.image = imgTk
        
        self.after(1, self.showVideo) #Para actualizarse cada 1ms
            
    
    def convertToFrameTk(self, img, frame): #1, BGR; 2, Binary and GrayScale
        if(img==1):    
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Tkinter solo recibe imágenes en RGB
            imgArray = Image.fromarray(frame) #Y Tkinter lo tiene como un array
        else:
            framebin = cv2.cvtColor(frame, cv2.COLOR_GRAY2RGB)  # Tkinter solo recibe imágenes en RGB
            imgArray = Image.fromarray(framebin) #Y Tkinter lo tiene como un array
        return ImageTk.PhotoImage(image = imgArray)
    
def main():
    root = tk.Tk()
    root.title("CNC")
    appRunCamera = Application(master=root)
