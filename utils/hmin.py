import re
import sys

def hmin(content):
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    lines = content.splitlines()
    minlin = []
    for line in lines:
        clean_line = ""
        in_string = False
        i = 0
        while i < len(line):
            char = line[i]
            if char == '"' and (i == 0 or line[i-1] != '\\'):
                in_string = not in_string
            if not in_string and line[i:i+2] == "//":
                break
            clean_line += char
            i += 1
        processed = clean_line.rstrip()
        if processed.strip():
            minlin.append(processed)
    result = "\n".join(minlin)
    result = re.sub(r'\n\s*\{', ' {', result)
    result = re.sub(r'\n+', '\n', result)
    return result

def main():
    if len(sys.argv) != 3:
        print("<input> <output>")
        sys.exit(1)
    inpt = sys.argv[1]
    outpt = sys.argv[2]
    try:
        with open(inpt, "r", encoding="utf-8", errors="ignore") as f:
            data = f.read()
        mindat = hmin(data)
        with open(outpt, "w", encoding="utf-8") as f:
            f.write(mindat)
    except Exception as e:
        print(f"{e}")

if __name__ == "__main__":
    main()