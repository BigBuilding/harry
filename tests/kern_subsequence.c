/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 */

#include "config.h"
#include "common.h"
#include "hconfig.h"
#include "util.h"
#include "measures.h"
#include "tests.h"

/* Global variables */
int verbose = 0;
config_t cfg;

#define LAM 0.5

/*
 * Structure for testing string kernels/distances
 */
struct hstring_test
{
    char *x;		/**< String x */
    char *y;		/**< String y */
    float l;        /**< Weight for gaps */
    int n;          /**< Length of subsequences */
    float v;		/**< Expected output */
};


struct hstring_test tests[] = {
    /* Corner cases */
    {"", "", LAM, 2, 1},
    {"lala", "", LAM, 2, 0},
    {"", "lala", LAM, 2, 0},

    /* Length 2 */
    {"ab", "ab", LAM, 2, pow(LAM,4)},
    {"ab", "axb", LAM, 2, pow(LAM,5)},
    {"ab", "abb", LAM, 2, pow(LAM,5) + pow(LAM,4)},
    
    {NULL}
};

/**
 * Test runs
 * @param error flag
 */
int test_compare()
{
    int i, err = FALSE;
    hstring_t x, y;

    for (i = 0; tests[i].x && !err; i++) {
        config_set_float(&cfg, "measures.kern_subsequence.lambda", tests[i].l);
        config_set_int(&cfg, "measures.kern_subsequence.length", tests[i].n);
        measure_config("kern_subsequence");
    
        x = hstring_init(x, tests[i].x);
        y = hstring_init(y, tests[i].y);

        x = hstring_preproc(x);
        y = hstring_preproc(y);

        float d = measure_compare(x, y);
        double diff = fabs(tests[i].v - d);

        if (diff > 1e-6) {
            printf("Error %f != %f\n", d, tests[i].v);
            hstring_print(x);
            hstring_print(y);
            err = TRUE;
        }

        hstring_destroy(x);
        hstring_destroy(y);
    }

    return err;
}

/**
 * Main test function
 */
int main(int argc, char **argv)
{
    int err = FALSE;

    config_init(&cfg);
    config_check(&cfg);

    err |= test_compare();

    config_destroy(&cfg);
    return err;
}
