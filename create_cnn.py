N = 12
# a = 3

import random

for k in range(5, 10):
	line = random.randint(1, N)
	ck = True
	with open('cnn_input' + str(k) + '.txt', 'w') as f:
		for i in range(N):
			if i == line:
				ck = False
			if ck:
				for j in range(N):
					f.write(str(random.randint(50, 55)) + ' ')
			else:
				for j in range(N):
					f.write(str(random.randint(1, 5)) + ' ')

			f.write('\n')