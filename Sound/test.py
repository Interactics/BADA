import numpy
import json

a=numpy.array([1,2,3,4])
b=numpy.array([3,4,5,6])

c=numpy.dstack((a,b))

print(c)
print(json.dumps(c.tolist()))
