import numpy as np
#from matplotlib import pyplot as plt

h_0 = 5.32 # height of pivot point

x_0 = 7.744 #x position of servo 1
y_0 = 7.744 #y posiiton of servo 2

l_1 = 2 # length of servo arm
l_2 = 3.398 # length of linkage

h_1_0 = 2.26 # height of servo axis
h_2_0 = 2.26
    
# compute maximum tilt
h_1 = h_1_0 + l_1 + l_2
dh = h_1 - h_0
p_max = np.arctan(dh/x_0)

h_1 = h_1_0 - l_1 + l_2
dh = h_1 - h_0
p_min = np.arctan(dh/x_0)


def tilt2servo(phi, rad=True):
    if not rad:
        phi = np.deg2rad(phi)

    if phi > p_max:
        phi = p_max
    elif phi < p_min:
        phi = p_min

    k = h_0 + x_0 * np.tan(phi) - h_1_0
    t = np.arcsin((l_1**2-l_2**2+k**2)/(2*k*l_1))

    if not rad:
        t = np.rad2deg(t)
    return t

if __name__ == "__main__":
    print tilt2servo(-15.0, rad=False)

    #print np.rad2deg(p_min) # max theoretical tilt
    #print np.rad2deg(p_max) # max theoretical tilt

    #p = np.linspace(-p_min,p_max) # desired tilt angle
    #t = tilt2servo(p) # desired servo angle
    #plt.plot(p,t)
    #plt.show()
