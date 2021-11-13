import pygame
import pygame.locals
import base64
import sys


array = bytearray(72)

if (len(sys.argv) == 2):
    face = sys.argv[1].encode()
    array[:] = base64.b64decode(face)
else:
    face = None

pygame.display.init()
pygame.display.set_caption("rinaboard editor")

disp = pygame.display.set_mode((1280,720))
surf = pygame.Surface((1280, 720))
timer = pygame.time.Clock()

run = True
redraw = True

def drawbox(disp, x, y, c):
    pygame.draw.rect(disp, c, (x,y,38,38))

def update_disp(disp, array):
    for i in range(32*18):
        t = array[i // 8] & 0x80 >> (i%8)
        drawbox(disp, (i%32)*40+1, (i // 32) * 40+1, '#f482c8' if t else 0xffffff)
    pygame.draw.line(disp, 0, (640,0), (640,719),2)
    pygame.draw.line(disp, 0, (0,360), (1279,360),2)

def shift_up():
    for i in range(17):
        array[4*i:4*(i+1)] = array[4*(i+1):4*(i+2)]
    array[68:72] = b'\0\0\0\0'

def shift_down():
    for i in range(17, 0, -1):
        array[4*i:4*(i+1)] = array[4*(i-1):4*i]
    array[0:4] = b'\0\0\0\0'

def shift_right():
    for i in range(18):
        array[i * 4:(i+1)*4] = ((int.from_bytes(array[i * 4:(i+1)*4], 'big') >> 1)).to_bytes(4, 'big')

def shift_left():
    for i in range(18):
        array[i * 4:(i+1)*4] = ((int.from_bytes(array[i * 4:(i+1)*4], 'big') << 1) & 0xffffffff).to_bytes(4, 'big')

def blank():
    for i in range(72):
        array[i] = 0

def togglepixel(x, y):
    n = x + y*32
    array[n // 8] ^= 0x80 >> (n%8)

while run:
    for ev in pygame.event.get():
        if ev.type == pygame.locals.QUIT:
            run = False
        if ev.type == pygame.locals.MOUSEBUTTONDOWN:
            x, y = ev.pos
            if ev.button == 1:
                togglepixel(x//40, y//40)
            elif ev.button == 3:
                print(base64.b64encode(array))
            redraw = True
        if ev.type == pygame.locals.KEYDOWN:
            if ev.key == pygame.locals.K_ESCAPE:
                run = False
            elif ev.key == pygame.locals.K_UP:
                shift_up()
            elif ev.key == pygame.locals.K_DOWN:
                shift_down()
            elif ev.key == pygame.locals.K_LEFT:
                shift_left()
            elif ev.key == pygame.locals.K_RIGHT:
                shift_right()
            elif ev.key == pygame.locals.K_SPACE:
                blank()
            elif ev.key == pygame.locals.K_r:
                if face:
                    array[:] = base64.b64decode(face)
            redraw = True

    if redraw:
        surf.fill('#f4e9f0')
        update_disp(surf, array)
        redraw = False
    disp.blit(surf, (0,0))
    pygame.display.flip()
    timer.tick(60)

pygame.quit()
