from tkinter import Tk, Canvas, Frame, BOTH, W, Label ,CENTER
from PIL import Image, ImageTk , ImageDraw, EpsImagePlugin, ImageGrab
import ghostscript
import leaderboard
import datetime

ranks_min = leaderboard.make_leaderboard_to_show()

if len(ranks_min) < 5:
    ranks_min.append(('To już wszyscy wyprowadzający!' , ''))
    ranks_min.append(('' , ''))

class Example(Frame):

    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):

        self.master.title("Obecny ranking: ")
        self.pack(fill=BOTH, expand=1)
        
        canvas = Canvas(self , bg= '#d4f0c5')   

        imag = Image.open('Desktop\\img2.png')

        self.photo = ImageTk.PhotoImage(imag)
        canvas.create_image(-180 , 200 , image = self.photo , anchor = W)

        canvas.create_text(200, 350, anchor=W, font="Arial", fill='blue',
            text=str(ranks_min[0][0]) + str(ranks_min[0][1]))
        canvas.create_text(200, 380, anchor=W, font="Arial", fill='blue',
            text=str(ranks_min[1][0]) + str(ranks_min[1][1]))
        canvas.create_text(200, 410, anchor=W, font="Arial", fill='blue',
            text=str(ranks_min[2][0]) + str(ranks_min[2][1]))
        canvas.create_text(200, 440, anchor=W, font="Arial", fill='blue',
            text=str(ranks_min[3][0]) + str(ranks_min[3][1]))
        canvas.create_text(200, 470, anchor=W, font="Arial", fill='blue',
            text=str(ranks_min[4][0]) + str(ranks_min[4][1]))
       
        canvas.pack(fill=BOTH, expand=1)

def main():
    now = datetime.datetime.now()
    now = now.strftime('%f')
    root = Tk()
    Example()
    root.geometry("900x750-50-50")
    root.mainloop()
    image = ImageGrab.grab(bbox = (722 , 32 , 1848 , 1006))
    image.save('Desktop\\ranking'+now+'.png' , 'png')

if __name__ == '__main__':
    main()
