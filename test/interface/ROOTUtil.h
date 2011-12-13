// utility ROOT functions for plotting
//
// Created by Samvel Khalatyan, Nov 18, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_ROOT_UTIL
#define BSM_ROOT_UTIL

#include <iostream>
#include <string>

#include "interface/Input.h"

class TH1;

// Scale by x-section, number of events and filter efficiency. Luminosity is
// not taken into account
//
void scale(TH1 *h, const Input &input);

std::string folder(const Input &input);

// systematic argument whill shift color. If systematic is PLUS (+1) then color
// will be shifted by -7; if systematic is MINUS (-1) then color is shiftd
// by +2; otherwise no shift is observed
//
void style(TH1 *hist, const Input &input, const int &systematic = 0);

float luminosity();

float triggerSF();

#endif
