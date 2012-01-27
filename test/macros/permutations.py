#! /usr/bin/env python

import sys

def factorial(n):
    if (1 >= n):
        return 1
    else:
        return n * factorial(n - 1)

def subcombo(k):
    n_fact = factorial(n)

    sum = n_fact * (k - 1) / factorial(n - k)

    uniq = 0
    for l in range(1, k):
        uniq += n_fact / (factorial(n - k) * factorial(l) * factorial(k - l))

    return sum, uniq

def combo(n):
    sum = 0
    uniq = 0
    n_fact = factorial(n)
    for k in range(2, n + 1):
        sum += n_fact * (k - 1) / factorial(n - k)

        for l in range(1, k):
            uniq += n_fact / (factorial(n - k) * factorial(l) * factorial(k - l))

        remainder = n_fact * (k - 1) % factorial(n - k)
        if (0 != remainder):
            print "remainder: %d" % (remainder)

    return sum, uniq

if "__main__" == __name__:
    n = int(sys.argv[1])

    print "factorial({0}) = {1}".format(n, factorial(n))
    total, uniq = combo(n)
    print "combo({0})".format(n)
    print "total: {0}   uniq: {1}".format(total, uniq)
    difference = total - uniq
    print "total - uniq: {0}".format(difference)
    print "difference / uniq: {0}".format(difference / uniq)

    print
    sum, uniq = subcombo(n)
    print "subcombo"
    print "sum: {0}     uniq: {1}".format(sum, uniq)

    print
    print "3**{0} = {1}".format(n, 3**n)
