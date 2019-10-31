#include "ga.hpp"


void init_population(Solution first_individual)
{
    Solution new_individual;
    ga_sol_population.push_back(first_individual);

    for (size_t i = 0; i < ga_length_population-1; i++)
    {
        new_individual.candb_solution = new Frame_CAN[length_frames];
        for (size_t j = 0; j < length_frames; j++)
        {
          new_individual.candb_solution[j] = first_individual.candb_solution[j];
          new_individual.candb_solution[j].delay_time = (double)(rand()%((int)new_individual.candb_solution[j].deadline_time));
        }

        ga_sol_population.push_back(new_individual);
    }
}

void fitness_calc(Solution& individual)
{
    individual.fitness = SCALAR_WCRT*(individual.wcrt) + SCALAR_TIME_BURST*(individual.time_mean_burst)
                        + SCALAR_FRAMES_BURST*(individual.frames_burst);
}

void run_simulation(Solution& individual)
{
    simulator = new Simulator_CAN(individual.candb_solution, length_frames);
    simulator->run_simulation(TIME_SIMULATION);

    individual.wcrt            = simulator->wcrt;
    // printf("wcrt: %lf \n", simulator->wcrt);
    individual.frames_burst    = simulator->frames_burst;
    // printf("Burst: %lf \n", simulator->frames_burst);
    individual.time_mean_burst = simulator->time_mean_burst;
    // printf("Time: %lf \n", simulator->time_mean_burst);
    individual.deadlines       = simulator->deadlines;
    // printf("deadlines: %d \n", simulator->deadlines);

    fitness_calc(individual);

    delete(simulator);
}

void sort_fitness()
{
    for (size_t i = 0; i < ga_sol_population.size(); i++)
    {
        double fitness_better = ga_sol_population[i].fitness;
        size_t pos_better    = i;
        for (size_t j = i+1; j < ga_sol_population.size(); j++)
        {
            if (ga_sol_population[j].fitness < fitness_better)
            {
                fitness_better = ga_sol_population[j].fitness;
                pos_better    = j;
            }
        }
        if (pos_better != i)
        {
            Solution aux = ga_sol_population[i];
            ga_sol_population[i] = ga_sol_population[pos_better];
            ga_sol_population[pos_better] = aux;
        }
    }
    for (size_t i = (ga_sol_population.size()/2); i < ga_sol_population.size(); i++)
    {
        delete(ga_sol_population[i].candb_solution);
    }
    ga_sol_population.resize((ga_sol_population.size()/2));
}

void mutation(Solution& individual)
{
    for (size_t i = 0; i < length_frames; i++)
    {
        double prob_mut = ((double)rand()/RAND_MAX);
        if (prob_mut <= ga_prob_mution)
        {
            individual.candb_solution[i].delay_time = (double) (rand()%((int)individual.candb_solution[i].deadline_time));
            if (individual.candb_solution[i].delay_time < 0)
                individual.candb_solution[i].delay_time = 0;
            if (individual.candb_solution[i].delay_time > individual.candb_solution[i].deadline_time)
                individual.candb_solution[i].delay_time = individual.candb_solution[i].deadline_time;
        }
    }
}

Solution* crossover(Solution parent1, Solution parent2)
{
    Solution* sons = new Solution[2];
    sons[0].candb_solution = new Frame_CAN[length_frames];
    sons[1].candb_solution = new Frame_CAN[length_frames];

    int wp1 = rand()%(length_frames);
    int wp2;
    do
    {
        wp2 = rand()%length_frames;
    } while(wp1 == wp2);

    if (wp1 > wp2)
    {
        int aux = wp1;
        wp1 = wp2;
        wp2 = aux;
    }

    for (size_t i = 0; i < length_frames; i++)
    {
        if ((i < wp1)||(i>wp2))
        {
            sons[0].candb_solution[i] = parent1.candb_solution[i];
            sons[1].candb_solution[i] = parent2.candb_solution[i];
        }
        else
        {
            sons[1].candb_solution[i] = parent1.candb_solution[i];
            sons[0].candb_solution[i] = parent2.candb_solution[i];
        }
    }
    mutation(sons[0]);
    mutation(sons[1]);
    return sons;
}

