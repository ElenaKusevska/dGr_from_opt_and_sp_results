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
#include <thread> 
#include <chrono>


// Get all the data we need out of the gaussian output file
// when there is only one isomer
int gauss_output_one_isomer(std::string chemical_species, double& E_opt, \
      double& G_opt, double& H_opt, double& freq1, double& freq2, \
      double& cpu_time, double& E_sp, double& G_sp, double& H_sp, \
      double& S_opt, double& S_sp, double T, int& imaginary_freq, \
      std::string lower_level_of_theory, \
      std::string higher_level_of_theory, \
      std::string goodvibes_command, std::string out_files_path) {

   double days, hours, minutes, seconds;
   std::string cp_out_file_command; // cp *.out path/name.out
   std::string two_up = "../../"; // go two directories up

   //----------------------------------------------
   // Lower level of theory:
   //----------------------------------------------
   
   change_directory(lower_level_of_theory) ;
   change_directory(chemical_species);

   // Is there only one .out file in this directory?
   one_ext_file("out"); // will exit if there was not 1
   // If there is only one, copy it to out_files_path:
   cp_out_file_command = "cp *.out " + out_files_path + "/"
                         + chemical_species + ".out";
   system(cp_out_file_command.c_str());

   // Get the SCF energy:
   get_gout_value(goodvibes_command.c_str(), E_opt, 2);

   std::this_thread::sleep_for (std::chrono::seconds(1));

   // Get the enthalpy:
   get_gout_value(goodvibes_command.c_str(), H_opt, 4) ;
   
   std::this_thread::sleep_for (std::chrono::seconds(1));

   // Get the entropy:
   get_gout_value(goodvibes_command.c_str(), S_opt, 6) ;

   std::this_thread::sleep_for (std::chrono::seconds(1));

   // Get the Gibbs free energy:
   get_gout_value(goodvibes_command.c_str(), G_opt, 8) ;

   // Get the lowest frequency:
   get_gout_value("grep 'Frequencies' *out | head -1", 
         freq1, 2) ;
   get_gout_value("grep 'Frequencies' *out | head -1",
         freq2, 3) ;
   if (freq1 < 0) {imaginary_freq = 1;}
   else {imaginary_freq = 0;}

   // Get CPU time:
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         days, 3);
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         hours, 5);
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         minutes, 7);
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         seconds, 9);

   cpu_time = days*24.0*60.0 + hours*60.0 + minutes + seconds/60.0;  

   change_directory(two_up);

   //-------------------------------------
   // higher level of theory:
   //-------------------------------------

   change_directory(higher_level_of_theory);
   change_directory(chemical_species);

   // Is there only one .out file in this directory?
   one_ext_file("out"); // will exit if there was not 1
   // If there is only one, copy it to out_files_path:
   cp_out_file_command = "cp *.out " + out_files_path + "/"
      + chemical_species + "_sp.out";
   system(cp_out_file_command.c_str());

   get_gout_value("grep 'SCF Done' *.out | tail -1",
      E_sp, 4);

   G_sp = E_sp + (G_opt - E_opt);
   H_sp = E_sp + (H_opt - E_opt);
   S_sp = (H_sp - G_sp) / T;
   
   change_directory(two_up);

   return 0;
}   

