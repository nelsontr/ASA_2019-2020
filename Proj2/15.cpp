#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <queue>

using namespace std;
#define NIL -1

/*  Pre-load Class's*/
class Vertex;
class Node;
class Graph;
class ResEdge;

Graph *_g;
int numNodes = 0;
list<Vertex *> nextVertexes;

class ResEdge{
  private:
    int _capacity;
    int _flow;
    Vertex *_origin;
    Vertex *_destiny;

  public:
    ResEdge(Vertex *origin, Vertex *destiny, int capacity){
      _origin = origin;
      _destiny = destiny;
      _capacity = capacity;
      _flow = 0;
    }
    Vertex *getOriginVertex() { return _origin; }
    Vertex *getDestinyVertex() { return _destiny; }
    int getFlow() { return _flow; }
    int getCapacity() { return _capacity; }
    void addFlow() { _flow=1; }
    void setCapacity() { _capacity = 1; }
};

class Vertex{
  private:
    int _id;
    bool _visited;
    Vertex *_parent;
    ResEdge *_parentEdge;
    list<ResEdge *> _edges;
    
  public:
    Vertex(int id){
      _visited = false;
      _parent = NULL;
      _parentEdge = NULL;
      _id = id;
      
    }
    void addEdge(Vertex* destiny, int cap){
      _edges.push_back(new ResEdge(this, destiny, cap));
    }
    void addEdge(ResEdge* destiny){
      _edges.push_back(destiny);
    }
    int getId() { return _id; }
    bool isVisited(){ return _visited; }
    Vertex* getParent(){ return _parent; }
    ResEdge* getParentEdge(){ return _parentEdge; }
    list<ResEdge *> getEdges() { return _edges; }
    void setVisited(bool change) { _visited = change; }
    void setParent(Vertex *pred) { _parent = pred; }
    void setParentEdge(ResEdge *edge) { _parentEdge = edge; }
};

class Node{
  private:
    int _id;
    Vertex  *Vin, *Vout;
    ResEdge *edgeBetween_front;
    ResEdge *edgeBetween_back;

  public:
    Node(){
      _id = numNodes++;
      Vin = new Vertex(_id);
      Vout = new Vertex(_id);
      edgeBetween_front = new ResEdge(Vin, Vout, 1);
      Vin->addEdge(edgeBetween_front);
      edgeBetween_back = new ResEdge(Vout, Vin, 0);
      Vout->addEdge(edgeBetween_back);
    }
    int getId() { return _id; }
    void setId(int id) {_id=id;}
    Vertex *getVin() { return Vin; }
    Vertex *getVout() { return Vout; }
    ResEdge *getEdgeBetween_front() { return edgeBetween_front; }
    ResEdge *getEdgeBetween_back() { return edgeBetween_back; }
    void addEdge(Node *Destiny){
      Vout->addEdge(Destiny->getVin(), 1);
      Destiny->getVin()->addEdge(Vout, 0);
    }
    void reset(){
      Vin->setVisited(false);
      Vin->setParent(NULL);
      Vin->setParentEdge(NULL);

      Vout->setVisited(false);
      Vout->setParent(NULL);
      Vout->setParentEdge(NULL);
    }
    
};

class Graph{
  private:
    Node* _nodes;
    int _avenues,_streets;
    Vertex *Source, *Sink;
  public:
    Graph(int avenues, int streets){
      _avenues = avenues;
      _streets = streets;
      Source = new Vertex(-1);
      Sink = new Vertex(_avenues * _streets);
      _nodes = new Node[_avenues * _streets];
      addConnections();
    }
    Vertex* getSource() { return Source; }
    Vertex* getSink() { return Sink; }
    Node* getListNodes() { return _nodes; }
    Node* getNode(int id) { return &_nodes[id]; }
    Node* getNode(int x, int y) { return getNode(x-1 + (y-1)*_avenues); }
    int getSize() { return _avenues*_streets; }
    void addConnections(){
      for(int i = 0; i < getSize(); i++){
        if((i+1) % _avenues != 0)  //Right
          getNode(i)->addEdge(getNode(i + 1));
        if(i % _avenues != 0)   //Left
          getNode(i)->addEdge(getNode(i - 1));
        if(i + _avenues < getSize()) //Down
          getNode(i)->addEdge(getNode(i + _avenues));
        if(i - _avenues >= 0)  //Up
          getNode(i)->addEdge(getNode(i - _avenues));
      }
    }

    void newMarket(int x, int y){
      Vertex *vout = getNode(x , y)->getVout();
      vout->addEdge(Sink, 1);
    }

    void newCitizen(int x, int y){
      Vertex *vin = getNode(x,y)->getVin();
      Source->addEdge(vin, 1); /*Marado com 3 para o mesmo?*/
    }
};

