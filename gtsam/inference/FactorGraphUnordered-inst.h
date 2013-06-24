/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file   FactorGraph-inl.h
 * @brief  Factor Graph Base Class
 * @author Carlos Nieto
 * @author Frank Dellaert
 * @author Alireza Fathi
 * @author Michael Kaess
 * @author Richard Roberts
 */

#pragma once

#include <gtsam/base/FastSet.h>
#include <gtsam/base/treeTraversal-inst.h>
#include <gtsam/inference/VariableIndex.h>
#include <gtsam/inference/FactorGraphUnordered.h>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <stdio.h>
#include <sstream>

namespace gtsam {

  /* ************************************************************************* */
  template<class FACTOR>
  void FactorGraphUnordered<FACTOR>::print(const std::string& s, const KeyFormatter& formatter) const {
    std::cout << s << std::endl;
    std::cout << "size: " << size() << std::endl;
    for (size_t i = 0; i < factors_.size(); i++) {
      std::stringstream ss;
      ss << "factor " << i << ": ";
      if (factors_[i])
        factors_[i]->print(ss.str(), formatter);
    }
  }

  /* ************************************************************************* */
  template<class FACTOR>
  bool FactorGraphUnordered<FACTOR>::equals(const This& fg, double tol) const {
    /** check whether the two factor graphs have the same number of factors_ */
    if (factors_.size() != fg.size()) return false;

    /** check whether the factors_ are the same */
    for (size_t i = 0; i < factors_.size(); i++) {
      // TODO: Doesn't this force order of factor insertion?
      sharedFactor f1 = factors_[i], f2 = fg.factors_[i];
      if (f1 == NULL && f2 == NULL) continue;
      if (f1 == NULL || f2 == NULL) return false;
      if (!f1->equals(*f2, tol)) return false;
    }
    return true;
  }

  /* ************************************************************************* */
  namespace {
    template<class FACTOR, class CLIQUE>
    int _pushClique(FactorGraphUnordered<FACTOR>* fg, const boost::shared_ptr<CLIQUE>& clique) {
      fg->push_back(clique->conditional_);
      return 0;
    }
  }

  /* ************************************************************************* */
  template<class FACTOR>
  template<class CLIQUE>
  void FactorGraphUnordered<FACTOR>::push_back_bayesTree(const BayesTreeUnordered<CLIQUE>& bayesTree)
  {
    // Traverse the BayesTree and add all conditionals to this graph
    int data = 0; // Unused
    treeTraversal::DepthFirstForest(bayesTree, data, boost::bind(&_pushClique<FACTOR,CLIQUE>, this, _1));
  }

  /* ************************************************************************* */
  template<class FACTOR>
  size_t FactorGraphUnordered<FACTOR>::nrFactors() const {
    size_t size_ = 0;
    BOOST_FOREACH(const sharedFactor& factor, factors_)
      if (factor) size_++;
    return size_;
  }

  /* ************************************************************************* */
  template<class FACTOR>
  FastSet<Key> FactorGraphUnordered<FACTOR>::keys() const {
    FastSet<Key> allKeys;
    BOOST_FOREACH(const sharedFactor& factor, factors_)
      if (factor)
        allKeys.insert(factor->begin(), factor->end());
    return allKeys;
  }

  /* ************************************************************************* */
} // namespace gtsam
