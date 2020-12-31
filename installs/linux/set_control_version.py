#!/bin/python

import re, sys

if len(sys.argv) < 3 :
	print 'Usage:', sys.argv[0], '<file> <version>'
	sys.exit(-1)

path = sys.argv[1]
version = sys.argv[2]

print ('Reading control file...')
file = open(path, 'r')
content = file.readlines();
file.close()

print ('Preparing file for write...')
file = open(path, 'w+')

print ('Substituting version...')
reg = re.compile('Version: (.+)$')
for line in content :
	m = reg.match(line)
	if m:
		print 'Found', m.group(), ', substituting...'
		file.write('Version: ')
		file.write(version)
		file.write('\n')
	else:
		file.write(line)
file.close()
