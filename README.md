# Boosting
## Generic Machine Learning Library in c++11

This library is a machine learning framework providing an
efficient implementation of Classification and Regression
Tree Boosting in C++11.

This library is headers only. See USAGE section.

### DEPENDENCIES:
        null

### INSTALL:
        # compile a test program (original data subject to copyright)
        ./configure && make

### USAGE:
```c++
        #include "classification_tree.hh"
        #include "adaboost.hh"
        #include "misclass.hh"

        using namespace classification;

        typedef std::vector<double> feature_t;
        typedef classification_tree<feature_t, misclass> classifier_t;
        typedef adaboost<classifier_t> booster_t;

        // learning/test  data
        std::vector<feature_t> lfeatures, tfeatures;
        std::vector<int>       llabels, tlabels;
        ...

        booster_t g(lfeatures, llabels, 15);
        g.boost();

        std::cout << g(tfeatures[0]) << std::endl;
```