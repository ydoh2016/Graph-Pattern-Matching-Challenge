/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

struct DagInfo{
  std::vector<Vertex> in;
  std::vector<Vertex> out;
  bool isEmpty(){
    return in.empty() && out.empty();
  }
};

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

Vertex getNext(std::vector<Vertex>& result, const std::vector<DagInfo>& dag) {
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
        //std::cout << i << " is selected for next" << std::endl; 
        return i;
      }
    }
  }
  return -1;
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

void buildOrder(std::vector<Vertex>& result, const std::vector<DagInfo>& dag, std::vector<Vertex>& order) {
  while(1) {
    Vertex next = getNext(result, dag);
    if(next < 0)
      break;
    order.push_back(next);
    result[next] = 0;
  }
  //std::cout << "Order" << std::endl;
  order.push_back(-1);
  // for(auto e:order) {
  //   std::cout << e << std::endl;
  // }
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
    if(!verification(result, data, query, cs)) {
      getchar();
    }
    // std::cout << "a ";
    // for(auto e:result) {
    //   std::cout << e << " ";
    // }
    // std::cout << std::endl;
    return;
  }
  int candidateSize = cs.GetCandidateSize(id);
  // std::cout << "candidateSize is "<< candidateSize << std::endl;
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
      result[id] = candi;
      M.insert(candi);
      doCheck(data, query, cs, result, dag, order, M, depth + 1);
      M.erase(candi);
      result[id] = -1;
    }
  }
}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";
  std::vector<DagInfo> dag;
  dag.resize(query.GetNumVertices());
  build(query, dag, 0);
  size_t val = 1;
  //std::cout << "candi sizes" << std::endl;
  for(size_t i = 0; i < query.GetNumVertices(); i++) {
    
    val *= cs.GetCandidateSize(i);
    //std::cout<<cs.GetCandidateSize(i)<<" "<<val<<std::endl;
  }
  //std::cout << "max combination:" << val << std::endl;
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
  for(size_t i = 0;i < result.size(); ++i) {
    result[i] = -1;
  }
  buildOrder(result, dag, order);
  for(size_t i = 0;i < result.size(); ++i) {
    result[i] = -1;
  }
  std::set<Vertex> M;
  doCheck(data, query, cs, result, dag, order, M, 0);
  std::cout << std::endl;
}
