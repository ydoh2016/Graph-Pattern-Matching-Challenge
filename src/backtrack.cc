/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
//#define TRACE_DBG

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

struct DagInfo{
  std::vector<Vertex> in;
  std::vector<Vertex> out;
  bool isEmpty(){
    return in.empty() && out.empty();
  }
};

#include <unordered_map>

size_t pointer = 0;
std::vector<Vertex> remains;
std::unordered_map<Vertex, Vertex> vtxConvMap;
std::unordered_map<Vertex, Vertex> vtxRevConvMap;
Vertex acc = 0;
int* matchingArray = nullptr;
int* visitedMap = nullptr;

int* buildMatchingArray(const Graph& data, const Graph& query, const CandidateSet& cs) {
  for(size_t i = 0; i < query.GetNumVertices(); ++i) {
    for(size_t j = 0; j < cs.GetCandidateSize(i); ++j) {
      Vertex v = cs.GetCandidate(i, j);
      if(vtxConvMap.find(v) == vtxConvMap.end()) {
        vtxConvMap.insert(std::make_pair(v, acc++));
      }
    }
  }
  matchingArray = new int[acc * acc];
  for(int i = 0; i < acc*acc; ++i)
    matchingArray[i] = -1;
  return matchingArray;
}

int* buildMatchingArray2(const Graph& data, const Graph& query, CandidateSet& cs) {
  for(size_t i = 0; i < query.GetNumVertices(); ++i) {
    for(size_t j = 0; j < cs.GetCandidateSize(i); ++j) {
      Vertex v = cs.GetCandidate(i, j);
      auto it = vtxConvMap.find(v);
      if(it == vtxConvMap.end()) {
        vtxConvMap.insert(std::make_pair(v, acc));
        vtxRevConvMap.insert(std::make_pair(acc, v));
        cs.SetCandidate(i,j,acc);
        acc++;
      }
      else{
        cs.SetCandidate(i, j, it->second);
      }
    }
  }
  matchingArray = new int[acc * acc];
  for(int i = 0; i < acc*acc; ++i)
    matchingArray[i] = -1;
  visitedMap = new int[acc];
  for(int i = 0; i < acc; ++i)
    visitedMap[i] = 0;
  return matchingArray;
}

