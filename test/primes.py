#! /usr/bin/env python

import sys

class PrimeNumber(object):
    def __init__(self):
        self._number = 2

    def next(self):
        while True:
            self._number += 1

            for i in range(2, self._number):
                if (0 == self._number % i):
                    break
            else:
                break

    def number(self):
        return self._number


if "__main__" == __name__:
    numbers = int(sys.argv[1])

    prime = PrimeNumber()

    product  = 1
    for i in range(0, numbers):
        product *= prime.number()
        print "{0}".format(prime.number())

        prime.next()

    print "product: {0}".format(product)
