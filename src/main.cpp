//==================================================================
// File			: main.cpp
// Description	: Driver for tsp.h
//==================================================================
#include <iostream>
#include <climits>
#include "../header/tsp.h"
#include "../header/usage.h"
#include "../header/MyThread.h"	// thread wrapper class

#define CPS CLOCKS_PER_SEC
#define NUM_THREADS 1

int main(int argc, char **argv) {
    // Check that user entered filename on command line
    if (argc < 2) {
        usage();
        exit(-1);
    }

    // Read file names from input
    string f, o;
    f = o = argv[1];
    o.append(".tour");

    // Create new tsp object
    TSP tsp(f, o);
    int n = tsp.get_size();

    // Start timing
    clock_t t = clock();
    clock_t t2;

    // Read cities from file
    if (DEBUG) cout << "Reading cities" << endl;
    tsp.readCities();
    if (DEBUG)
        cout << "Time to read cities: "
        << ((float) (clock() - t)) / CLOCKS_PER_SEC << " s\n";

    cout << "number of cities: " << tsp.n << endl;

    // Fill N x N matrix with distances between nodes
    if (DEBUG) cout << "\nFilling matrix" << endl;
    t2 = clock();
    tsp.fillMatrix_threads();
    if (DEBUG)
        cout << "Time to fill matrix: "
        << ((float) (clock() - t2)) / CPS
        << " s\n";

    // Find a MST T in graph G
    if (DEBUG) cout << "\nFinding mst" << endl;
    t2 = clock();
    tsp.findMST();
    if (DEBUG)
        cout << "Time to find mst: "
        << ((float) (clock() - t2)) / CPS
        << " s\n";

    // Find a minimum weighted matching M for odd vertices in T
    if (DEBUG) cout << "\nFinding perfect matching" << endl;
    t2 = clock();
    tsp.perfect_matching();
    if (DEBUG)
        cout << "Time to find matching: "
        << ((float) (clock() - t2)) / CPS
        << " s\n\n";

    // Find the node that leads to the best path
    clock_t start, end;
    start = clock();

    // Create array of thread objects
    MyThread threads[NUM_THREADS];

    int best = INT_MAX;
    int bestIndex;
    int stop_here = NUM_THREADS;

    // Amount to increment starting node by each time
    int increment = 1; // by 1 if n < 1040

    if (n >= 600 && n < 1040) increment = 3;
    else if (n >= 1040 && n < 1800) increment = 8;
    else if (n >= 1800 && n < 3205) increment = 25; // ~ 220s @ 3200
    else if (n >= 3205 && n < 4005) increment = 50; // ~ 230s @ 4000
    else if (n >= 4005 && n < 5005) increment = 120;// ~ 200 @ 5000
    else if (n >= 5005 && n < 6500) increment = 250;// ~ 220s @ 6447
    else if (n >= 6500) increment = 500;

    int remaining = n;
    int node = 0; // Start at node zero
    int count = 0; // Count to get thread ids

    while (remaining >= increment) {
        // Keep track iteration when last node will be reached
        if (remaining < (NUM_THREADS * increment)) {
            // each iteration advances NUM_THREADS * increment nodes
            stop_here = remaining / increment;
        }

        for (long th = 0; th < stop_here; th++) {
            threads[th].start_node = node;
            threads[th].my_id = count;
            threads[th].mytsp = &tsp;
            threads[th].start();
            node += increment;
            count++;
        }

        // Wait for all the threads
        for (long th = 0; th < stop_here; th++) threads[th].join();

        remaining -= (stop_here * increment);
    }

    cout << "count: " << count << endl;
    // Loop through each index used and find shortest path
    for (long th = 0; th < count; th++) {
        if (tsp.path_vals[th][1] < best) {
            bestIndex = tsp.path_vals[th][0];
            best = tsp.path_vals[th][1];
        }
    }

    end = clock();
    cout << "\nbest: " << best << " @ index " << bestIndex << endl;
    cout << "time: " << ((float) (end - start)) / CPS << "s\n";

    // Store best path
    tsp.create_tour(bestIndex);
    tsp.make_shorter();
    tsp.make_shorter();
    tsp.make_shorter();
    tsp.make_shorter();
    tsp.make_shorter();

    cout << "\nFinal length: " << tsp.pathLength << endl;

    // Print to file
    tsp.printResult();

    if (DEBUG) cout << "\nTotal time: " << ((float) (clock() - t)) / CPS << "s\n";

    return 0;
}
