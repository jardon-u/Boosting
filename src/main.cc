// Computational Biology
// Analysis of Gene Microarray Data

#include <iostream>
#include <random>

#include "parser.hh"
#include "classification_tree.hh"
#include "adaboost.hh"
#include "gini.hh"
#include "misclass.hh"



bool usage(int argc, char ** argv)
{
  if (argc != 2)
  {
    std::cout << argv[0] << " ./filename.data" << std::endl;
    return true;
  }
  return false;
}

//FIXME: rewrite this piece of code
template < typename T, typename U >
void fill_data0(T& features,  U& labels,
                T& lfeatures, U& llabels,
                T& tfeatures, U& tlabels)
{
  int split = 0.7 * features.size();
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

std::random_device rd;
std::mt19937 gen(0);

template < typename T, typename U >
void fill_data(T& features,  U& labels,
               T& lfeatures, U& llabels,
               T& tfeatures, U& tlabels)
{
  std::uniform_real_distribution<> dis(0, 1);
  for (unsigned i = 0; i < features.size(); i++)
  {
    if (dis(gen) < 0.8)
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

template < typename F, typename L, typename G>
void
print_accuracy(std::string msg, F& tfeatures, L& tlabels, G& g)
{
  float accuracy = 0;
  for (unsigned i = 0; i < tfeatures.size(); i++)
  {
    //std::cout << g(tfeatures[i]) << " | " << tlabels[i] << std::endl;
    accuracy += static_cast<int>(g(tfeatures[i]) == tlabels[i]);
  }
  std::cout << msg << accuracy / tfeatures.size() <<
    " = " << accuracy << "/" << tfeatures.size() << std::endl;
}

int main(int argc, char ** argv)
{
  if (usage(argc, argv))
    return 1;

  using namespace classification;

  typedef std::vector<double> feature_t;
  typedef classification_tree<feature_t, misclass> classifier_t;
  typedef adaboost<classifier_t> booster_t;

  std::cout << "load_file" << std::endl;
  std::vector<feature_t> features;
  std::vector<int>       labels;
  load_file(std::string(argv[1]),labels,features);

  std::cout << "learning/test data" << std::endl;
  // learning/test data
  std::vector<feature_t> lfeatures, tfeatures;
  std::vector<int>       llabels, tlabels;

  fill_data(features, labels,
            lfeatures, llabels,
            tfeatures, tlabels);

  std::cout << "learning set: " << lfeatures.size() << std::endl;
  std::cout << "test     set: " << tfeatures.size() << std::endl;

  std::cout << "Boost" << std::endl;
  booster_t g(lfeatures, llabels, 10);
  g.boost();

  print_accuracy("learning set accuracy: ", lfeatures, llabels, g);
  print_accuracy("testing set accuracy: ", tfeatures, tlabels, g);
}
