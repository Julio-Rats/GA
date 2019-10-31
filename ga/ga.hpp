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
#define     SCALAR_TIME_BURST     0
#define     SCALAR_FRAMES_BURST   0


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
u_int16_t               ga_number_generation;
u_int16_t               ga_length_population;
u_int16_t               length_frames;
double                  ga_difference_pop;
double                  ga_prob_mution;

Simulator_CAN*          simulator;

#endif
