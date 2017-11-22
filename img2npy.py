from PIL import Image
import numpy as np

IMGDIR = './out_img_6view'
NPYDIR = './npy'

n_model = 1202
n_view = 6
views = np.zeros((n_model * n_view, 100, 100, 1), dtype='uint8')

for i in xrange(n_model):
    if i % 100 == 0: print i
    for j in xrange(n_view):
        imgpath = '%s/%d.%d.png' % (IMGDIR, i, j)
        img = Image.open(imgpath)
        img = img.resize((100, 100))
        img = img.convert('L')
        arr = np.array(img, dtype='uint8')
        views[i*n_view + j, ..., 0] = arr
mean = np.mean(views, axis=0)

views.dump('%s/views_6view.npy' % NPYDIR)
mean.dump('%s/mean_6view.npy' % NPYDIR)

