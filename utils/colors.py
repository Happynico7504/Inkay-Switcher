import sys

def clamp255(x):
    return max(0, min(255, int(x)))

def hex2f32(hex_str):
    hex_str = hex_str.strip().replace("0x", "").replace("#", "")
    v = int(hex_str, 16)

    r = ((v >> 24) & 0xFF) / 255.0
    g = ((v >> 16) & 0xFF) / 255.0
    b = ((v >> 8)  & 0xFF) / 255.0
    a = ((v >> 0)  & 0xFF) / 255.0

    return f"F32Color{{ {r:.3f}f, {g:.3f}f, {b:.3f}f, {a:.3f}f }}"

def f322color(parts):
    r = clamp255(float(parts[0]) * 255)
    g = clamp255(float(parts[1]) * 255)
    b = clamp255(float(parts[2]) * 255)
    a = float(parts[3])

    return f"Color{{ {r}, {g}, {b}, {a}f }}"

def color2f32(parts):
    r = clamp255(float(parts[0]))
    g = clamp255(float(parts[1]))
    b = clamp255(float(parts[2]))
    a = float(parts[3])

    return f"F32Color{{ {r/255:.3f}f, {g/255:.3f}f, {b/255:.3f}f, {a:.3f}f }}"

def parsevec(tokens):
    nums = []
    for t in tokens:
        t = t.replace("{", "").replace("}", "").replace(",", "")
        if t:
            nums.append(t)
    return nums

def main():
    if len(sys.argv) < 3:
        print("Usage:")
        print("  hex 0xAARRGGBB")
        print("  f32 { r g b a }")
        print("  color { r g b a }")
        sys.exit(1)

    mode = sys.argv[1].lower()
    args = parsevec(sys.argv[2:])

    if mode == "hex":
        print(hex2f32(args[0]))
    elif mode == "f32":
        print(f322color(args))
    elif mode == "color":
        print(color2f32(args))
    else:
        print("Unknown mode")

if __name__ == "__main__":
    main()