list<ResEdge *> BFS(){
  list<Vertex *> queue; //fila de vértices a percorrer;
  list<ResEdge *> path;  //lista com o caminho mais curto;
  
  for(int i = 0; i < _g->getSize(); i++){
    _g->getNode(i)->reset();
  }

  Vertex *s = _g->getSource();   
  Vertex *t = _g->getSink();

  t->setVisited(false);
  t->setParent(NULL);
  t->setParentEdge(NULL);
  
  s->setVisited(true);
  queue.push_back(s); 

  //enquanto o t não for descoberto e a lista não estiver vazia, vai percorrer 
  //os vertices. Quando encontrar o t ou a lista acabar, para.
  while(!queue.empty()){ 
    Vertex *aux = queue.front();
    queue.pop_front();
    for(ResEdge *edge : aux->getEdges()){
      if(!edge->getDestinyVertex()->isVisited() && edge->getCapacity() == 1 && edge->getFlow() == 0 ){
        queue.push_back(edge->getDestinyVertex());
        edge->getDestinyVertex()->setVisited(true);
        edge->getDestinyVertex()->setParent(aux);
        edge->getDestinyVertex()->setParentEdge(edge);         
      } 
      if(edge->getDestinyVertex() == t){
        while(!queue.empty()){
          Vertex *temp = queue.front();
          queue.pop_front();
          for(ResEdge *edge1 : temp->getEdges()){
            if (edge1->getDestinyVertex() == t){
              nextVertexes.push_back(temp);
              //printf("AQUI: %d!", temp->getId());
            }
            
          }
        } 
      }
    }
  }

  if(!t->isVisited()) {
    return path;  
  }

  //a partir do t vai fazer backtrack até até chegar ao s, que não tem pai
  //e vai adicionar os arcos ao caminho
  else{
    Vertex *aux = t;
    while(aux->getParent() != NULL){
      path.push_front(aux->getParentEdge());
      aux = aux->getParent();
    }
    return path;
  }   
}

int EdmondsKarp(){
  bool canAdd = true;
  int max_flow = 0;
  list<ResEdge *>path;
  
  while(true){
    path = BFS();
    /*for (ResEdge *x: path)
      printf("%d->%d\n", x->getOriginVertex()->getId(), x->getDestinyVertex()->getId());*/
    if(!path.empty()){
      max_flow += 1;
      for(ResEdge *edge : path){
        edge->addFlow();
        Vertex *orig = edge->getOriginVertex();
        Vertex *dest = edge->getDestinyVertex();
        for (ResEdge *aux : dest->getEdges())
          if(aux->getDestinyVertex() == orig) aux->setCapacity();
      }

      while(!nextVertexes.empty()){
        Vertex *nextV = nextVertexes.front();
        nextVertexes.pop_front();
        Vertex *aux = _g->getSink();
        aux->setParent(nextV);
        for(ResEdge * edgeAux : nextV->getEdges())
          if (edgeAux->getDestinyVertex() == aux) aux->setParentEdge(edgeAux);
        
        while(aux->getParent() != NULL){
          if (aux->getParentEdge()->getFlow() == 1){
            canAdd = false;
            break;
          }
          aux = aux->getParent();
        }

        if(canAdd){
          aux = _g->getSink();
          while(aux->getParent() != NULL){
            ResEdge *edge = aux->getParentEdge();
            edge->addFlow();
            Vertex *orig = edge->getOriginVertex();
            Vertex *dest = edge->getDestinyVertex();
            //printf("%d->%d\n", orig->getId(), dest->getId());
            for (ResEdge *auxE : dest->getEdges())
              if(auxE->getDestinyVertex() == orig) 
                auxE->setCapacity();
            aux = aux->getParent();
          }
          max_flow += 1;
        }
        canAdd = true;
      }
    }
    else break;
  }
  return max_flow;
}


void parseCommandLine(){
  int aven_num = 0, street_num = 0;
  int markets = 0, citizens = 0;

  if (scanf("%d %d", &aven_num, &street_num) != 2)
    fprintf(stderr, "Scanf error\n"); //reads the first line of input

  if (scanf("%d %d", &markets, &citizens) != 2)
    fprintf(stderr, "Scanf error\n"); //reads the second line of input
    
  _g = new Graph(aven_num, street_num);

/*for(int i=0; i<_g->getSize();i++)
    for (auto x: _g->getNode(i)->getVout()->getEdges())
      printf("%d->%d\n", _g->getNode(i)->getId(), x->getDestinyVertex()->getId());*/

  int coordX = 0, coordY = 0;
  for(int i = 0; i < markets; i++){        //reads the location of supermarkets
		if(scanf("%d %d", &coordX, &coordY) != 2);
    _g->newMarket(coordX,coordY);
	}

	for(int i = 0; i < citizens; i++){       //reads the location of citizens
		if(scanf("%d %d",&coordX, &coordY) != 2);
    _g->newCitizen(coordX,coordY);
  }
} 

int main(){
  parseCommandLine();
  printf("%d\n", EdmondsKarp());
}