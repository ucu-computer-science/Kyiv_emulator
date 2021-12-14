import math

y = 1
a = 0.04  # float(input())
delta = 100000
while abs(delta) >= 2**(-36):
    delta = 0.5*(a/y - y)
    y += delta
    print("y= ", y, "real value= ", math.sqrt(a), "error= ",
          abs(math.sqrt(a) - y), "-", "delta ", delta, a/y)

print("y= ", y, "real value= ", math.sqrt(a), "error= ", abs(math.sqrt(a) - y))