void build(const Graph& graph, std::vector<DAGNode>& dag) {
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

size_t pathCost = 0;

Vertex getNext(std::vector<Vertex>& result, std::vector<DAGNode>& dag, std::vector<Vertex>& order) {
  
  std::vector<Vertex> extendable;
  for(size_t i = 0; i < result.size(); i++) {
    auto v = result[i]; // if result[i] is -1, can it accidentally match v by overflow?
    if(v < 0) {
      bool ok = true;
      for(Vertex p : dag[i].GetParent()) {
        if(result[p] < 0) { // parent node is not visited yet
          ok = false;
          break;
        }
      }
      if(ok) { // all parent nodes are visited already
        extendable.push_back(i);
      }
    }
  }

  size_t theMostMin = 0;
  Vertex selected = -1;
  for(auto e : extendable) {
    size_t minStep = 10000;
    for(auto inEdge:dag[e].GetParent()) {
      for(size_t idx = 0; idx < order.size(); idx++) {
        if(order[idx] == inEdge) {
          if(idx < minStep) {
            minStep = idx;
          }
          break;
        }
      }
    }
    if(selected < 0) {
      selected = e;
      theMostMin = minStep;
    }
    else
    if(minStep > theMostMin) {
      selected = e;
      theMostMin = minStep;
    }
  }
  pathCost += (order.size() - theMostMin);
  // std::cout << "selected, depth " << selected << "," << (order.size() - theMostMin) << std::endl; 
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

void buildOrder(std::vector<Vertex>& result, std::vector<DAGNode>& dag, std::vector<Vertex>& order, Vertex start = 0) {
  pathCost = 0;
  Vertex next = start;
  while(1) {
    if(next < 0)
      break;
    order.push_back(next);
    result[next] = 0;
    next = getNext(result, dag, order);
  }
  order.push_back(-1);
  #ifdef TRACE_DBG
  std::cout << "Path Cost:" << pathCost << std::endl;
  #endif
}

void checkCandidate(const Graph &data, const Graph &query,
                                const CandidateSet &cs,
                                std::vector<Vertex>& result, const std::vector<DagInfo>& dag,
                                const std::vector<Vertex>& order, int inOrOut) {
  
  for(int i = 0; i < order.size(); ++i) {
    Vertex id = order[i];
    if(id < 0) {
      return;
    }
    int candidateSize = cs.GetCandidateSize(id);
    // std::cout << "candidateSize is "<< candidateSize << std::endl;
    for(size_t i = 0; i < candidateSize; ++i) {
      bool checkOk = true;
      Vertex candi = cs.GetCandidate(id, i);
      const std::vector<Vertex>& ways = inOrOut > 0 ? dag[id].out : dag[id].in;
      for(auto outID : ways) {
        bool oneOutCheckOk = false;
        size_t targetCandiSize = cs.GetCandidateSize(outID);
        for(size_t j = 0; j < targetCandiSize; ++j) {
          Vertex targetCandi = cs.GetCandidate(outID, j);
          if(data.IsNeighbor(candi, targetCandi)) {
            oneOutCheckOk = true;
            break;
          }
        }
        if(!oneOutCheckOk) {
          checkOk = false;
          break;
        }
      }
      if(!checkOk) {
        std::cout << "Wow" << std::endl;
      }
    }
  }
}

void doCheck3(const Graph &data, const Graph &query,
                                const CandidateSet &cs,
                                std::vector<Vertex>& result, std::vector<DAGNode>& dag,
                                const std::vector<Vertex>& order,
                                int* visited
                                ) {

  size_t depth = 0;
  std::vector<size_t> progress;
  progress.resize(order.size());
  for(auto& e:progress) {
    e = 0;
  }

  while(true) {

    Vertex id = order[depth];
    
    if(id < 0) {
      static size_t count = 0;
      std::cout << "success " << ++count << "\r";
      // if(!verification(result, data, query, cs)) {
        // getchar();
      // }
      // std::cout << "a ";
      // for(auto e:result) {
      //   std::cout << e << " ";
      // }
      // std::cout << std::endl;
      depth--;
      id = order[depth];
      visited[result[id]] = 0;
      result[id] = -1;
      continue;
    }
    int candidateSize = cs.GetCandidateSize(id);
    // std::cout << "depth:" << depth << " id:" << id << std::endl;
    // std::cout << "candidateSize is "<< candidateSize << std::endl;
    bool goNext = false;
    for(; progress[depth] < candidateSize; ++progress[depth]) {
      Vertex idxA = cs.GetCandidate(id, progress[depth]);
      bool ok = true;
      if(visited[idxA]) {
        continue;
      }
      else {
        for(auto inID : dag[id].GetParent()) {
          Vertex idxB = result[inID];

          // std::cout << "idxA, idxB, inID" << idxA << "," << idxB << "," << inID << std::endl;
          
          int checkIdx = 0;
          if(idxA > idxB ) {
            checkIdx = idxB * acc + idxA;
          }
          else
            checkIdx = idxA * acc + idxB;
          int& chk = matchingArray[idxA*acc + idxB];
          // std::cout << "idxA,idxB,chk " << idxA << "," << idxB << "," << chk << std::endl;
          if(chk == -1) {
            Vertex convIdxA = vtxRevConvMap[idxA];
            Vertex convIdxB = vtxRevConvMap[idxB];
            if(data.IsNeighbor(convIdxA, convIdxB)) {
              chk = 1;
            }
            else {
              chk = 0;
            }
            // std::cout << "conv(" << convIdxA <<","<<convIdxB<<","<<chk<<")"<<std::endl;
          }          
          if(!chk) {
            ok = false;
            break;
          }
        }
      }
      if(ok) {
        result[id] = idxA;
        visited[idxA] = 1;
        progress[depth]++;
        progress[++depth] = 0;
        goNext = true;
        break;
      }
    }
    if(!goNext) {
      if(depth <= 0)
        break;
      --depth;
      id = order[depth];
      visited[result[id]] = 0;
      result[id] = -1;
    }
  }  
}


void Backtrack::PrintAllMatches(const Graph &data, const Graph &query, CandidateSet &cs) {
  
  std::cout << "t " << query.GetNumVertices() << "\n";
  
  std::vector<DAGNode> dag;
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

  std::vector<Vertex> result; // 
  result.assign(query.GetNumVertices(), -1);

  std::vector<Vertex> order; 

  remains.clear(); //
  remains.push_back(0);
  build(query, dag);

  buildOrder(result, dag, order, 0);
  for(size_t i = 0;i < result.size(); ++i) {
    result[i] = -1;
  }
  
  buildMatchingArray2(data, query, cs);
  doCheck3(data, query, cs, result, dag, order, visitedMap);
  std::cout << std::endl;
}
