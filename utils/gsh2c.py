import sys

def gsh2c(filename, var_name):
    with open(filename, "rb") as f:
        data = f.read()
    print(f"static const u8 {var_name}[] = {{")
    for i in range(0, len(data), 12):
        chunk = data[i:i+12]
        hex_line = ", ".join([f"0x{b:02x}" for b in chunk])
        comma = "," if i + 12 < len(data) else ""
        print(f"    {hex_line}{comma}")
    print("};")
    print(f"static const u32 {var_name}_len = {len(data)};")

gsh2c(sys.argv[1], sys.argv[2])