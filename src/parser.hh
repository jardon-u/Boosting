// Author: Ugo Jardonnet

#ifndef _PARSER_HH
# define _PARSER_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include "parser_util.hh"

template <typename L>
void get_labels(std::ifstream& is,
                L& labels)
{
  std::string line;
  std::getline(is, line);

  char * s = (char *) line.c_str();
  char ** ss = &s;

  while (**ss != '\n' && **ss != '\0')
  {
    if (successful_eat(ss, "Tumor"))
      labels.push_back(1);
    if (successful_eat(ss, "Normal"))
      labels.push_back(-1);
    (*ss)++;
  }
  if ((is.rdstate() & std::ifstream::failbit ) != 0)
  {
    std::cerr << "[ERR] Unknown error when parsing labels\n";
  }
}

template <typename F>
void get_features(std::ifstream& is,
                  F& features)
{
  std::string line;
  int nb_lines = 0;
  while (std::getline(is, line))
  {
    char  * s = (char *) line.c_str();
    char ** ss = &s;

    std::vector<double> feature;
    while (**ss != '\n' && **ss != '\0')
    {
      double n = eat_number(ss);
      feature.push_back(n);
      (*ss)++;
    }
    if (feature.size() != 0)
      features.push_back(feature);
    nb_lines++;
  }
  if (nb_lines == 0 && (is.rdstate() & std::ifstream::failbit ) != 0)
  {
    std::cerr << "[ERR] Unknown error when parsing features\n";
    std::cerr << "      next token was " << is << std::endl;
  }
}


template <typename L, typename F>
void load_file(const std::string& fn,
               L& labels,
               F& features)
{
  std::ifstream is(fn.c_str());
  if (not is.is_open())
  {
    std::cout << "Error opening file" << std::endl;
    exit(-1);
  }

  // get labels for patients (Tumoral, Normal)
  //std::cout << "get_labels" << std::endl;
  get_labels(is, labels);

  // get transcripts for every genes
  //std::cout << "get_features" << std::endl;
  std::vector< std::vector<double> > transcripts;
  get_features(is, transcripts);

  // convert to feature vectors for each patient
  //std::cout << "convert features" << std::endl;
  features.resize(labels.size());
  for (unsigned j = 0; j < features.size(); j++)
    for (unsigned i = 0; i < transcripts.size(); i++)
      features[j].push_back(transcripts[i][j]);

  //std::assert(labels.size() == features.size());
  std::cout << "Loaded " << fn << " - observations:" << features.size()
            << " features:" << features[0].size() << std::endl;
}


#endif /* _PARSER_HH */

