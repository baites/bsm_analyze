#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 29, 2012
Copyright 2011, All rights reserved
'''

def split_use_and_ban(values):
    ban = set()
    use = set()

    for value in values:
        if value.startswith('-'):
            ban.add(value[1:])
        else:
            use.add(value)

    return use, ban
