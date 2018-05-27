import sys

option = sys.argv[1]
filename = sys.argv[2]

dick    = dict()
chinese = list()


with open(filename, encoding='cp950') as f:
	for line in f:
		if len(line) == 0:
			continue

		char, zhu = line.split(' ')

		chinese.append(char)

		zhu   = zhu.split('/')
		zhu_h = [element[0] for element in zhu]

		for fuck in zhu_h:
			if fuck not in dick:
				dick[fuck] = list(char)
			else:
				if char not in dick[fuck]:
					dick[fuck].append(char)



file = open("ZhuYin-Big5.map", "w+", encoding='cp950')

for zhu, chars in dick.items():
	file.write('{}\t'.format(zhu))
	for char in chars:
		file.write('{} '.format(char))
	file.write('\n')
for char in chinese:
	file.write('{}\t{}\n'.format(char, char)) 