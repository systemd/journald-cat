import journald
import fileinput
import json

for line in fileinput.input():
   d = json.loads(line)
   args = []
   for key, value in d.items():
       args.append("{0}={1}".format(key, value))
   journald.send(*args)
