#include "functions.h"
#include "sorting_algorithms.h"
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

int main() {

   // Variables:
   std::string chemical_species, sorting_algorithm; 
   std::vector<double> E_opt, G_opt, freq, cpu_time;
   double days, hours, minutes, seconds; // for cpu time
   std::vector<std::string> isomers; // output of ls -d i.e. all
                                     // the subdirectories in
                                     // chemical_species for the isomers
   std::string sortoutfilename;
   std::string one_up = ".."; // go one directory up
   int i ;

   // Read from the input file:
   read_from_input(chemical_species, sorting_algorithm);
   
   // Find all the isomers:
   change_directory(chemical_species);
   run_bash_command("ls -d */  2> /dev/null",isomers);
   pop_back_dir_array(isomers); // because of //n at the end

   E_opt.resize(isomers.size());
   G_opt.resize(isomers.size());
   freq.resize(isomers.size());
   cpu_time.resize(isomers.size());
   
   // Get E_opt, G_opt, freq, and cpu_time for
   // each isomer:
   for (i=0; i<isomers.size(); i=i+1) {
      change_directory(isomers[i]);

      // Get the SCF energy:
      get_gout_value("grep 'SCF Done' *.out | tail -1", 
            E_opt[i], 4);

      // Get the Gibbs free energy
      get_gout_value("grep 'Sum of electronic and thermal Free Energies=' *out | tail -1", G_opt[i], 7) ;

      // Get the frequency:
      get_gout_value("grep 'Frequencies' *out | head -1", freq[i], 2) ;
      
   	// Get CPU time:
   	get_gout_value("grep 'Job cpu time:' *out | head -1",
         	days, 3);
   	get_gout_value("grep 'Job cpu time:' *out | head -1",
         	hours, 5);
  	 	get_gout_value("grep 'Job cpu time:' *out | head -1",
         	minutes, 7);
   	get_gout_value("grep 'Job cpu time:' *out | head -1",
         	seconds, 9);
		cpu_time[i] = days*24.0*60.0 + hours*60.0 + minutes + seconds/60.0;

      change_directory(one_up);
   }

   if (sorting_algorithm == "bubble_sort") {
      bubble_sort(isomers, E_opt, G_opt, freq, cpu_time) ;
   }
   else {
      std::cout << "no sorting algorithm selected" << std::endl;
      exit(1);
   }

   //---------------------------------------------
   // Print sorted energies to file:
   //---------------------------------------------
   
   sortoutfilename = chemical_species + "_sorted_energies";
   if_file_exist_delete(sortoutfilename);
   std::ofstream sorted_energies;
   sorted_energies.open (sortoutfilename.c_str());
   sorted_energies << "isomer " << "E_opt" << " G_opt" 
                   << " smallest_frequency" 
                   << " CPU_time_minutes" << std::endl;
   sorted_energies.precision(11) ;
   for (i=0; i<isomers.size(); i=i+1) {
      sorted_energies << isomers[i] << " " << E_opt[i] << " " 
                      << G_opt[i] << " " << freq[i] 
                      << " " << cpu_time[i] << std::endl;
   }
   sorted_energies.close();

   return 0;
}
