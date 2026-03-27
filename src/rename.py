import os

for root, _, files in os.walk("."):
    for f in files:
        if f.endswith(".h"):
            path = os.path.join(root, f)
            with open(path, "r") as file:
                lines = file.readlines()

            if len(lines) < 3:
                continue

            if lines[0].startswith("#ifndef") and lines[1].startswith("#define"):
                # убираем первые 2 строки и последний #endif
                lines = lines[2:]
                if lines[-1].strip().startswith("#endif"):
                    lines = lines[:-1]

                lines.insert(0, "#pragma once\n")

                with open(path, "w") as file:
                    file.writelines(lines)