#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <iterator>

#define NIL -1 
using namespace std;

/* Classes */
class Vertex {
  private:
    int _Id;
    int _grade;
    list<Vertex *> _connections;
  public:
    Vertex() {}
    ~Vertex() {}

    int getGrade() { return _grade; }

    int getId() { return _Id; }

    void setGrade(int newGrade) { _grade = newGrade; }

    void setId(int id) { _Id = id; }

    void addConnections(Vertex *v) { _connections.push_back(v); }

    list<Vertex *> getAdjacents() { return _connections; }

    bool hasConnection(int id) {
      for (Vertex *v : _connections)
        if (v->getId() == id)
          return true;
      return false;
    }
    bool hasConnections(){ return !_connections.empty();}
};

class Graph {
  private:
    int _numVertexes;
    Vertex *_vertexes;
    
  public:
    Graph() {}
    Graph(int vertexes) {
      _vertexes = new Vertex[vertexes];
      _numVertexes = vertexes;
    }

    ~Graph() { delete _vertexes; }

    int getGrade(int num) { return _vertexes[num].getGrade(); }

    void newVert(int id);

    Vertex *getVertex(int id) { return &_vertexes[id]; }

    int getNumVectors() { return _numVertexes; }

    void addConnection(int id, int idConnection) {
      _vertexes[id].addConnections(&_vertexes[idConnection]);
    }

    void getSCC();
    void tarjan(int i, int* d, int* low,list<int> *L);
};

void Graph::newVert(int id){
  int grade = 0;
  if (scanf("%d", &grade) != 1)
    printf("ERRO!\n");
  _vertexes[id].setId(id);
  _vertexes[id].setGrade(grade);
}


/* Global Variable */
Graph *_g;

bool contains(list<int> *lst, int v){
  for(auto it = lst->begin(); it != lst->end(); ++it)
		if(*it == v) return true;
  return false;
}

void maxi(Vertex *a, Vertex *b) {
  if (a->getGrade() > b->getGrade())
    b->setGrade(a->getGrade());

  if(a->hasConnection(b->getId()) && b->getGrade() > a->getGrade())
    maxi(b,a);
}

void compare(list<int> sccList){
  if(sccList.size() == 1 && !_g->getVertex(*sccList.begin())->hasConnections()){
    return;
  }

  else if(sccList.size() == 1 && _g->getVertex(*sccList.begin())->hasConnections()){        
    //O(V)
    for(auto it: _g->getVertex(*sccList.begin())->getAdjacents())
      maxi(it,_g->getVertex(*sccList.begin()));
    return;
  }

  else{
    int max = _g->getVertex(*sccList.begin())->getGrade();
    for(int i: sccList){    //O(V+E)
      if(_g->getVertex(i)->getGrade() > max) max = _g->getVertex(i)->getGrade();

      for(Vertex* v: _g->getVertex(i)->getAdjacents())
        if(v->getGrade() > max) max = v->getGrade();
    }

    for(int i: sccList){  //O(V)
      if(_g->getVertex(i)->getGrade() < max) _g->getVertex(i)->setGrade(max);
    }
  }
}

void Graph::tarjan(int u, int* d, int* low,list<int> *L){  //O(V+E)
  static int visited = 0;
  d[u] = low[u] = visited++;
  L->push_front(u);
  for(auto v: _g->getVertex(u)->getAdjacents()){  
    int vert = v->getId();
  
    if (d[vert]==NIL || contains(L,vert)){
      if (d[vert] == NIL)
        tarjan(vert,d,low,L);
      low[u] = min(low[u], low[vert]);
    }
  }

  int v=0;
  list<int> Scc;
  if (d[u] == low[u]){
    do
    {
      v = L->front();
      Scc.push_back(v);
      L->erase(L->begin());
    }while (u!=v);
  compare(Scc);
  Scc.clear();
  }
}

void Graph::getSCC(){
  int *d = new int[_numVertexes];
  int *low = new int[_numVertexes];
  list<int> *L = new list<int>(); 

  for(int i=0; i<_numVertexes;i++)
    d[i] = NIL, low[i] = NIL;

  for(int i=0; i<_numVertexes;i++)
    if (d[i] == NIL)
      tarjan(i,d,low,L);
}

/* Functions */

void parseCommandLine() {
  int id = 0, idConnection = 0;
  int num_vert = 0, num_edges = 0;

  if (scanf("%d,%d", &num_vert, &num_edges) != 2)
    fprintf(stderr, "Scanf error\n"); //1 linha do input

  if (num_vert < 2){
    fprintf(stderr, "Minimum of students are 2.");
    exit(1);
  }
  else if (num_edges < 1){
    fprintf(stderr, "Minimum connections are 1.");
    exit(1);
  }
  //Inicialize graph
  _g = new Graph(num_vert); 

  for (int i = 1; i <= num_vert; i++) //Set Vertices with grade
    _g->newVert(i - 1);

  for (int i = 1; i <= num_edges; i++) {  //Set relations between
    if (scanf("%d %d", &id, &idConnection) != 2)
      fprintf(stderr, "Error");
    _g->addConnection(id - 1, idConnection - 1);
  }
}

void output() {
  for (int i = 0; i < _g->getNumVectors(); ++i)
    printf("%d\n", _g->getGrade(i));
}

int main() {
  parseCommandLine();

  _g->getSCC();
  
  output();
  return 0;
}