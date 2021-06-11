/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

#include <unordered_map>

//#define TRACE_DBG

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

int Backtrack::buildMatchingArray(const Graph& query, CandidateSet& cs,
                       std::unordered_map<Vertex, Vertex>& vtxConvMap,
                       std::unordered_map<Vertex, Vertex>& vtxRevConvMap,
                       std::vector<int>& matchingArray, std::vector<int>& visitedMap) {
  Vertex acc = 0; // order of candidate in whole candidates

  for(size_t i = 0; i < query.GetNumVertices(); ++i) {
    for(size_t j = 0; j < cs.GetCandidateSize(i); ++j) {
      // loop for every candidates possible
      Vertex v = cs.GetCandidate(i, j);
      auto it = vtxConvMap.find(v);
      if(it == vtxConvMap.end()) {
        // if no vertex v in vtxConvMap
        vtxConvMap.insert(std::make_pair(v, acc)); // insert v mapped with order
        vtxRevConvMap.insert(std::make_pair(acc, v)); // insert order mapped with v
        cs.SetCandidate(i, j, acc); // build new candidate set with order mapped
        acc++;
      } else {
        // if vertex v already in vtxConvMap, which means that v is already visited by loop
        cs.SetCandidate(i, j, it->second); // bulid new candidate set with order(same with v found before) mapped
      }
    }
  }

  matchingArray.resize(acc * acc); // used array for O(1) search time // todo : replace with vector?
  for(int i = 0; i < acc * acc; ++i)
    matchingArray[i] = -1; // initialize with -1

  visitedMap.resize(acc);
  for(int i = 0; i < acc; ++i)
    visitedMap[i] = 0; // initialize with 0 // todo : why not check visited in if(it == vtxConvMap.end()) and else?

  return acc;
}

void Backtrack::build(const Graph& graph, std::vector<DAGNode>& dag, std::vector<Vertex>& remains, size_t pointer) {
  while(pointer < remains.size()) {

    Vertex id = remains[pointer];
    size_t startN = graph.GetNeighborStartOffset(id);
    size_t endN = graph.GetNeighborEndOffset(id);

    for(size_t i = startN; i < endN; ++i) {
      Vertex v = graph.GetNeighbor(i); // v is neighbor of i
      if(dag[v].IsEmpty()) {
        // i is a parent of v
        dag[id].SetDescendant(v);
        // mark vertex visited by build()
        bool check = true;
        for(auto e : remains) {
          if(e == v) {
            check = false;
            break;
          }
        }
        if(check) 
          remains.push_back(v);
      }
      else {
        // v is a parent of i
        dag[id].SetParent(v);
      }
    }
    ++pointer;
  }
}

size_t pathCost = 0; // todo : delete before submit

Vertex Backtrack::getNext(std::vector<Vertex>& result, std::vector<DAGNode>& dag, std::vector<Vertex>& order) {
  std::vector<Vertex> extendable;

  for(size_t i = 0; i < result.size(); i++) {
    auto v = result[i];
    if(v < 0) {
      bool parentVisited = true;
      for(Vertex p : dag[i].GetParent()) {
        if(result[p] < 0) { 
          // one of parent node is not visited yet
          parentVisited = false;
          break;
        }
      }
      if(parentVisited) {
        // all parent nodes are visited already
        extendable.push_back(i);
      }
    }
  }

  size_t theMostMin = 0;
  Vertex selected = -1;

  for(auto e : extendable) {
    size_t minStep = SIZE_MAX;

    for(auto inEdge : dag[e].GetParent()) {
      // inEdge is parent of vertex e, which is one of extendable vertices
      for(size_t idx = 0; idx < order.size(); idx++) {
        if(order[idx] == inEdge) {
          // loop in order, and find if parent of vertex e is one of query
          if(idx < minStep) { // modify minStep(maximum number of loop)
            minStep = idx;
          }
          break;
        }
      }
    }

    if(selected < 0) {
      // if this is a first of loop
      selected = e; // vertex that last loop looked at
      theMostMin = minStep;
    } else if(minStep > theMostMin) {
      selected = e;
      theMostMin = minStep; // for pathCost calculation // todo : delete before submit
    }
  }

  pathCost += (order.size() - theMostMin);

  return selected;
}

bool verification(const std::vector<Vertex>& result, const Graph& data, const Graph& query, const CandidateSet &cs) {

  //set test
  std::set<Vertex> st;
  for(auto e:result) {
    st.insert(e);
  }
  if(st.size() != result.size()) {
    std::cout << "There's duplicated vertex" << std::endl;
    return false;
  }
  for(size_t i = 0; i < query.GetNumVertices(); ++i) {
    size_t size = cs.GetCandidateSize(i);
    bool chk = false;
    for(size_t j = 0; j < size; ++j) {
      if(result[i] == cs.GetCandidate(i, j)) {
        chk = true;
        break;
      }
    }
    if(!chk){
      std::cout << "Wrong label vertex...." << i << std::endl;
      return false;
    }
  }
  for(size_t i = 0; i < query.GetNumVertices(); ++i) {
    size_t stOffset = query.GetNeighborStartOffset(i);
    size_t endOffset = query.GetNeighborEndOffset(i);
    for(size_t j = stOffset; j < endOffset; ++j) {
      Vertex neighbor = query.GetNeighbor(j);
      if(!data.IsNeighbor(result[i], result[neighbor])) {
        std::cout << "Edge error...." << i << " " << neighbor << " "<< result[i] << " " << result[neighbor] << std::endl;
        return false;
      }
    }
  }
  // std::cout << "verification success" << std::endl;
  return true;
}

