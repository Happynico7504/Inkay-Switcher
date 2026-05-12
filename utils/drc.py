import sys

# Formula:
# x = ((854 - width)  * 0.5) + xinput
# y = ((480 - height) * 0.5) + yinput

SCREEN_W = 854.0
SCREEN_H = 480.0

def calctouchpos(width, height, xinput=0.0, yinput=0.0):
    outputx = ((SCREEN_W - width) * 0.5) + xinput
    outputy = ((SCREEN_H - height) * 0.5) + yinput
    return outputx, outputy

if len(sys.argv) < 5:
    print("Usage:")
    print("python calc.py <width> <height> <xoffset> <yoffset>")
    sys.exit(1)

def main():
    w = float(sys.argv[1])
    h = float(sys.argv[2])
    xoffset = float(sys.argv[3])
    yoffset = float(sys.argv[4])

    x, y = calctouchpos(w, h, xoffset, yoffset)

    print(f"x = {x}")
    print(f"y = {y}")

if __name__ == "__main__":
    main()