class Particle:
    __slot__=('x','y','ang_vel')
    def __init__(self,x,y,ang_vel):
        self.x=x
        self.y=y
        self.ang_vel=ang_vel