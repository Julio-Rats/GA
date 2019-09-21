#ifndef _GA_
#define  _GA_

#include "../Simulador CANbus/simulador.hpp"

#include <iostream>
#include <cstdlib>
#include <cfloat>
#include <vector>
#include <fstream>

#define     LENGTH_CHAR           64
#define     TIME_SIMULATION       120000

#define     SCALAR_WCRT           1000
#define     SCALAR_TIME_BURST     100
#define     SCALAR_FRAMES_BURST   10


class Solution
{
  public:
      double        fitness;
      double        wcrt;
      double        frames_burst;
      double        time_mean_burst;
      double        deadlines;
      Frame_CAN*    candb_solution;
};

std::vector<Solution>   ga_sol_population;
unsigned int            ga_number_generation;
unsigned int            ga_length_population;
unsigned int            length_frames;
double                  ga_prob_mution;

Simulator_CAN*          simulator;

#endif
