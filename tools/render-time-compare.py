def avg_middle(lines):
    num_lines = len(lines)
    total = 0
    count = 0
    for i in range(round(num_lines/3), round(num_lines*2/3)):
        time = int(lines[i].split(" ")[2])
        total += time
        count += 1
    return total/count

with open("render-hardware.out", "r") as file:
    lines_h = file.readlines()
print(f"hardware render avg is {avg_middle(lines_h)}");

with open("render-software.out", "r") as file:
    lines_s = file.readlines()
print(f"software render avg is {avg_middle(lines_s)}");