void make_new_population()
{
    size_t current_length_population = ga_sol_population.size();
    for (size_t i = 0; i < current_length_population-1; i+=2)
    {
        Solution* sons = crossover(ga_sol_population[i], ga_sol_population[i+1]);
        ga_sol_population.push_back(sons[0]);
        ga_sol_population.push_back(sons[1]);
    }
}

Solution genetic_algorithm()
{
    u_int16_t generation_current = 1;
    double    last_fitnes, current_fitnes;

    do{
          // std::cout << "Generation " << generation_current << '\n';

          for(Solution& individual: ga_sol_population)
              run_simulation(individual);

          sort_fitness();

          last_fitnes = ga_sol_population[0].fitness;
          if (last_fitnes == 0) break;

          make_new_population();

          for(Solution& individual: ga_sol_population)
              run_simulation(individual);

          sort_fitness();
          current_fitnes = ga_sol_population[0].fitness;
          make_new_population();

          if (ga_difference_pop > (current_fitnes/last_fitnes))
              generation_current = 1;
          else
              generation_current++;

          // printf("Last:%f\tNow:%f\tDifirence:%f\n", last_fitnes, current_fitnes, ga_sol_population[0].fitness/last_fitnes);

    }while(generation_current <= ga_number_generation);

    sort_fitness();
    return ga_sol_population[0];
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    FILE* candb_file = fopen(argv[1], "r");
    Solution first;
    first.candb_solution = get_CANDB(candb_file, length_frames);
    ga_length_population = 90;
    ga_number_generation = 50;
    ga_difference_pop    = 0.99;
    ga_prob_mution       = 0.25;
    run_simulation(first);

    if (1)
    {
          init_population(first);
          Solution best = genetic_algorithm();
          run_simulation(best);
          fitness_calc(best);
          printf("WCRT %f \t Frames %f \t Time Burst %f\n", best.wcrt, best.frames_burst, best.time_mean_burst);
          // // // std::cout << "WCRT = " << best.wcrt << '\n';
          FILE* out = fopen(argv[2], "w+");
          if (!out)
          {
              std::cout << "\n[ERRO:] Ao criar arquivo "<< argv[2] << "\n\n";
              exit(10);
          }
          fprintf(out, "Função Objetiva\t%lf\n", best.fitness);
          fprintf(out, "WCRT\t%lf\n",            best.wcrt);
          fprintf(out, "TIME BURST\t%lf\n",      best.time_mean_burst);
          fprintf(out, "SIZE BURST\t%lf\n\n",    best.frames_burst);
          fprintf(out, "ID_MSG\tCYCLE\tDEADLINE\tSTART_DLY\tPAYLOAD\n");
          for (size_t i = 0; i < length_frames; i++)
          {
              fprintf(out,"%u\t%lf\t%lf\t%lf\t%u\n", best.candb_solution[i].id,best.candb_solution[i].cycle_time, best.candb_solution[i].deadline_time, best.candb_solution[i].delay_time, best.candb_solution[i].payload_frame);
          }
          fclose(out);
    }
    else
    {
        Solution best = first;
        best.candb_solution = new Frame_CAN[length_frames];
        for (int i = 0; i < length_frames; i++)
        {
          best.candb_solution[i] = first.candb_solution[i];
          // printf("ID %u\tCicle: %f\tDelay: %f\tPay: %d\n", best.candb_solution[i].id,
          //       best.candb_solution[i].cycle_time, best.candb_solution[i].delay_time,
          //       best.candb_solution[i].payload_frame);
        }

        run_simulation(best);

        printf("WCRT %f \t Frames %f \t Time Burst %f\n", best.wcrt, best.frames_burst, best.time_mean_burst);
    }
    return 0;
}
