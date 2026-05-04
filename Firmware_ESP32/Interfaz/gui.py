from logger import Logger
import tkinter as tk
from PIL import Image, ImageTk #pip install pillow
import tkinter.font as font
from tkinter import ttk
import cv2 #pip install opencv-python
import camera
import numpy as np
import serial #Para comunicación con lenguaje C de la ESP32
import threading #Para logging con ESP32
ser = serial.Serial('COM9', 115200, timeout=1) #RECORDAR CAMBIAR COM DEPENDIENDO DE DÓNDE ESTÉ CONECTADO ESP32

class Application(ttk.Frame):
    def __init__(self, master=None): #Todo lo gráfico irá en el master
        super().__init__(master)
        # variables globales:
        self.logReport = Logger("logGUI")
        self.logReport.logger.info("init GUI")
        self.frame = None
        self.imgTk = None # Video
        self.step = 0 #Pasos para dar en los eje X, Y, Z
        self.master = master

        self.master.attributes("-fullscreen", True)
        self.master.bind("<Escape>", lambda e: self.master.attributes("-fullscreen", False))

        self.camera_1 = camera.RunCamera() # Ruta del video, () se obtiene la cámara por defecto
        self.camera_1.start() #start de camera.py
        
        self.widgetText("CONTROL DE LA CNC", 20, 10, 10) # Texto
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
        self.start_serial_listener() #Recepción de Logging de ESP32
        self.labelInfo = self.widgetText("INFO: ", 15, 800, 545)
        self.labelWarn = self.widgetText("WARN: ", 15, 800, 595)
        self.labelError = self.widgetText("ERROR: ", 15, 800, 645)
        
        self.labelStep = self.widgetText(f"Step: {self.step}", 12, 970, 430)
        self.widgetText("mm/min ", 10, 1040, 432)

        self.showVideo()
        print("Start Video")

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

    def start_serial_listener(self):
        #Crear un hilo que corre en paralelo a la GUI:
        hilo = threading.Thread(target=self.serial_reader, daemon=True)
        hilo.start()
    def serial_reader(self):
        while True:
            #Acá recibe los datos por UART de la ESP32 en gui_send_log, línea uart_write_bytes(UART_PORT, buffer, len);
            if ser.in_waiting > 0: 
                linea = ser.readline().decode('utf-8', errors='ignore').strip() #Lee todo hasta encontrar salto de línea '\n', ignorar errores por ruido de la señal
                if linea:
                    # .after para actualizar la GUI de forma segura desde otro hilo:
                    self.master.after(0, self.update_gui_log, linea)
    def update_gui_log(self, linea):
        if "[INFO]" in linea:
            texto = linea.replace("[INFO] ", "")
            self.labelInfo.config(text=f"INFO: {texto}", fg="green")
        elif "[WARN]" in linea:
            texto = linea.replace("[WARN] ", "")
            self.labelWarn.config(text=f"WARN: {texto}", fg="orange")
        elif "[ERROR]" in linea:
            texto = linea.replace("[ERROR] ", "")
            self.labelError.config(text=f"ERROR: {texto}", fg="red")
    
    def widgetText(self, title, sz, xpos, ypos):
        self.fontLabelText = font.Font(
            family='Helvetica', size = sz, weight = 'bold' #Fuente para el texto
        )
        label = tk.Label(self.master, text = title, fg = '#000000')
        label['font'] = self.fontLabelText
        label.place(x=xpos, y=ypos)
        return label #Para mostrar valores, como el paso (step)

    def createButton(self, whichone, w, h, sz, posx, posy):
        # Botones:
        match whichone:
            case "Start":
                self.btnStart = tk.Button(
                    self.master,
                    text="START",
                    bg = '#007A39',
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Startpressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnStart.place(x=posx, y=posy) #Lugar del botón dependiendo del tamaño de la resolución dada width and height
            case "Pause":
                self.btnPause = tk.Button(
                    self.master,
                    text="PAUSE",
                    bg = "#C97601",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Pausepressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnPause.place(x=posx, y=posy)
            case "Stop":
                self.btnStop = tk.Button(
                    self.master,
                    text="STOP",
                    bg = "#950707",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Stoppressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnStop.place(x=posx, y=posy)
            case "Reset":
                self.btnReset = tk.Button(
                    self.master,
                    text="RESET",
                    bg = "#1C12A0",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Resetpressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnReset.place(x=posx, y=posy)
            case "+X":
                self.btnXp = tk.Button(
                    self.master,
                    text="+ X",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Xppressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnXp.place(x=posx, y=posy)
            case "-X":
                self.btnXm = tk.Button(
                    self.master,
                    text="- X",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Xmpressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnXm.place(x=posx, y=posy)
            case "+Y":
                self.btnYp = tk.Button(
                    self.master,
                    text="+ Y",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Yppressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnYp.place(x=posx, y=posy)
            case "-Y":
                self.btnYm = tk.Button(
                    self.master,
                    text="- Y",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Ympressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnYm.place(x=posx, y=posy)
            case "+Z":
                self.btnZp = tk.Button(
                    self.master,
                    text="+ Z",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Zppressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnZp.place(x=posx, y=posy)
            case "-Z":
                self.btnZm = tk.Button(
                    self.master,
                    text="- Z",
                    bg = "#000000",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Zmpressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnZm.place(x=posx, y=posy)
            case "+step":
                self.btnStepp = tk.Button(
                    self.master,
                    text="+ STEP",
                    bg = "#762E01",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Stepppressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnStepp.place(x=posx, y=posy)
            case "-step":
                self.btnStepm = tk.Button(
                    self.master,
                    text="- STEP",
                    bg = "#762E01",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"),
                    command = self.Stepmpressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnStepm.place(x=posx, y=posy)
            case "origin":
                self.btnO = tk.Button(
                    self.master,
                    text="O",
                    bg = "#5F5F5F",
                    fg = '#ffffff',
                    width = w,
                    height = h,
                    font=("Helvetica", sz, "bold"), 
                    command = self.Opressed #FUNCIÓN QUE CONTIENE LO QUE HARÁ ESTE BOTÓN
                )
                self.btnO.place(x=posx, y=posy)

    def Startpressed(self):
        ser.write(b"START\n")
        print("Orden de inicio enviada a ESP32")
        self.btnStart.config(state=tk.DISABLED)
        self.btnStop.config(state=tk.NORMAL)
        self.btnPause.config(state=tk.NORMAL)
        self.btnReset.config(state=tk.NORMAL)
    def Pausepressed(self):
        ser.write(b"PAUSE\n")
        print("Orden de pausa enviada a ESP32")
        self.btnStart.config(state=tk.NORMAL)
        self.btnStop.config(state=tk.NORMAL)
        self.btnPause.config(state=tk.DISABLED)
        self.btnReset.config(state=tk.NORMAL)
    def Stoppressed(self):
        ser.write(b"STOP\n")
        print("Orden de paro enviada a ESP32")
        self.btnStart.config(state=tk.NORMAL)
        self.btnStop.config(state=tk.DISABLED)
        self.btnPause.config(state=tk.NORMAL)
        self.btnReset.config(state=tk.NORMAL)
    def Resetpressed(self):
        ser.write(b"RESET\n")
        print("Orden de reset enviada a ESP32")
        self.btnStart.config(state=tk.NORMAL)
        self.btnStop.config(state=tk.NORMAL)
        self.btnPause.config(state=tk.NORMAL)
        self.btnReset.config(state=tk.DISABLED)
    def Zppressed(self):
        ser.write(b"Zp\n")
        print("Orden de + Z enviada a ESP32")
    def Zmpressed(self):
        ser.write(b"Zm\n")
        print("Orden de - Z enviada a ESP32")
    def Xppressed(self):
        ser.write(b"Xp\n")
        print("Orden de + X enviada a ESP32")
    def Xmpressed(self):
        ser.write(b"Xm\n")
        print("Orden de - X enviada a ESP32")
    def Yppressed(self):
        ser.write(b"Yp\n")
        print("Orden de + Y enviada a ESP32")
    def Ympressed(self):
        ser.write(b"Ym\n")
        print("Orden de - Y enviada a ESP32")
    def Stepppressed(self):
        self.step += 1 #mm
        self.labelStep.config(text=f"Step: {self.step}") #Actualizar el valor mostrado en GUI        
        mensaje = f"STEP:{self.step}\n"
        ser.write(mensaje.encode('utf-8')) #Lo codificamos a bytes y lo enviamos
        print(f"Step enviado a ESP32: {mensaje.strip()}")
    def Stepmpressed(self):
        if self.step > 0:
            self.step -= 1 #mm
            self.labelStep.config(text=f"Step: {self.step}") #Actualizar el valor mostrado en GUI
            mensaje = f"STEP:{self.step}\n"
            ser.write(mensaje.encode('utf-8')) #Lo codificamos a bytes y lo enviamos
            print(f"Step enviado a ESP32: {mensaje.strip()}")
    def Opressed(self):
        ser.write(b"ORIGIN\n")
        print("Orden de origen enviada a ESP32")
    
        
    def showVideo(self): #Actualiza cada frame para mostrar video continuo
        if(self.camera_1.frame is not None):
            imgresize = cv2.resize(self.camera_1.frame, (750, 450))
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
