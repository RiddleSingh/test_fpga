A1_FILE='a1'
A4_FILE='a4'
NUM_IDSHOCKS=(1100 * 10000)

a1vals = [None] * NUM_IDSHOCKS
a4vals = [None] * NUM_IDSHOCKS

idx = 0
with open(A1_FILE) as fp:
	for line in fp:
		a1vals[idx] = line
		idx = idx+1
idx=0
with open(A4_FILE) as fp:
	for line in fp:
		a4vals[idx] = line
		idx = idx+1

for x in range(NUM_IDSHOCKS):
	if a4vals[x] != a1vals[x]:
		print(str(x) + "(a1: " + str(a1vals[x]) + ", a4: " + str(a4vals[x]) + ")")
