#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

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

// Does given directory exist?
int directory_exists(const char* path) {
   struct stat info;
   int stats = stat( path, &info );

   if( stats == 0 ) { // could access the path
      if ( S_ISDIR(info.st_mode) == 1 ) { // the file is a directory
         std::cout << "could access and is a directory " 
                   << path << std::endl;
         return 1;
      }
      else { // the path is not a directory
         std::cout << "could access but was not a directory " 
                   << path << std::endl;
         return 2;
      }
   }
   else { // could not access the path
      std::cout << "could not access " << path << std::endl;
      return 0; 
   }
}

//   if( stats != 0 ) {
//      std::cout <<  "cannot access " << path << std::endl;
//      return 0;
//   }
//   else if( info.st_mode & S_IFDIR ){
//      std::cout <<  "is a directory" << path << std::endl;
//      return 1;
//   }
//   else {
//      std::cout <<  "is not a directory" << path << std::endl;
//      return 0;
//   }

int run_bash_command(const char *command, std::vector<std::string>& ls_output) {
   FILE *fp;
   char outs[PATH_MAX];
   int i;

   ls_output.clear();

   // open pipe to read output of shell command:
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

   pclose(fp);
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


// How many files of a given extension are there 
// in the current directory? If it's not one, exit:
int one_ext_file(std::string ext) {
   std::vector<std::string> ls_output_ext;
   std::string ls_ext_command;

   ls_ext_command = "ls *." + ext;
   run_bash_command(ls_ext_command.c_str() ,ls_output_ext);
   pop_back_dir_array(ls_output_ext); // because of //n at the end
   std::cout << ls_output_ext.size() << std::endl;
   if (ls_output_ext.empty()) {
      std::cout << "there are no ." << ext 
                << " files in this directory." << std::endl;
      exit(1);
   }
   else if (ls_output_ext.size() == 1) { return(1); }
   else if (ls_output_ext.size() > 1) {
      std::cout << "you have more than one ." << ext
                << " files in this directory." << std::endl;
      exit(1);
   }
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

   run_bash_command(command, command_grep_output);
   split_string_to_words(command_grep_output[0], command_line_to_vec);
   command_value_string = command_line_to_vec[n];
   std::cout << "command value string: " << command_value_string << std::endl;
   command_value = std::stod(command_value_string);
   std::cout <<  "command value: " << command_value << std::endl;

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
          std::cout << filename << " found and deleted " << std::endl;
       }
   }
   return 0;
}

