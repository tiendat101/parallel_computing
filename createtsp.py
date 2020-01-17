N = 12
# a = 3

import random

# with open('tsp_input.txt', 'w') as f:
# 	for i in range(N * N):
# 		if (i + 1) % N == 0:
# 			f.write(str(random.randint(1, 20)) + ' \n')
# 		else:
# 			f.write(str(random.randint(1, 20)) + ' ')

for k in range(10):
	with open('tsp_input' + str(k) + '.txt', 'w') as f:
		for i in range(N):
			for j in range(N):
				f.write(str(random.randint(1, 20)) + ' ')
			f.write('\n')