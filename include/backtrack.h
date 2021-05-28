/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class DAGNode {
  private:
    std::vector<Vertex> parent;
    std::vector<Vertex> descendant;
  public:
    inline bool IsRoot() const {
      return parent.empty();
    }
      
    inline std::vector<Vertex> GetParent() {
      return parent;
    }
      
    inline void SetParent(Vertex p) {
      parent.push_back(p);
      return;
    }
      
    inline std::vector<Vertex> GetDescendant() {
      return descendant;
    }
      
    inline void SetDescendant(Vertex d) {
      descendant.push_back(d);
      return;
    }
      
    inline bool IsEmpty() {
      return parent.empty() && descendant.empty();
    }
};

class Backtrack {
 public:
  Backtrack();
  ~Backtrack();

  void PrintAllMatches(const Graph &data, const Graph &query,
                       const CandidateSet &cs);
};

#endif  // BACKTRACK_H_