int read_from_input(std::vector<std::vector<int> >& reactants, \
      std::vector<std::vector<int> >& products, std::vector<std::string>& \
      reactions, std::vector<std::string>& chemical_species, std::string& \
      lower_level_of_theory, std::string& higher_level_of_theory) {

   //--------------------------------
   // Variables:
   //--------------------------------

   // To get the information out of the input file:
   std::string line;
   std::vector<std::string> lines;
   std::vector <std::vector <std::string> > input_matrix; //matrix
                                                // to hold all the
                                                // words from the input
                                                // file exactly as they
                                                // are in the input file.
   std::string temp;
   std::string p_test="p", r_test="r"; // to find r and p at each line
   std::vector <int> r, p; // to specify the index location of r 
                                    // and p label on each
                                    // row of the input_matrix
   int duplicate; // to test if a ractant/product label in a
                              // reaction is already present in the 
                              // chemical_species array
   unsigned int nreactions; // number of reactions
   unsigned int nline; // lines in input file
   unsigned int i, j, k, m;

   //-----------------------------------------------------------
   // Take information from input file
   //-----------------------------------------------------------

   // count number of lines in input file, and store
   // each line as string
   std::ifstream inputfile;
   inputfile.open("input");
   nreactions = 0;
   while (getline(inputfile, line)) {
      nreactions = nreactions + 1;
      lines.push_back(line);
   }
   nreactions = nreactions - 1; // first line is levels of theory
   std::cout << "number of reactions: " << nreactions << std::endl;

   // break up each line/string into words and store 
   // each word as element [line,j] of input_matrix:
   nline = 0;
   inputfile.clear();
   inputfile.seekg(0, std::ios::beg);
   input_matrix.resize(nreactions + 1); // input file has nreactions + 1 lines
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

   // from first line take characters 5 for low level of theory
   // and 10 for high level of theory
   lower_level_of_theory = input_matrix[0][4];
   higher_level_of_theory = input_matrix[0][9];
   std::cout << "levels Of theory: " << std::endl;
   std::cout << lower_level_of_theory << " " 
             << higher_level_of_theory << std::endl;

   // Find the reaction labels, reactants and products
   reactions.resize(nreactions);
   r.resize(nreactions);
   p.resize(nreactions);
   for (i=1; i<input_matrix.size(); i=i+1) {
      reactions[i-1] = input_matrix[i][0]; // reaction labels
      // find where on the line the "p" and "r" symbols are.
      for (j=1; j<input_matrix[i].size(); j=j+1) {
         if (input_matrix[i][j] == r_test) {r[i-1] = j;}
         if (input_matrix[i][j] == p_test) {p[i-1] = j;}
      }
   }
   // now that you have the locations of "r" and "p", rr and
   // pp. Find the labels of the reactants and products on the
   // line. Reactants are from r+1 to p-1. Products are from
   // p+1w to the end.
   // --------------------------------------
   // For the reactants on the first reaction, when 
   // chemical_species.size()=1, and i=1, and that
   // one element hasn't been assigned a value:
   chemical_species.resize(1);
   reactants.resize(nreactions);
   products.resize(nreactions);
   for (i=1; i<input_matrix.size(); i=i+1) {
      if (i == 1) {
         // that first element in chemical species, will simply be
         // the first reactant listed on the first line
         chemical_species[0] = input_matrix[1][r[i-1]+1];
         reactants[0].push_back(0);
         // moving on to the next element. Check if that label is
         // already present in chemical_species
         for (k=r[i-1]+2; k<p[i-1]; k=k+1) { // colums of input_matrix
            duplicate=0;
            for (m=0; m<chemical_species.size(); m=m+1) {
               if (input_matrix[1][k] == chemical_species[m]) {
                  reactants[0].push_back(m); // is already present in 
                                       //chemical_species
                  duplicate=1;
                  break;
               }
            }
            // if this label was not found in chemical_species:
            if (duplicate == 0) {
               chemical_species.push_back(input_matrix[1][k]);
               reactants[0].push_back(chemical_species.size() - 1);
            }
         }
      }
      else { // reactants for every reaction other than the first one:
         duplicate = 0;
         for (k=r[i-1]+1; k<p[i-1]; k=k+1) {
            for (m=0; m<chemical_species.size(); m=m+1) {
               if (input_matrix[i][k] == chemical_species[m]) {
                  reactants[i-1].push_back(m);
                  duplicate = 1;
                  break;
               }
            }
            if (duplicate == 0) {
               chemical_species.push_back(input_matrix[i][k]);
               reactants[i-1].push_back(chemical_species.size() - 1);
            }
         }
      }
      // Products for every reaction
      duplicate = 0;
      for (k=p[i-1]+1; k<(input_matrix[i].size()); k=k+1) {
         for (m=0; m<chemical_species.size(); m=m+1) {
            if (input_matrix[i][k] == chemical_species[m]) {
               products[i-1].push_back(m);
               duplicate = 1;
               break;
            }
         }
         if (duplicate == 0) {
            chemical_species.push_back(input_matrix[i][k]);
            products[i-1].push_back(chemical_species.size() - 1);
         }      
      }
   }

   std::cout << " " << std::endl;
   std::cout << "reactions: " ;
   print_vector_string(reactions) ;
   std::cout << "chemical_species: " ;
   print_vector_string(chemical_species);
   std::cout << "reactants: " << std::endl;
   print_matrix_int(reactants);
   std::cout << "products: " << std::endl;
   print_matrix_int(products);

   inputfile.close();
   return 1;
}
        
