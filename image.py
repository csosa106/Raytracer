import numpy as np
import glob
from PIL import Image

filename = glob.glob("*txt")[0]
data = np.loadtxt(filename)

dims = np.shape(data)
height = int(dims[0] / 3)
width = dims[1]

rgb = np.zeros((height, width, 3), dtype = np.uint8)

for i in range(3):
    rgb[:, :, i] = (255 * data[height * i : height * (i+1), :]).astype(np.uint8)

img = Image.fromarray(rgb, 'RGB')
img.save(filename.replace(".txt", ".png"))
