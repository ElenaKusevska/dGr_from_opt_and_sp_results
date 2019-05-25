#ifndef FUNCTIONS_H // include guard
#define FUNCTIONS_H
#include <vector>
#include <string>

int current_directory (std::string& curr_dir) ;

int change_directory (std::string dir);

int directory_exists(const char* const path);

int run_bash_command(const char *command, std::vector<std::string>& ls_outpu);

int split_string_to_words(std::string line, std::vector<std::string>& split_line);

int pop_back_dir_array(std::vector<std::string>& A);

int print_vector_int(std::vector<int> A);

int print_vector_double(std::vector<double> A);

int print_vector_string(std::vector<std::string> A);

int print_matrix_int(std::vector<std::vector<int> > A);

int print_matrix_string(std::vector<std::vector<std::string> > A);

int one_ext_file(std::string ext);

int get_gout_value (const char *command, double& command_value, int n);

int swap_double(std::vector<double>& v, int i, int j);

int swap_string(std::vector<std::string>& v, int i, int j);

bool fexists(const char *filename);

int if_file_exist_delete (std::string filename);

int read_from_input(std::vector<std::vector<int> >& reactants, \
      std::vector<std::vector<int> >& products, std::vector<std::string>& \
      reactions, std::vector<std::string>& chemical_species, std::string& \
      lower_level_of_theory, std::string& higher_level_of_theory);

#endif
