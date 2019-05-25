#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <limits.h>

// What is the current directory?
int current_directory (std::string& curr_dir) {
   char temp [ PATH_MAX ];
   if ( getcwd(temp, PATH_MAX) != 0) {
      curr_dir = temp;
   }
   else {
      std::cout << "problem reading current directory" << std::endl;
      exit(1);
   }
   return (0);
}

int change_directory (std::string dir) {
   int test_chdir;
   std::string curr_dir;

   // Go to directory:
   test_chdir = chdir(dir.c_str());
   if (test_chdir != 0 ) {
      std::cout << "can't change direcotry to: " << dir << std::endl;
      exit(1) ;
   }
   
   // Read current directory:
   current_directory(curr_dir);
   // Print current directory:
   std::cout << curr_dir << std::endl;
   
   return(0);
}

int run_bash_command(const char *command, std::vector<std::string>& ls_output) {
   FILE *fp;
   char outs[PATH_MAX];
   int i;

   ls_output.clear();

   fp = popen(command, "r");
   if (fp == NULL) {
      std::cout << "could not run bash command" << std::endl;
      exit(1);
   }

   i = 0;
   while (fgets(outs, PATH_MAX, fp) != NULL) {
      ls_output.push_back(outs);
      i = i + 1;
   }

   return(0);
}

int split_string_to_words(std::string line, std::vector<std::string>& split_line) {
   
   std::string temp;
   std::stringstream ssin(line);

   while (ssin.good()){
      ssin >> temp;
      split_line.push_back(temp);
   }
}

// To remove the / and /n characters at the end
// of a string array containing the output of "ls"
int pop_back_dir_array(std::vector<std::string>& A) {
   int i;
   for (i=0; i<A.size(); i=i+1) {
      A[i].pop_back();
      A[i].pop_back();
   }
   return(0);
}

int print_vector_int(std::vector<int> A) {
   int i;
   for (i=0; i<A.size(); i=i+1) {
      std::cout << A[i] <<  " " ;
   }
   std::cout << " " << std::endl;
   std::cout << " " << std::endl;
   return 0;
}

int print_vector_double(std::vector<double> A) {
   int i;
   std::cout << std::setprecision(8);
   for (i=0; i<A.size(); i=i+1) {
      std::cout << A[i] <<  " " ;
   }
   std::cout << " " << std::endl;
   std::cout << " " << std::endl;
   return 0;
}

int print_vector_string(std::vector<std::string> A) {
   int i;
   for (i=0; i<A.size(); i=i+1) {
      std::cout << A[i] << " ";
   }
   std::cout << " " << std::endl;
   std::cout << " " << std::endl;
   return 0;
}

int print_matrix_int(std::vector<std::vector<int> > A) {
   int i, j;
   for (i=0; i<A.size(); i=i+1) {
      for (j=0; j<A[i].size(); j=j+1) {
         std::cout << A[i][j] << " ";
      }
      std::cout << " " << std::endl;
   }
   std::cout << " " << std::endl;
   return 0;
}

int print_matrix_string(std::vector<std::vector<std::string> > A) {
   int i, j;
   for (i=0; i<A.size(); i=i+1) {
      for (j=0; j<A[i].size(); j=j+1) {
         std::cout << A[i][j] <<  " " ;
      }
      std::cout << " " << std::endl;
   }
   std::cout << " " << std::endl;
   return 0;
}

// Get one value out of a gaussian output file:
// First run bash command and get output as string, then split string
// to vector of strings (words). You have to know the location 
// (int n) of the value on the line.
// int n is the location in command_output of command_value
int get_gout_value (const char *command, double& command_value, int n) {
   
   std::vector<std::string> command_grep_output;
   std::vector<std::string> command_line_to_vec;
   std::string command_value_string;

   std::cout << "running command: " << command << std::endl;

   run_bash_command(command, command_grep_output);
   split_string_to_words(command_grep_output[0], command_line_to_vec);
   command_value_string = command_line_to_vec[n];
   
   std::cout << "command value string: " << command_value_string << std::endl;
   command_value = std::stod(command_value_string);

   return 0;
}

int swap_double(std::vector<double>& v, int i, int j) {
   double a, b;
   
   a = v[i];
   b = v[j];

   v[i] = b;
   v[j] = a;

   return 0;
}
 
int swap_string(std::vector<std::string>& v, int i, int j) {
   std::string a, b;

   a = v[i];
   b = v[j];

   v[i] = b;
   v[j] = a;
   
   return 0;
}


bool fexists(const char *filename) {
   std::ifstream ifile(filename);
   return ifile.good();
}

int if_file_exist_delete (std::string filename) {
   if (fexists(filename.c_str())) {
      if (std::remove(filename.c_str()) != 0) {
          std::cout << "failed to remove " << filename << std::endl;
          exit(1);
       }
       else {
          std::cout << "removed " << filename << std::endl;
       }
   }
   return 0;
}

int read_from_input(std::string& chemical_species, std::string& \
      sorting_algorithm) {

   //--------------------------------
   // Variables:
   //--------------------------------

   std::string line;
   std::vector<std::string> lines;
   std::vector <std::vector <std::string> > input_matrix; //matrix
                                                // to hold all the
                                                // words from the input
                                                // file exactly as they
                                                // are in the input file.
   std::string temp;
   unsigned int nline; // line in input file
   unsigned int nlines; // number of lines in input file

   //-----------------------------------------------------------
   // Take information from input file
   //-----------------------------------------------------------

   // count number of lines in input file, and store
   // each line as string
   std::ifstream inputfile;
   inputfile.open("input");
   nlines = 0;
   while (getline(inputfile, line)) {
      nlines = nlines + 1;
      lines.push_back(line);
   }

   // break up each line/string into words and store 
   // each word as element [line,j] of input_matrix:
   nline = 0;
   inputfile.clear();
   inputfile.seekg(0, std::ios::beg);
   input_matrix.resize(nlines);
   while (getline(inputfile, line)) {
      std::stringstream ssin(line);
      while (ssin.good()){
         ssin >> temp;
         input_matrix[nline].push_back(temp);
      }
   nline = nline + 1;
   }

   std::cout << " " << std::endl;
   std::cout << "input_matrix: " << std::endl;
   print_matrix_string(input_matrix);

   // Read sorting algorithm and chemical species
   // from input_matrix:
   sorting_algorithm = input_matrix[0][2];
   chemical_species = input_matrix[1][3];
   std::cout << "from input: " << std::endl;
   std::cout << sorting_algorithm << " " 
             << chemical_species << std::endl;

   inputfile.close();
   return 1;
}
        
