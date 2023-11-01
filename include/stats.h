#ifndef _STATS_H
#define _STATS_H

struct statistics_t {
    int num_ops;
    unsigned long total_time_microseconds; // Tipo de dado inteiro sem sinal, que é apropriado para armazenar valores inteiros não negativos.
    int num_clients_connected;
};

#endif