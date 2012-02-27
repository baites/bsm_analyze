#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 22, 2012
Copyright 2011, All rights reserved
'''

from root.template import Template
from channel_type import ChannelType
from base_style import Style
from channel_style import ChannelStyle

class ChannelTemplate(ChannelType, ChannelStyle, Template):
    '''
    Channel with type and template. ChannelTemplate knows what input types
    can be added to it (the policy is defined by allowed_inputs property).

    A RuntimeError will be raised if an attempt is made to add wrong input
    template. At the same time, a ValueError is raised if user attempts to
    add empty input template (without histogram).

    Added input templates can be accessed as a list or a sum-plot through
    hist property. The latter one is dynamically calculated and cached upon
    use.
    '''

    def __init__(self, channel_type):
        '''
        Initialize empty Channel Template with specific type
        '''

        ChannelType.__init__(self, channel_type)
        ChannelStyle.__init__(self, channel_type)
        Template.__init__(self)

        self.__input_templates = []         # track added input templates
        self.__input_template_types = set() # track added input template types

    def add(self, input_template):
        '''
        Add input template to channel. Only template with allowed type can
        be added. Next exception are raised in case of errors:

            RuntimeError    if wrong template is added
            ValueError      if template plot is empty
        '''

        if input_template.type not in self.allowed_inputs:
            raise RuntimeError("can not add input template {0!r} "
                               "to channel {1!r}".format(
                                   input_template.type,
                                   self.type))

        # make sure input template was not added to the channel before
        if input_template.type not in self.__input_template_types:
            # issue error if input template does not have histogram attached
            if not input_template.hist:
                raise ValueError("attempt to add input template {0!r} "
                                 "without histogram".format(
                                     input_template.type))

            self.input_templates.append(input_template)
            self.__input_template_types.add(input_template.type)

            # Reset histogram only if was previously created
            if Template.hist.__get__(self, self.__class__):
                Template.hist.__set__(self, None)

    @property
    def input_templates(self):
        '''
        Access list of added input templates. Do not modify the list or
        templates as it will break the code. Use add(...)  method instead
        '''

        return self.__input_templates

    @property
    def hist(self):
        '''
        Dynamically calculate sum-histogram and cache it for later use.
        Plot will be re-calculated if any new input templates are added.

        None object is returned if no input templates were added
        '''

        hist = Template.hist.__get__(self, self.__class__)

        # create histogram of it does not exist and input templates are
        # available
        if not hist and self.input_templates:

            # Add up all input templates
            for template in self.input_templates:
                # Make sure input template still holds histogram
                if not template.hist:
                    raise RuntimeError("input template {0!r} was modified and "
                                       "does not hold histogram "
                                       "any more".format(template.type))

                # Create histogram if does not exist or add input template plot
                if hist:
                    hist.Add(template.hist)
                else:
                    hist = template.hist.Clone()
                    hist.SetDirectory(0)

            # Store newly created histogram in Template class
            Template.hist.__set__(self, hist)

            # Copy filename and path
            self._Template__filename = template.filename
            self._Template__path = template.path

            self.channel_style.apply(self.hist)

        return hist

    @hist.setter
    def hist(self, obj):
        '''
        User can only reset histogram cache, otherwise RuntimeError exception
        is raised.
        '''

        if not obj:
            Template.hist.__set__(self, obj)
        else:
            raise RuntimeError("channel template histogram can not be set")

    def __str__(self):
        '''
        Add counter of templates to print
        '''

        return ("<{Class} with {templates} input template(s) at 0x{ID:x}>\n"
                "  + {ChannelTypeStr}\n"
                "  + {TemplateStr}").format(
                    Class = self.__class__.__name__,
                    templates = len(self.input_templates),
                    ChannelTypeStr = ChannelType.__str__(self),
                    TemplateStr = Template.__str__(self),
                    ID = id(self))

    def __contains__(self, value):

        return (ChannelType.__contains__(self, value) and
                ChannelStyle.__contains__(self, value))

class MCChannelTemplate(ChannelTemplate):
    channel_types = {
            "mc": ["ttbar", "zjets", "wjets", "stop"]
            }

    channel_styles = {
            "mc": Style(fill_style = 3004)
            }

if "__main__" == __name__:
    import random
    import unittest
    import ROOT

    from input_template import InputTemplate
    from input_type import InputType

    # Prepare function for later random fill
    my_gaus = ROOT.TF1("my_gaus", "gaus(0)", 0, 100)
    my_gaus.SetParameters(1, 50, 10)

    # Create plot and randomly fill with above function
    plot = ROOT.TH1F("plot", "plot", 100, 0, 100);
    plot.FillRandom("my_gaus", 10000)

    class TestChannelTemplate(unittest.TestCase):
        def test_empty_template_type(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)
            self.assertEqual(template.type, channel_type)

        def test_empty_template_path(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)
            self.assertEqual(template.path, "")

        def test_empty_template_name(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)
            self.assertEqual(template.name, "")

        def test_empty_template_dim(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)
            self.assertEqual(template.dimension, None)

        def test_empty_template_hist(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)
            self.assertEqual(template.hist, None)

        def test_empty_template_allowed_inputs(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)
            self.assertEqual(template.allowed_inputs,
                             ChannelType.channel_types[channel_type])

        def test_empty_template_input_templates(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)
            self.assertEqual(len(template.input_templates), 0)

        def test_empty_template_set_hist(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create input template and add
            input_template = InputTemplate(random.choice(template.allowed_inputs),
                                           Template(plot, clone = True))

            template.add(input_template)
            template.hist

            try:
                template.hist = "123"
            except RuntimeError as error:
                def raise_error(obj):
                    raise obj

                self.assertRaises(RuntimeError, raise_error, error)

        def test_template_add_input(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create input template and add
            input_template = InputTemplate(random.choice(template.allowed_inputs),
                                           Template(plot, clone = True))

            template.add(input_template)

            self.assertEqual(len(template.input_templates), 1)

        def test_template_add_empty_input(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create empty input template and add
            input_template = InputTemplate(random.choice(template.allowed_inputs),
                                           Template())

            self.assertRaises(ValueError, template.add, input_template)
        
        def test_template_add_inputs(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create input template(s) and add keep track of unique types
            added_input_templates = set()
            for x in range(2 * len(template.allowed_inputs)):
                choice = random.choice(template.allowed_inputs)
                added_input_templates.add(choice)
                input_template = InputTemplate(choice,
                                               Template(plot, clone = True))
                template.add(input_template)

            self.assertEqual(len(template.input_templates),
                             len(added_input_templates))

        def test_template_add_inputs_check_types(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create input template(s) and add keep track of unique types
            added_input_templates = set()
            for x in range(2 * len(template.allowed_inputs)):
                choice = random.choice(template.allowed_inputs)
                added_input_templates.add(choice)
                input_template = InputTemplate(choice,
                                               Template(plot, clone = True))
                template.add(input_template)

            for input_template in template.input_templates:
                self.assertIn(input_template.type, template.allowed_inputs)

        def test_template_add_wrong_input(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create input template and add
            input_template = InputTemplate(
                    random.choice(
                        list(set(InputType.input_types.keys()) -
                        set(template.allowed_inputs))),
                    Template(plot, clone = True))

            self.assertRaises(RuntimeError, template.add, input_template)
        
        def test_template_calculate_hist(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create input template(s) and add keep track of unique types
            added_input_templates = set()
            for x in range(2 * len(template.allowed_inputs)):
                choice = random.choice(template.allowed_inputs)
                added_input_templates.add(choice)
                input_template = InputTemplate(choice,
                                               Template(plot, clone = True))
                template.add(input_template)

            self.assertNotEqual(template.hist, None)

        def test_template_recalculate_hist(self):
            channel_type = random.choice(ChannelType.channel_types.keys())
            template = ChannelTemplate(channel_type)

            # create input template(s) and add keep track of unique types
            added_input_templates = set()
            for x in range(2 * len(template.allowed_inputs)):
                choice = random.choice(template.allowed_inputs)
                added_input_templates.add(choice)
                input_template = InputTemplate(choice,
                                               Template(plot, clone = True))
                template.add(input_template)

            self.assertNotEqual(template.hist, None)

            template.hist = None

            self.assertNotEqual(template.hist, None)

    unittest.main()
