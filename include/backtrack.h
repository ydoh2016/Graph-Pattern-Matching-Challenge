/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"

#include <unordered_map>

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
                       CandidateSet &cs);
  
  int buildMatchingArray(const Graph& query, CandidateSet& cs,
                       std::unordered_map<Vertex, Vertex>& vtxConvMap,
                       std::unordered_map<Vertex, Vertex>& vtxRevConvMap,
                       std::vector<int>& matchingArray, std::vector<int>& visitedMap);

  void build(const Graph& graph, std::vector<DAGNode>& dag, std::vector<Vertex>& remains, size_t pointer);

  Vertex getNext(std::vector<Vertex>& result, std::vector<DAGNode>& dag, std::vector<Vertex>& order);

  void buildOrder(std::vector<Vertex>& result, std::vector<DAGNode>& dag, std::vector<Vertex>& order, Vertex start);

  void doCheck(const Graph &data, const CandidateSet &cs, std::vector<Vertex>& result,
             std::vector<DAGNode>& dag, const std::vector<Vertex>& order,
             std::vector<int> visitedMap, std::unordered_map<Vertex, Vertex> vtxRevConvMap,
             std::vector<int> matchingArray, int acc);
};

#endif  // BACKTRACK_H_