void Backtrack::buildOrder(std::vector<Vertex>& result, std::vector<DAGNode>& dag, std::vector<Vertex>& order, Vertex start = 0) {
  pathCost = 0;
  Vertex next = start;
  while(1) {
    if(next < 0) {
      // if this vertex next is last vertex
      break;
    }
    order.push_back(next);
    result[next] = 0;
    next = getNext(result, dag, order);
  }
  order.push_back(-1);
  #ifdef TRACE_DBG
  std::cout << "Path Cost : " << pathCost << std::endl;
  #endif
}

void Backtrack::doCheck(const Graph &data, const CandidateSet &cs, std::vector<Vertex>& result,
             std::vector<DAGNode>& dag, const std::vector<Vertex>& order,
             std::vector<int> visitedMap, std::unordered_map<Vertex, Vertex> vtxRevConvMap,
             std::vector<int> matchingArray, int acc) {

  size_t depth = 0; // search for n-th query vertex

  std::vector<size_t> progress;
  progress.resize(order.size());
  for(auto& e : progress) {
    e = 0;
  }
  while(true) {
    Vertex id = order[depth];
    if(id < 0) {
      // sucessfully arrived at last vertex
      std::cout << "\n" << "a ";
      for(int ord = 0; ord < depth; ord++) {
        auto idx = order[ord];
        std::cout << vtxRevConvMap.at(result[idx]) << " ";
      }
      // get out from loop, once
      id = order[--depth];
      visitedMap[result[id]] = 0;
      result[id] = -1;
      successCount++;
      if(successCount >= 100000)
        return;
    }
    
    int candidateSize = cs.GetCandidateSize(id);
    bool goNext = false; // check invalid subgraph

    for(; progress[depth] < candidateSize; ++progress[depth]) {
      // for all candidates in id
      Vertex candiIdx = cs.GetCandidate(id, progress[depth]);
      bool isValidSubgraph = true;

      if(visitedMap[candiIdx]) {
        // if this candidate have been already visited, look at next candidate
        continue;
      } else {
        for(auto parent : dag[id].GetParent()) {
          Vertex idxB = result[parent];
          int checkIdx = 0;

          if(candiIdx > idxB ) {
            checkIdx = idxB * acc + candiIdx;
          }
          else {
            checkIdx = candiIdx * acc + idxB;
          }

          int& chk = matchingArray[candiIdx * acc + idxB];
          if(chk == -1) {
            Vertex convIdxA = vtxRevConvMap[candiIdx];
            Vertex convIdxB = vtxRevConvMap[idxB];
            if(data.IsNeighbor(convIdxA, convIdxB)) {
              chk = 1;
            }
            else {
              chk = 0;
            }
          }     
          if(!chk) {
            isValidSubgraph = false;
            break;
          }
        }
      }

      if(isValidSubgraph) {
        result[id] = candiIdx;
        visitedMap[candiIdx] = 1;
        progress[depth]++;
        progress[++depth] = 0;
        goNext = true;
        break;
      }
    }

    if(!goNext) {
      if(depth <= 0) break;
      id = order[--depth];
      visitedMap[result[id]] = 0;
      result[id] = -1;
    }
  }
}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query, CandidateSet &cs) {
  
  std::cout << "t " << query.GetNumVertices();
  
  std::vector<DAGNode> dag; // vector containing dagnodes
  dag.resize(query.GetNumVertices());

  size_t val = 1; // total number of routes to check
  size_t accum = 0; // total number of candidates
  for(Vertex v = 0; v < query.GetNumVertices(); v++) {
    val *= cs.GetCandidateSize(v);
    accum += cs.GetCandidateSize(v);
  }

  #ifdef TRACE_DBG
  std::cout << "max combination:" << val << " accum:" << accum << std::endl;
  #endif

  std::vector<Vertex> remains;
  remains.clear(); // visited vertex while build()
  remains.push_back(0);
  size_t pointer = 0;
  build(query, dag, remains, pointer);

  std::vector<Vertex> result; // 
  result.assign(query.GetNumVertices(), -1);
  std::vector<Vertex> order; 

  buildOrder(result, dag, order, 0);
  for(size_t i = 0;i < result.size(); ++i) {
    result[i] = -1;
  }
  
  std::unordered_map<Vertex, Vertex> vtxConvMap;
  std::unordered_map<Vertex, Vertex> vtxRevConvMap;
  std::vector<int> matchingArray;
  std::vector<int> visitedMap;
  int acc = buildMatchingArray(query, cs, vtxConvMap, vtxRevConvMap, matchingArray, visitedMap);
  successCount = 0;
  doCheck(data, cs, result, dag, order, visitedMap, vtxRevConvMap, matchingArray, acc);
  std::cout << std::endl;
}
