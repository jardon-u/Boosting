// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#include <fstream>
#include <stdlib.h>

#include "classification_tree.hh"
#include "adaboost.hh"
#include "parser.hh"
#include "misclass.hh"

//FIXME: You may have to modify the array length in parser.cc:57
// Remember the longest match must be first in line
const char * flux::parser::functions[] =
{
  "obj1","obj","det","de_obj","coord","mod_rel","mod","ponct","comp",
  "dep_coord","dep","suj","aux_pass","aux_caus","a_obj","head",
  "aff","aux_tps","ats","ato","arg_a","arg","p_obj","missinghead"
};


// ~30
const char * flux::parser::phrase[] =
{
  "ADJWH", "ADJ", "ADVWH", "ADV", "CC", "CLS", "CLR", "CLO", "CS",
  "DETWH", "DET", "ET", "NC", "NPP", "I", "P+D", "PROREL",
  "PONCT", "PROWH", "PRO", "P+PRO", "PREF", "P", "VIMP", "VINF", "VPP",
  "VPR", "VS",  "V",
};

char * load_file(char * fn)
{
  std::ifstream is(fn);
  if (not is.is_open())
  {
    std::cout << "Error opening file" << std::endl;
    exit(-1);
  }

  // get length of file:
  is.seekg (0, std::ios::end);
  unsigned length = is.tellg();
  is.seekg (0, std::ios::beg);

  // allocate memory:
  char * buffer = new char [length+1];

  // read data as a block:
  is.read (buffer,length);
  is.close();

  std::cout << "File loaded" << std::endl;

  buffer[length] = '\0';

  return buffer;
}


bool usage(int argc, char ** argv)
{
  if (argc != 2)
  {
    std::cout << argv[0] << " filename" << std::endl;
    return true;
  }
  return false;
}


int main(int argc, char ** argv)
{
  if (usage(argc,argv))
    return -1;

  char * buffer = load_file(argv[1]);
  flux::parser p;

  try {
    p.parse_corpus(&buffer);
  }
  catch (std::string& s)
  {
    std::cout << " .Parse error:" << std::endl;
    std::cout << s << std::endl;
  }
  catch (...)
  {
    std::cout << "Unexpected error" << std::endl;
  }

  using namespace classification;

  typedef std::vector<double> feature_t;
  typedef classification_tree<feature_t, misclass> classifier_t;
  typedef adaboost<classifier_t> booster_t;

  std::vector<feature_t>        x;
  std::vector<int> y;

  for (std::size_t i = 0; i < p.sentences.size(); i++)
  {
    for (std::size_t j = 0; j < p.sentences[i].words.size(); j++)
    {
      
    }
  }

  booster_t  booster(x,y,5u);
  booster.boost();
}
