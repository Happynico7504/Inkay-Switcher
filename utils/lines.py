import os

def countlines(root_dir, extensions=(".cc", ".hh")):
    total_lines = 0
    file_count = 0
    for root, _, files in os.walk(root_dir):
        for name in files:
            if name.endswith(extensions):
                path = os.path.join(root, name)
                try:
                    with open(path, "r", encoding="utf-8", errors="ignore") as f:
                        lines = f.readlines()
                        total_lines += len(lines)
                        file_count += 1
                except Exception as e:
                    print(f"Could not read {path}: {e}")
    return total_lines, file_count

if __name__ == "__main__":
    lines, files = countlines(".")
    print(f"C++ Source Files: {files}")
    print(f"Lines of Code: {lines}")