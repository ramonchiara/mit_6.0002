#include <algorithm>
#include <map>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

class Node
{
    private:
        string name;

    public:
        Node(string name): name(name) {}

        string getName() const
        {
            return name;
        }

        friend ostream &operator<<(ostream &o, Node &n);
};

ostream &operator<<(ostream &o, Node &n)
{
    return o << n.name;
}

bool operator<(const Node &n1, const Node &n2)
{
    return n1.getName() < n2.getName();
}

bool operator==(const Node &n1, const Node &n2)
{
    return n1.getName() == n2.getName();
}

class Edge
{
    private:
        Node src, dest;

    public:
        Edge(Node src, Node dest): src(src), dest(dest) {}

        Node getSource() const
        {
            return src;
        }

        Node getDestination() const
        {
            return dest;
        }

        friend ostream &operator<<(ostream &o, Edge &e);
};

ostream &operator<<(ostream &o, Edge &e)
{
    return o << e.src.getName() << "->" << e.dest.getName();
}

class ValueError
{
    public:
        string error;
        ValueError(string error): error(error) {}
};

class NameError
{
    public:
        string name;
        NameError(string name): name(name) {}
};

class DiGraph
{
    private:
        map<Node, vector<Node> > edges;

    public:
        void addNode(Node node)
        {
            if (edges.find(node) != edges.end()) {
                throw ValueError("Duplicate node");
            } else {
                edges[node] = vector<Node>();
            }
        }

        virtual void addEdge(Edge edge)
        {
            Node src = edge.getSource();
            Node dest = edge.getDestination();

            if (edges.find(src) == edges.end() || edges.find(dest) == edges.end()) {
                throw ValueError("Node not in graph");
            } else {
                edges[src].push_back(dest);
            }
        }

        vector<Node> &childrenOf(Node &node)
        {
            return edges.at(node);
        }

        bool hasNode(Node &node)
        {
            return edges.find(node) != edges.end();
        }

        Node getNode(string name)
        {
            typedef map<Node, vector<Node> >::const_iterator CI;
            for (CI it = edges.begin(); it != edges.end(); it++) {
                if (it->first.getName() == name) {
                    return it->first;
                }
            }
            throw NameError(name);
        }

        friend ostream &operator<<(ostream &o, DiGraph &d);
};

ostream &operator<<(ostream &o, DiGraph &d)
{
    typedef map<Node, vector<Node> >::const_iterator CISrc;
    typedef vector<Node>::const_iterator CIDest;

    for (CISrc sit = d.edges.begin(); sit != d.edges.end(); sit++) {
        Node src = sit->first;
        vector<Node> destinations = sit->second;
        for (CIDest dit = destinations.begin(); dit != destinations.end(); dit++) {
            Node dest = *dit;
            o << src.getName() << "->" << dest.getName() << endl;
        }
    }

    return o;
}

class Graph : public DiGraph
{
    public:
        void addEdge(Edge edge)
        {
            DiGraph::addEdge(edge);
            Edge rev(edge.getDestination(), edge.getSource());
            DiGraph::addEdge(rev);
        }
};

void buildCityGraph(DiGraph *city)
{
    string names[] = {
        "Boston",
        "Providence",
        "New York",
        "Chicago",
        "Denver",
        "Phoenix",
        "Los Angeles"
    };

    for (int i = 0; i < 7; i++) {
        string name = names[i];
        city->addNode(Node(name));
    }

    city->addEdge(Edge(city->getNode("Boston"), city->getNode("Providence")));
    city->addEdge(Edge(city->getNode("Boston"), city->getNode("New York")));
    city->addEdge(Edge(city->getNode("Providence"), city->getNode("Boston")));
    city->addEdge(Edge(city->getNode("Providence"), city->getNode("New York")));
    city->addEdge(Edge(city->getNode("New York"), city->getNode("Chicago")));
    city->addEdge(Edge(city->getNode("Chicago"), city->getNode("Denver")));
    city->addEdge(Edge(city->getNode("Chicago"), city->getNode("Phoenix")));
    city->addEdge(Edge(city->getNode("Denver"), city->getNode("Phoenix")));
    city->addEdge(Edge(city->getNode("Denver"), city->getNode("New York")));
    city->addEdge(Edge(city->getNode("Los Angeles"), city->getNode("Boston")));
}

ostream &operator<<(ostream &o, vector<Node> &n)
{
    for(unsigned int i = 0; i < n.size(); i++) {
        if (i > 0) {
            o << "->";
        }
        o << n[i].getName();
    }
    return o;
}

// Depth First Search
vector<Node> DFS(DiGraph &graph, Node start, Node end, vector<Node> path, vector<Node> shortest, bool toPrint = false)
{
    path.push_back(start);
    if (toPrint) {
        cout << "Current DFS path: " << path << endl;
    }
    if (start == end) {
        return path;
    }
    vector<Node> children = graph.childrenOf(start);
    typedef vector<Node>::const_iterator CI;
    for (CI it = children.begin(); it != children.end(); it++) {
        Node node = *it;
        if (find(path.begin(), path.end(), node) == path.end()) { // avoid cycles
            if (shortest.empty() || path.size() < shortest.size()) {
                vector<Node> newPath = DFS(graph, node, end, path, shortest, toPrint);
                if (!newPath.empty()) {
                    shortest = newPath;
                }
            }
        } else if (toPrint) {
            cout << "Already visited " << node << endl;
        }
    }
    return shortest;
}

bool printQueue = false;

// Breadth-First Search
vector<Node> BFS(DiGraph &graph, Node start, Node end, bool toPrint = false)
{
    vector<Node> initPath;
    vector<vector<Node> > pathQueue;

    initPath.push_back(start);
    pathQueue.push_back(initPath);

    while(pathQueue.size() != 0) {
        // Get and remove oldest element in pathQueue
        if (printQueue) {
            cout << "Queue: " << pathQueue.size() << endl;
            for (unsigned int i = 0; i < pathQueue.size(); i++) {
                vector<Node> p = pathQueue[i];
                cout << p << endl;
            }
        }
        vector<Node> tmpPath = pathQueue.front();
        pathQueue.erase(pathQueue.begin());
        if (toPrint) {
            cout << "Current BFS path: " << tmpPath << endl << endl;
        }
        Node lastNode = tmpPath.back();
        if (lastNode == end) {
            return tmpPath;
        }
        vector<Node> children = graph.childrenOf(lastNode);
        typedef vector<Node>::const_iterator CI;
        for (CI it = children.begin(); it != children.end(); it++) {
            Node nextNode = *it;
            if (find(tmpPath.begin(), tmpPath.end(), nextNode) == tmpPath.end()) {
                vector<Node> newPath = tmpPath;
                newPath.push_back(nextNode);
                pathQueue.push_back(newPath);
            }
        }
    }

    return vector<Node>();
}

vector<Node> shortestPath(DiGraph &graph, Node start, Node end, bool toPrint = false)
{
    // vector<Node> path, shortest;
    // return DFS(graph, start, end, path, shortest, toPrint);
    return BFS(graph, start, end, toPrint);
}

void testSP(string source, string destination)
{
    DiGraph g;
    buildCityGraph(&g);

    vector<Node> sp = shortestPath(g, g.getNode(source), g.getNode(destination), true);

    if (!sp.empty()) {
        cout << "Shortest path from " << source << " to " << destination << " is " << sp << endl;
    } else {
        cout << "There is no path from " << source << " to " << destination << endl;
    }
}

int main()
{
    // testSP("Chicago", "Boston");
    testSP("Boston", "Phoenix");
}
