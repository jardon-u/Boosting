// Computational Biology
// Analysis of Gene Microarray Data

#include <cstdlib>
#include <iostream>
#include "parser.hh"
#include "classification_tree.hh"
#include "adaboost.hh"
#include "gini.hh"
#include "misclass.hh"


bool usage(int argc, char ** argv)
{
  if (argc != 1)
  {
    std::cout << argv[0] << std::endl;
    return true;
  }
  return false;
}

//FIXME: rewrite this piece of code
template < typename T, typename U >
void fill_data(T& features,  U& labels,
               T& lfeatures, U& llabels,
               T& tfeatures, U& tlabels)
{
  for (unsigned i = 0; i < 51; i++)
  {
    if (i < 45)
    {
      lfeatures.push_back(features[i]);
      llabels.push_back(labels[i]);
    }
    else
    {
      tfeatures.push_back(features[i]);
      tlabels.push_back(labels[i]);
    }
  }
  for (unsigned i = 52; i < 102; i++)
  {
    if (i < 97)
    {
      lfeatures.push_back(features[i]);
      llabels.push_back(labels[i]);
    }
    else
    {
      tfeatures.push_back(features[i]);
      tlabels.push_back(labels[i]);
    }
  }
}

int main(int argc, char ** argv)
{
  if (usage(argc, argv))
    return 1;

  using namespace classification;

  typedef std::vector<double> feature_t;
  typedef classification_tree<feature_t, misclass> classifier_t;
  typedef adaboost<classifier_t> booster_t;

  std::vector<feature_t> features;
  std::vector<int>       labels;
  load_file(std::string("./comBioData/Prostate1.data"),labels,features);

  // learning/test  data
  std::vector<feature_t> lfeatures, tfeatures;
  std::vector<int>       llabels, tlabels;

  fill_data(features, labels,
            lfeatures, llabels,
            tfeatures, tlabels);

  booster_t g(lfeatures, llabels, 15);
  g.boost();

  float accuracy = 0;
  for (unsigned i = 0; i < tfeatures.size(); i++)
  {
    std::cout << g(tfeatures[i]) << " | " << tlabels[i] << std::endl;
    accuracy += static_cast<int>(g(tfeatures[i]) == tlabels[i]);
  }
  std::cout << "accuracy: " << accuracy / tfeatures.size() << std::endl;
}
