import sys

old_char_left_for_draw = None
old_char_top_for_draw = None

for line in sys.stdin:
	print(line[:-1])
# 	if line.startswith("movement: old_x"):
	if line.startswith("Draw orc at"):
		parts = line.split(" ")
		xy = parts[3].split(",")
		char_left = xy[0]
		char_top = xy[1]
		if old_char_left_for_draw != None:
			if char_left > old_char_left_for_draw:
				print("> Moved right!")
		old_char_left_for_draw = char_left
		old_char_top_for_draw = char_top
		