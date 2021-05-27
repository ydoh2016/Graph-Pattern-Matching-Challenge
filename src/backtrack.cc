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
std::unordered_map<Vertex, size_t> vtxConvMap;
size_t acc = 0;
int* matchingArray = nullptr;

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

void build(const Graph& graph, std::vector<DagInfo>& dag) {

  while(pointer < remains.size()) {
    Vertex id = remains[pointer];
    size_t startN = graph.GetNeighborStartOffset(id);
    size_t endN = graph.GetNeighborEndOffset(id);
    // std::cout << "build startN:" << startN << " endN:" << endN << std::endl;
    for(size_t i = startN; i < endN; ++i) {
      Vertex v = graph.GetNeighbor(i);
      if(dag[v].isEmpty()) {
        dag[id].out.push_back(v);
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
        dag[id].in.push_back(v);
      }
    }
    ++pointer;
  }
}

void build(const Graph& graph, std::vector<DagInfo>& dag, int id) {
  if(!dag[id].isEmpty())
    return;
  size_t startN = graph.GetNeighborStartOffset(id);
  size_t endN = graph.GetNeighborEndOffset(id);
  // std::cout << "build startN:" << startN << " endN:" << endN << std::endl;
  for(size_t i = startN; i < endN; ++i) {
    Vertex v = graph.GetNeighbor(i);
    if(dag[v].isEmpty()) {
      dag[id].out.push_back(v);
    }
    else {
      dag[id].in.push_back(v);
    }
  }
  // std::cout << id << " ";
  // std::cout << "in" << " ";
  // for(auto e:dag[id].in)
  //   std::cout << e << " ";
  // std::cout << "out" << " ";
  // for(auto e:dag[id].out)
  //   std::cout << e << " ";
  // std::cout << std::endl;
  for(auto outV: dag[id].out) {
    build(graph, dag, outV);
  }
}

size_t pathCost = 0;

