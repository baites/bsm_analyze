#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 20, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import shutil
import sys

import ROOT

def replace(templates = set(),
            in_file = None,
            with_file = None,
            to_file = None):
    '''
    Replace specified templates in the input file and store result in file

    templates   collection of template names to be replaced
    in_file     source file where all templates will be taken from
    with_file   source of replacement templates
    to_file    destination file
    '''
    if not os.path.lexists(in_file):
        raise Exception("templates input file does not exist: " + in_file)

    if os.path.lexists(to_file):
        raise Exception("destination file exists: " + to_file)

    # Shortcut
    #
    if not templates:
        print("no templates are specified for replacement: copy {0} to {1}".format(in_file, to_file))
        shutil.copy2(in_file, to_file)

        return

    if not os.path.lexists(with_file):
        raise Exception("replacement templates file does not exist: " + in_file)

    replacement = {
            "in": ROOT.TFile(in_file),
            "with": ROOT.TFile(with_file),
            "to": ROOT.TFile(to_file, "recreate")
            }

    for key, file in replacement.items():
        if file.IsZombie():
            raise Exception("failed to open {0} file: {1}".format(key.upper(), file.GetName()))

    # Get set of templates in files
    # 
    names = {}
    for key, file in replacement.items():
        if "to" == key:
            continue

        objects = set([x.GetName() for x in file.GetListOfKeys()])

        objects = objects - templates if "in" == key else objects & templates
        names[key] = objects

    # Store templates that are not found in the with_file
    # 
    names["missing"] = templates - (templates & names["with"])

    # Copy templates from in_file to output
    #
    output_file = replacement["to"]
    for key in "in", "with":
        input_file = replacement[key]
        for name in names[key]:
            h = input_file.Get(name)
            if not h:
                print("failed to read template: " + name, file = sys.stderr)

                continue

            h = h.Clone()
            output_file.WriteObject(h, name)

    if names["missing"]:
        print("Missing templates: they were not found in file " + with_file, file = sys.stderr)
        print(names["missing"], file = sys.stderr)
        print()

if "__main__" == __name__:
    print("module is not designed for execution", file = sys.stderr)

    sys.exit(1)
