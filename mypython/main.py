from matplotlib import pyplot as plt
from matplotlib import animation
from PariticleSimulator import PariticleSimulator as ps
from Particle import Particle as Pa
from random import *
def visualize(simulator):
    X= [p.x for p in simulator.particles]
    Y=[p.y for p in simulator.particles]
    fig=plt.figure()
    ax=plt.subplot(111,aspect='equal')
    line, = ax.plot(X,Y,'ro')

    plt.xlim(-1,1)
    plt.ylim(-1,1)

    def init():
        line.set_data([],[])
        return line,

    def animate(i):
        simulator.evolve(0.01)
        X= [p.x for p in simulator.particles]
        Y=[p.y for p in simulator.particles]
        line.set_data(X,Y)
        return line,
    
    anim=animation.FuncAnimation(fig,animate,init_func=init,blit=True,interval=10)
    plt.show()

def test_visualize():
    # particles=[Pa(0.3,0.5,1),Pa(0.0,-0.5,-1),Pa(-0.1,-0.4,3)]
    particles=[Pa(uniform(-1,1.0),uniform(-1,1.0),uniform(-1,1.0)) for i in range(100)]
    simulator=ps(particles)
    visualize(simulator)
if __name__=='__main__':
    test_visualize()