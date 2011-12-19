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
  char line[1024];
  is.getline(line, 1024);

  char * s = line;
  char ** ss = &s;

  while (**ss != '\n' && **ss != '\0')
  {
    if (successful_eat(ss, "Tumor"))
      labels.push_back(1);
    if (successful_eat(ss, "Normal"))
      labels.push_back(-1);
    (*ss)++;
  }
}

template <typename F>
void get_features(std::ifstream& is,
                  F& features)
{
  while (!is.eof())
  {
    char line[1024] = {0};
    is.getline(line, 1024);

    char  * s = line;
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
  get_labels(is, labels);

  // get transcripts for every genes
  std::vector< std::vector<double> > transcripts;
  get_features(is, transcripts);

  // convert to feature vectors for each patient
  features.resize(labels.size());
  for (unsigned j = 0; j < features.size(); j++)
    for (unsigned i = 0; i < transcripts.size(); i++)
      features[j].push_back(transcripts[i][j]);

  //std::assert(labels.size() == features.size());
  std::cout << "Loaded " << fn << " - labels:" << labels.size()
            << " features:" << features.size() << std::endl;
}


#endif /* _PARSER_HH */