Vertex getNext(std::vector<Vertex>& result, const std::vector<DagInfo>& dag, std::vector<Vertex>& order) {
  std::vector<Vertex> extendable;
  for(size_t i = 0; i < result.size(); i++) {
    auto v = result[i];
    if(v < 0) {
      bool ok = true;
      for(auto inEdge : dag[i].in) {
        if(result[inEdge] < 0) {
          ok = false;
          break;
        }
      }
      if(ok) {
        extendable.push_back(i);
      }
    }
  }
  size_t theMostMin = 0;
  Vertex selected = -1;
  for(auto e:extendable) {
    size_t minStep = 10000;
    for(auto inEdge:dag[e].in) {
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
  std::cout << "selected, depth " << selected << "," << (order.size() - theMostMin) << std::endl; 
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

void buildOrder(std::vector<Vertex>& result, const std::vector<DagInfo>& dag, std::vector<Vertex>& order, Vertex start = 0) {
  pathCost = 0;
  Vertex next = start;
  while(1) {
    if(next < 0)
      break;
    order.push_back(next);
    result[next] = 0;
    next = getNext(result, dag, order);
  }
  //std::cout << "Order" << std::endl;
  order.push_back(-1);
  // for(auto e:order) {
  //   std::cout << e << std::endl;
  // }
  std::cout << "Path Cost:" << pathCost << std::endl;
}

void doCheck(const Graph &data, const Graph &query,
                                const CandidateSet &cs,
                                std::vector<Vertex>& result, const std::vector<DagInfo>& dag,
                                const std::vector<Vertex>& order,
                                std::set<Vertex>& M,
                                int depth
                                ) {
  Vertex id = order[depth];
  if(id < 0) {
    static size_t count = 0;
    std::cout << "success " << ++count << "\r";
    // if(!verification(result, data, query, cs)) {
    //   getchar();
    // }
    // std::cout << "a ";
    // for(auto e:result) {
    //   std::cout << e << " ";
    // }
    // std::cout << std::endl;
    return;
  }
  int candidateSize = cs.GetCandidateSize(id);
#ifdef TRACE_DBG
  std::cout << "try depth " << depth << " id" << id << " candidateSize is "<< candidateSize << std::endl;
#endif
  for(size_t i = 0; i < candidateSize; ++i) {
    Vertex candi = cs.GetCandidate(id, i);
    bool ok = true;
    if(M.find(candi) != M.end()) {
      ok = false;
    }
    else {
      for(auto inID : dag[id].in) {
        if(!data.IsNeighbor(candi, result[inID])) {
          ok = false;
          break;
        }
      }
    }
    if(ok) {
      #ifdef TRACE_DBG
      std::cout << "progress at " << depth << " id " << id << " to " << i << std::endl;
      #endif
      result[id] = candi;
      M.insert(candi);
      doCheck(data, query, cs, result, dag, order, M, depth + 1);
      M.erase(candi);
      result[id] = -1;
    }
  }
  #ifdef TRACE_DBG
  std::cout << "fail at " << depth << std::endl;
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

void doCheck2(const Graph &data, const Graph &query,
                                const CandidateSet &cs,
                                std::vector<Vertex>& result, const std::vector<DagInfo>& dag,
                                const std::vector<Vertex>& order,
                                std::set<Vertex>& M
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
      M.erase(result[id]);
      result[id] = -1;
      continue;
    }
    int candidateSize = cs.GetCandidateSize(id);
    // std::cout << "candidateSize is "<< candidateSize << std::endl;
    bool goNext = false;
    for(; progress[depth] < candidateSize; ++progress[depth]) {
      Vertex candi = cs.GetCandidate(id, progress[depth]);
      size_t idxA = vtxConvMap[candi];
      bool ok = true;
      if(M.find(candi) != M.end()) {
        ok = false;
      }
      else {
        for(auto inID : dag[id].in) {
          size_t idxB = vtxConvMap[result[inID]];
          
          if(idxA > idxB ) {
            size_t tmp = idxA;
            idxA = idxB;
            idxB = tmp;
          }
          int& chk = matchingArray[idxA*acc + idxB];
          if(chk == -1) {
            if(data.IsNeighbor(candi, result[inID])) {
              chk = 1;
            }
            else {
              chk = 0;
            }
          }          
          if(!chk) {
            ok = false;
            break;
          }
        }
      }
      if(ok) {
        result[id] = candi;
        M.insert(candi);
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
      M.erase(result[id]);
      result[id] = -1;
    }
  }  
}


void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";
  std::vector<DagInfo> dag;
  dag.resize(query.GetNumVertices());
  size_t val = 1;
  size_t accum = 0;
  //std::cout << "candi sizes" << std::endl;
  for(size_t i = 0; i < query.GetNumVertices(); i++) {
    
    val *= cs.GetCandidateSize(i);
    accum += cs.GetCandidateSize(i);
    //std::cout<<cs.GetCandidateSize(i)<<" "<<val<<std::endl;
  }
  std::cout << "max combination:" << val << " accum:" << accum << std::endl;
  // getchar();
  // for(int i = 0; i < dag.size(); i++) {
  //   std::cout << i << " ";
  //   std::cout << "in" << " ";
  //   for(auto e:dag[i].in) {
  //     std::cout << e << " ";
  //   }
  //   std::cout << "out" << " ";
  //   for(auto e:dag[i].out)
  //     std::cout << e << " ";
  //   std::cout << std::endl;
  // }
  std::vector<Vertex> result;
  result.resize(query.GetNumVertices());
  std::vector<Vertex> order;
  // for(Vertex start = 0; start < query.GetNumVertices(); ++start) {
  //   order.clear();
  //   for(auto& e:dag) {
  //     e.in.clear();
  //     e.out.clear();
  //   }
  //   remains.clear();
  //   remains.push_back(start);
  //   build(query, dag, start);
  //   std::cout << "start from " << start << std::endl;
  //   for(size_t i = 0;i < result.size(); ++i) {
  //     result[i] = -1;
  //   }
  //   buildOrder(result, dag, order, start);
  //   for(size_t i = 0;i < result.size(); ++i) {
  //     result[i] = -1;
  //   }
  //   checkCandidate(data, query, cs, result, dag, order, 1);
  //   for(size_t i = 0;i < result.size(); ++i) {
  //     result[i] = -1;
  //   }
  //   checkCandidate(data, query, cs, result, dag, order, -1);
  // }
  remains.clear();
  remains.push_back(0);
  build(query, dag, 0);
  for(size_t i = 0;i < result.size(); ++i) {
    result[i] = -1;
  }
  buildOrder(result, dag, order, 0);
  for(size_t i = 0;i < result.size(); ++i) {
    result[i] = -1;
  }
  buildMatchingArray(data, query, cs);
  std::set<Vertex> M;
  doCheck(data, query, cs, result, dag, order, M, 0);
  //doCheck2(data, query, cs, result, dag, order, M);
  std::cout << std::endl;
}
