def avg_middle(lines):
    num_lines = len(lines)
    total = 0
    count = 0
    for i in range(round(num_lines/3), round(num_lines*2/3)):
        time = int(lines[i].split(" ")[2])
        total += time
        count += 1
    return total/count

with open("log-argb8888.out", "r") as file:
    lines1 = file.readlines()
    lines1 = [x for x in lines1 if "Render time:" in x]
print(f"lines1 avg is {avg_middle(lines1)}");

with open("log-without-locking.out", "r") as file:
    lines2 = file.readlines()
    lines2 = [x for x in lines2 if "Render time:" in x]
print(f"lines2 avg is {avg_middle(lines2)}");


