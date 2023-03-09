from matplotlib import pyplot
import numpy

rho = numpy.sqrt(numpy.random.uniform(0, 1, 5000))
phi = numpy.random.uniform(0, 2*numpy.pi, 5000)

x = rho * numpy.cos(phi)
y = rho * numpy.sin(phi)

pyplot.scatter(x, y, s = 4)
pyplot.show()

