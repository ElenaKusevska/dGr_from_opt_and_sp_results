#include "functions.h"
#include "get_from_gaussian_output.h"
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
#include <sys/stat.h>
#include <string.h>
#include <algorithm>

int main(int argc, char* argv[]) {

   //--------------------------------------------------------
   // Do you want to the program to get the entropy and
   // enthalpy calculated by gaussian as well?
   //--------------------------------------------------------

   int get_entropy_and_enthalpy;

   if ( argc > 2 ) {
      std::cout << "run program as ./a.out S or ./a.out" << std::endl;
      return (0);
   }
   else if ( argc == 2 ) {
      if ( strcmp(argv[1],"S") == 0 ) { 
         get_entropy_and_enthalpy = 1; 
      }
      else { 
         get_entropy_and_enthalpy = 0; 
      }
   }
   else if (argc < 2 ) {
      get_entropy_and_enthalpy = 0;
   }

   //--------------------------------
   // Variables:
   //--------------------------------

   // Variables to hold the information about reactions, reactants
   // and products, levels of theory:
   std::vector <std::vector <int> > reactants, products; // indeces to 
                                    //specify location of reactants' 
                                    // and products' lables 
                                    // in chemical_species array
   std::vector<std::string> chemical_species; // every reactant/product
   std::vector<std::string> lowest_E_isomer;
   std::vector<std::string> reactions; // reactions to calculate dG
   std::string lower_level_of_theory;
   std::string higher_level_of_theory;

   // Variables to get information out of .out file:
   std::vector<std::string> ls_output_dir;
   std::string one_up = ".."; // go one directory up
   std::string two_up = "../../"; // go two directories up

   // Variables to hold information from .out file:
   std::vector<double> E_opt, G_opt, freq1, freq2, H_opt, cpu_time;
   std::vector<double> E_sp, G_sp, H_sp, S_sp, dGrs, dHrs, dSrs, T; 
   std::vector<int> imaginary_freq; // 1=yes; 0=no

   // Other variables:
   std::string curr_directory; // current directory
   std::string out_files_dir_name = "/out_files"; // cp output files with 
                                                 // names written as 
                                                 // .out and _sp.out
   std::string out_files_path; // full path of out_files
   std::string delete_command; // for using rm -r bash command
   std::string goutfilename, dGroutfilename;
   int i, j, k ;
   int test_dir; // does directory exist?

   //---------------------------------------------
   // Get the information from the input file:
   //---------------------------------------------

   read_from_input(reactants, products, reactions, chemical_species, lower_level_of_theory, higher_level_of_theory);
   
   //---------------------------------------------
   // Get values from gaussian output:
   //---------------------------------------------

   E_opt.resize(chemical_species.size());
   E_sp.resize(chemical_species.size());
   G_opt.resize(chemical_species.size());
   G_sp.resize(chemical_species.size());
   H_opt.resize(chemical_species.size());
   H_sp.resize(chemical_species.size());
   S_sp.resize(chemical_species.size());
   freq1.resize(chemical_species.size());
   freq2.resize(chemical_species.size());
   T.resize(chemical_species.size());
   imaginary_freq.resize(chemical_species.size());
   cpu_time.resize(chemical_species.size());
   lowest_E_isomer.resize(chemical_species.size());

   //-------------------------------------
   // make a directory called out_files:
   //-------------------------------------

   // get the current directory:
   current_directory(curr_directory);
   // define path of out_files directory:
   out_files_path = curr_directory + out_files_dir_name;
   // Does such a directory maybe exist?
   test_dir = directory_exists(out_files_path.c_str()); 
   // If it does exist, delete it: (because it probably has files
   // I don't need at the moment)
   std::cout << "test dir: " << test_dir << std::endl;
   if ( test_dir > 0 ) { // 1 - it was a dir; 2 - it was a file
      delete_command = "rm -r " + out_files_path;
      system(delete_command.c_str());
   }
   // And finally, create the directory:
   if (mkdir(out_files_path.c_str(), 0777) != 0) {
      std::cout << "Could not create directory  " << std::endl;
      exit (1);
   }
   else {
      std::cout << out_files_path << " Directory created" << std::endl;
   }

   //-----------------------------------------
   // Get the free energies:
   //-----------------------------------------

   // Assuming we only have one geometry for each species:
   for (i=0; i<chemical_species.size(); i=i+1) {
      lowest_E_isomer[i] = "only_geometry";
   }

   for (i=0; i<chemical_species.size(); i=i+1) {

      // Are there multiple low E isomers for chemical_species[i] ?
      change_directory(lower_level_of_theory) ;
      change_directory(chemical_species[i]);
      run_bash_command("ls -d */  2> /dev/null",ls_output_dir);
      pop_back_dir_array(ls_output_dir); // because of //n at the end
      change_directory(two_up);
      
      // If there are multiple low E isomers 
      // there will be several subdirectories in this directory:
      if (!ls_output_dir.empty()) {
         std::cout << chemical_species[i] 
            << " has several isomers: " << std::endl;
         print_vector_string(ls_output_dir);

         // Find the lowest energy isomer:
         sort_multiple_geometries (ls_output_dir, chemical_species[i], 
               lower_level_of_theory, higher_level_of_theory,
               lowest_E_isomer[i]);

         // Get the energies, smallest frequency and cpu time
         // for the lowest energy isomer
         gauss_output_multiple_isomers(chemical_species[i], 
               lowest_E_isomer[i], E_opt[i], G_opt[i], freq1[i], freq2[i], 
               H_opt[i], H_sp[i], S_sp[i], T[i], cpu_time[i], E_sp[i],
               G_sp[i], imaginary_freq[i], lower_level_of_theory, 
               higher_level_of_theory, out_files_path, 
               get_entropy_and_enthalpy);
      }
      
      // If there is only one structure:
      else if (ls_output_dir.empty()) {
         gauss_output_one_isomer(chemical_species[i], E_opt[i], G_opt[i], 
               H_opt[i], freq1[i], freq2[i], cpu_time[i], E_sp[i], G_sp[i],
               H_sp[i], S_sp[i], T[i], imaginary_freq[i],
               lower_level_of_theory, higher_level_of_theory, 
               out_files_path, get_entropy_and_enthalpy);
      }
   }

   //---------------------------------------------
   // Print values from Gaussian output to file:
   //---------------------------------------------
   
   goutfilename = "gaussian_output";
   if_file_exist_delete(goutfilename);
   std::ofstream goutput;
   goutput.open (goutfilename.c_str());

   int max_lable_len;
   max_lable_len = 9;
   for (k=0; k<chemical_species.size(); k=k+1) {
      max_lable_len = std::max( max_lable_len, 
            static_cast<int>(chemical_species[k].size()) );
   }

   int max_lowest_isom_len;
   max_lowest_isom_len = 16;
   for (k=0; k<lowest_E_isomer.size(); k=k+1) {
      max_lowest_isom_len = std::max( max_lowest_isom_len, 
            static_cast<int>(lowest_E_isomer[k].size()) );
   }

   int max_char_llt; // maximum characters for lower level of theory
   max_char_llt = std::max(18,
         static_cast<int>(lower_level_of_theory.size()) + 3);

   int max_char_hlt; // maximum characters for higher level of theory
   max_char_hlt = std::max(18,
         static_cast<int>(higher_level_of_theory.size()) + 3);

   // And now to format and print the output:
   goutput << std::setw(max_lable_len +1 ) << std::right << "species" 
           
           << std::setw(max_lowest_isom_len + 1) << std::right 
           << "lowest_E_isomer" 
           
           << std::setw(max_char_llt) << std::right
           << "E_" + lower_level_of_theory 
           
           << std::setw(max_char_llt) << std::right
           << "G_" + lower_level_of_theory 
           
           << std::setw(max_char_hlt) << std::right
           << "E_" + higher_level_of_theory 
           
           << std::setw(max_char_hlt) << std::right
           << "G_" + higher_level_of_theory
           
           << std::setw(8) << std::right << "freq" 
           
           << std::setw(17) << std::right << "cpu_time_minutes" 
           
           << std::endl;

   for (i=0; i<chemical_species.size(); i=i+1) {
      goutput << std::setw(max_lable_len + 1) << std::right 
              << chemical_species[i] 
              
              << std::setw(max_lowest_isom_len + 1) << std::right
              << lowest_E_isomer[i]

              << std::setw(max_char_llt) << std::right
              << std::setprecision(max_char_llt - 3) << E_opt[i] 

              << std::setw(max_char_llt) << std::right
              << std::setprecision(max_char_llt - 3) << G_opt[i]

              << std::setw(max_char_hlt) << std::right
              << std::setprecision(max_char_hlt - 3) << E_sp[i]

              << std::setw(max_char_hlt) << std::right
              << std::setprecision(max_char_hlt - 3) << G_sp[i]
              
              << std::setw(8) << std::setprecision(5) 
              << std::right << freq1[i]

              << std::setw(17) << std::setprecision(7)
              << std::right << cpu_time[i]
              
              << std::endl;
   }

   goutput.close();
      
   //---------------------------------------
   // Gibbs free energies of reaction:
   //---------------------------------------

   dGrs.resize(reactions.size());
   for (k=0; k<reactions.size(); k=k+1) {
      dGrs[k] = 0.0;
      for (j=0; j<products[k].size(); j=j+1) {
         dGrs[k] = dGrs[k] + G_sp[products[k][j]];
      }
      for (j=0; j<reactants[k].size(); j=j+1) {
         dGrs[k] = dGrs[k] - G_sp[reactants[k][j]];
      }

      std::cout << dGrs[k] << std::endl;
   }

   if (get_entropy_and_enthalpy == 1) {

      //-----------------------------------------------
      // Enthalpies, and entropies of reaction:
      //-----------------------------------------------

      dHrs.resize(reactions.size());
      dSrs.resize(reactions.size());
      for (k=0; k<reactions.size(); k=k+1) {
         dHrs[k] = 0.0;
         dSrs[k] = 0.0;
         for (j=0; j<products[k].size(); j=j+1) {
            dHrs[k] = dHrs[k] + H_sp[products[k][j]];
            dSrs[k] = dSrs[k] + S_sp[products[k][j]];
         }
         for (j=0; j<reactants[k].size(); j=j+1) {
            dHrs[k] = dHrs[k] - H_sp[reactants[k][j]];
            dSrs[k] = dSrs[k] - S_sp[reactants[k][j]];
         }
      }
   }

   //---------------------------------------
   // Print Gibbs free energies to file:
   //---------------------------------------

   dGroutfilename = "free_energies_of_reaction";
   if_file_exist_delete(dGroutfilename);
   std::ofstream reaction_energies;
   reaction_energies.open (dGroutfilename.c_str());

   reaction_energies << "lover level od theory: " 
                     << lower_level_of_theory
                     << "     higher level of theory: "
                     << higher_level_of_theory << std::endl
                     << " " << std::endl
                     << "R_i: reactants_j(freq_j) ---> "
                     << "products_j(freq_j)"
                     << std::endl;

   for (i=0; i<reactions.size(); i=i+1) {
      reaction_energies << "-----------------------------------------"
                        << "---\n";
      reaction_energies << reactions[i] << ": ";
      for (j=0; j<reactants[i].size(); j=j+1) { 
         reaction_energies << chemical_species[reactants[i][j]]
                           << "(" << freq1[reactants[i][j]];
         if (freq1[reactants[i][j]] < 0.0) { // print second frequency:
            reaction_energies << " " << freq2[reactants[i][j]]
                              << ")" << " ";
         }
         else { // do not print second frequency
            reaction_energies << ")" << " ";
         }
      }
      reaction_energies << "---> ";
      for (j=0; j<products[i].size(); j=j+1) {
         reaction_energies << chemical_species[products[i][j]]
                           << "(" << freq1[products[i][j]] ;
         if (freq1[products[i][j]] < 0.0) { // print second frequency:
            reaction_energies << " " << freq2[products[i][j]]
                              << ")" << " ";
         }
         else { // do not print second frequency
            reaction_energies << ")" << " ";
         }
      }
      reaction_energies << "dGr = " << dGrs[i]*627.51 
                        << " kcal/mol" << std::endl;

      if (get_entropy_and_enthalpy == 1) {

         //----------------------------------------------
         // Print Enthalpies and entropies to file:
         //----------------------------------------------

         reaction_energies << "                                      "
                           << "dHr = " << dHrs[i]*627.51 << std::endl;
         reaction_energies << "                                      "
                           << "dSr = " << dSrs[i]*627.51 << std::endl;
      }
   }   
   reaction_energies << "--------------------------------------------\n";
      
   reaction_energies.close();

   return 0;
}