int sort_multiple_geometries (std::vector<std::string> isomers, \
      std::string chemical_species, std::string lower_level_of_theory, \
      std::string higher_level_of_theory, std::string& lowest_E_isomer, \
      std::string goodvibes_command) {

   // isomers vector contains labels of isomers (ex: 3_1, 3_a, etc.)
   std::string three_up = "../../../"; // go three directories up
   std::string outfilename; // for printing sorted energies
   std::vector<double> E_opt_sort, G_opt_sort, freq_sort;
   std::vector<double> E_sp_sort, G_sp_sort;
   int k, test; // for sorting
   int i;

   E_opt_sort.resize(isomers.size());
   G_opt_sort.resize(isomers.size());
   freq_sort.resize(isomers.size());
   E_sp_sort.resize(isomers.size());
   G_sp_sort.resize(isomers.size());

   for (i=0; i<isomers.size(); i=i+1) {

      //----------------------------------------------
      // Get lower level of theory energies:
      //----------------------------------------------
      
      change_directory(lower_level_of_theory);
      change_directory(chemical_species);
      change_directory(isomers[i]);

      // Get the SCF energy:
      get_gout_value(goodvibes_command.c_str(), E_opt_sort[i], 2);

      std::this_thread::sleep_for (std::chrono::seconds(1));

      // Get the Gibbs free energy
      get_gout_value(goodvibes_command.c_str(), G_opt_sort[i], 8) ;

      // Get the frequency:
      get_gout_value("grep 'Frequencies' *out | head -1", freq_sort[i], 2) ;
      
      change_directory(three_up);

      //----------------------------------------------
      // Get higher level of theory energies:
      //----------------------------------------------

      change_directory(higher_level_of_theory);
      change_directory(chemical_species);
      change_directory(isomers[i]);

      get_gout_value("grep 'SCF Done' *.out | tail -1",
         E_sp_sort[i], 4);

      G_sp_sort[i] = E_sp_sort[i] + (G_opt_sort[i] - E_opt_sort[i]);
   
      change_directory(three_up);

   }
   
   //------------------------------------------------
   // Simple bubble sort of Gibbs free energies:
   //------------------------------------------------
      
   //-----------------------------------------------------
   //test = 1: they are ordered in descending order
   //test = 0: they are not ordered in descending order
   //-----------------------------------------------------

   test = 0; // initialize the loop
   k = 0; // variable to avoid looping the entire array when it's 
                                                 // not necessary

   while (test == 0) {
      test = 1; // maybe this time they will all be ordered this time
      for (i=0; i<G_sp_sort.size()-1; i=i+1) {
         if ( G_sp_sort[i] > G_sp_sort[i+1] ) {
            swap_double(G_sp_sort, i, i+1);
            swap_double(E_sp_sort, i, i+1);
            swap_double(G_opt_sort, i, i+1);
            swap_double(E_opt_sort, i, i+1);
            swap_double(freq_sort, i, i+1);
            swap_string(isomers, i, i+1);
            test = 0; // they weren't ordered so, change test back to 0
            k = k + 1;
         }
      }
   }

   // The label for the lowest energy isomer is in isomers[0]
   lowest_E_isomer = isomers[0];

   //----------------------------------------
   // Print sorted energies to file:
   //----------------------------------------
   
   // file << std::setprecision() - number of digits after decimal point
   // file.precision() - number od significant figures

   outfilename = chemical_species + "_sorted_energies";

   if_file_exist_delete(outfilename);
   
   std::ofstream sorted_energies;
   sorted_energies.open (outfilename.c_str());
   sorted_energies << "isomer " << "E_" << lower_level_of_theory
                   << " G_" << lower_level_of_theory
                   << " E_" << higher_level_of_theory
                   << " G_" << higher_level_of_theory 
                   << " smallest_frequency" << std::endl;
   sorted_energies.precision(11) ;
   for (i=0; i<isomers.size(); i=i+1) {
      sorted_energies << isomers[i] << " " << E_opt_sort[i] << " " 
                      << G_opt_sort[i] << " " << E_sp_sort[i] << " " 
                      << G_sp_sort[i] << " " << freq_sort[i] << std::endl;
   }
   sorted_energies.close();

   return 0;
}

int gauss_output_multiple_isomers(std::string chemical_species, \
   std::string current_isomer, double& E_opt, double& G_opt, \
   double& freq1, double& freq2, double & H_opt, double & H_sp, \
   double& S_sp, double& S_opt, double T, double& cpu_time, double& E_sp, \
   double& G_sp, int& imaginary_freq, std::string lower_level_of_theory, \
   std::string higher_level_of_theory, std::string goodvibes_command, \
   std::string out_files_path) {

   double days, hours, minutes, seconds;
   std::string cp_out_file_command; // cp *.out path/name.out
   std::string three_up = "../../../"; // go three directories up

   //----------------------------------------------
   // Lower level of theory:
   //----------------------------------------------
   
   change_directory(lower_level_of_theory) ;
   change_directory(chemical_species);
   change_directory(current_isomer);

   // Is there only one .out file in this directory?
   one_ext_file("out"); // will exit if there was not 1
   // If there is only one, copy it to out_files_path:
   cp_out_file_command = "cp *.out " + out_files_path + "/"
                          + current_isomer + ".out";
   system(cp_out_file_command.c_str());

   // Get the SCF energy:
   get_gout_value(goodvibes_command.c_str(), E_opt, 2);

   std::this_thread::sleep_for (std::chrono::seconds(1));

   // Get the enthalpy:
   get_gout_value(goodvibes_command.c_str(), H_opt, 4) ;

   std::this_thread::sleep_for (std::chrono::seconds(1));

   // Get the entropy:
   get_gout_value(goodvibes_command.c_str(), S_opt, 6);

   std::this_thread::sleep_for (std::chrono::seconds(1));

   // Get the Gibbs free energy:
   get_gout_value(goodvibes_command.c_str(), G_opt, 8) ;

   // Get the lowest and second lowest frequency:
   get_gout_value("grep 'Frequencies' *out | head -1", 
         freq1, 2) ;
   get_gout_value("grep 'Frequencies' *out | head -1",
         freq2, 3) ;
   if (freq1 < 0) {imaginary_freq = 1;}
   else {imaginary_freq = 0;}

   // Get CPU time:
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         days, 3);
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         hours, 5);
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         minutes, 7);
   get_gout_value("grep 'Job cpu time:' *out | head -1",
         seconds, 9);

   cpu_time = days*24.0*60.0 + hours*60.0 + minutes + seconds/60.0;  

   change_directory(three_up);

   //-------------------------------------
   // higher level of theory:
   //-------------------------------------

   change_directory(higher_level_of_theory);
   change_directory(chemical_species);
   change_directory(current_isomer);

   // Is there only one .out file in this directory?
   one_ext_file("out"); // will exit if there was not 1
   // If there is only one, copy it to out_files_path:
   cp_out_file_command = "cp *.out " + out_files_path + "/"
                        + current_isomer + "_sp.out";
   system(cp_out_file_command.c_str());

   get_gout_value("grep 'SCF Done' *.out | tail -1",
      E_sp, 4);

   H_sp = E_sp + (H_opt - E_opt);
   G_sp = E_sp + (G_opt - E_opt);
   S_sp = (H_sp - G_sp) / T;
   
   change_directory(three_up);

   return 0;
}   
