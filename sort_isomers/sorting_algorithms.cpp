#include "functions.h"
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <limits.h>

int bubble_sort (std::vector<std::string>& isomers, \
       std::vector<double>& E_opt, std::vector<double>& G_opt, \
       std::vector<double>& freq, std::vector<double>& cpu_time) {

   int test, i; //k

   //------------------------------------------------
   // Simple bubble sort of Gibbs free energies:
   //------------------------------------------------
      
   //-----------------------------------------------------
   //test = 1: they are ordered in descending order
   //test = 0: they are not ordered in descending order
   //-----------------------------------------------------

   test = 0; // initialize the loop
   //k = 0; // variable to avoid looping the entire array when it's 
                                                 // not necessary

   while (test == 0) {
      test = 1; // maybe this time they will all be ordered
      // for (i=0; i<G_opt.size()-k-1; i=i+1) {
      for (i=0; i<G_opt.size()-1; i=i+1) {
         if ( G_opt[i] > G_opt[i+1] ) {
            swap_double(G_opt, i, i+1);
            swap_double(E_opt, i, i+1);
            swap_string(isomers, i, i+1);
            swap_double(freq, i, i+1);
            swap_double(cpu_time, i, i+1);
            test = 0; // they weren't ordered so, change test back to 0
//            k = k + 1;
         }
      }
   }


   return 0;
}


