from subprocess import call
#!/usr/bin/env python
from subprocess import Popen, PIPE, STDOUT
input = b'whereami\n' * 11000
input += b'byebye\n'
proc = Popen(['./a.out', 'f'], stdout=PIPE, stdin=PIPE, stderr=STDOUT)

out, err = proc.communicate(input=input)
print(out)
print(err)
