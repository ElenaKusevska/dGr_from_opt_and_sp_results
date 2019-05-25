#ifndef GET_FROM_GAUSSIAN_OUTPUT_H
#define GET_FROM_GAUSSIAN_OUTPUT_H
#include <vector>
#include <string>

int gauss_output_one_isomer(std::string chemical_species, double& E_opt, \
      double& G_opt, double& H_opt, double& freq1, double& freq2, \
      double& cpu_time, double& E_sp, double& G_sp, \
      double& H_sp, double& S_sp, double& T, int& imaginary_freq, \
      std::string lower_level_of_theory,std::string higher_level_of_theory,\
      std::string out_files_path, int get_entropy_and_enthalpy);

int sort_multiple_geometries (std::vector<std::string> isomers, \
      std::string chemical_species, std::string lower_level_of_theory, \
      std::string higher_level_of_theory, std::string& lowest_E_isomer);

int gauss_output_multiple_isomers(std::string chemical_species, \
      std::string current_isomer, double& E_opt, double& G_opt, \
      double& freq1, double& freq2, double& H_opt, \
      double& H_sp, double& S_sp, double& T, double& cpu_time, \
      double& E_sp, double& G_sp, int& imaginary_freq, \
      std::string lower_level_of_theory, \
      std::string higher_level_of_theory, std::string out_files_path, \
      int get_entropy_and_enthalpy);

#endif
