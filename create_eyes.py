import numpy as np


N_EYES = 12
#ELEVATION_DEG = 30
ELEVATION_DEG = 60

def eye_xyz(rot, elev):
    """
    rot: rotation on the x-z plane, 0 : (x,z) = (0,1)
         clockwise viewed down from (0,1,0), degree
    elev: elevation from x-z plane, degree
    """
    
    rot = deg2rad(rot)
    elev = deg2rad(elev)
    z = np.cos(rot)
    x = np.sin(rot)
    xz = (z**2 + x**2) ** 0.5
    y = xz * np.tan(elev) 
    xyz = np.array([x,y,z]) / (x**2 + y**2 + z**2) ** 0.5
    return xyz

def deg2rad(degree):
    return np.pi / 180. * float(degree)
    
if __name__ == '__main__':
    # eyes_re = [
        # (0, 7.1),
        # (60, 7.1),
        # (120, 7.1),
        # (180, 7.1),
        # (240, 7.1),
        # (300, 7.1)
    # ]
    
    start_deg = 0
    eyes_re = []
    for i in xrange(N_EYES):
        deg = i * 360 / N_EYES
        eyes_re.append((deg, ELEVATION_DEG))

    for eye_re in eyes_re:
        eye = eye_xyz(eye_re[0], eye_re[1])
        print '%f, %f, %f' % (eye[0], eye[1], eye[2])

