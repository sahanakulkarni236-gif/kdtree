#include <bits/stdc++.h>
using namespace std;

const int K = 5; // Number of metrics (CPU, Mem, Net, Disk, Cost)

struct Instance {
    vector<double> metrics;
    string name;
};

struct Node {
    Instance inst;
    Node* left;
    Node* right;
};

// Build KD-Tree
Node* build(vector<Instance>& points, int depth) {
    if (points.empty()) return nullptr;

    int axis = depth % K;
    int mid = points.size() / 2;

    // Use nth_element to find median
    nth_element(points.begin(), points.begin() + mid, points.end(),
                [axis](const Instance &a, const Instance &b) {
                    return a.metrics[axis] < b.metrics[axis];
                });

    Node* root = new Node{points[mid], nullptr, nullptr};

    vector<Instance> left(points.begin(), points.begin() + mid);
    vector<Instance> right(points.begin() + mid + 1, points.end());

    root->left = build(left, depth + 1);
    root->right = build(right, depth + 1);

    return root;
}

// Print KD-Tree (Pre-order traversal)
void printKDTree(Node* root, int depth = 0) {
    if(!root) return;
    string indent(depth*2, ' ');
    cout << indent << "Node: " << root->inst.name << " | Metrics: ";
    for(double m : root->inst.metrics) cout << m << " ";
    cout << " | Axis: " << depth % K << endl;

    printKDTree(root->left, depth + 1);
    printKDTree(root->right, depth + 1);
}

// Squared Euclidean distance
double distanceSq(const vector<double>& a, const vector<double>& b) {
    double dist = 0;
    for (int i = 0; i < K; i++)
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    return dist;
}

// Nearest neighbor search
void nearest(Node* root, const Instance& target,
             int depth, Instance &best, double &bestDist) {

    if (!root) return;

    double d = distanceSq(root->inst.metrics, target.metrics);

    if (d < bestDist) {
        bestDist = d;
        best = root->inst;
    }

    int axis = depth % K;

    Node *next = target.metrics[axis] < root->inst.metrics[axis]
                 ? root->left : root->right;
    Node *other = next == root->left ? root->right : root->left;

    nearest(next, target, depth + 1, best, bestDist);

    if ((target.metrics[axis] - root->inst.metrics[axis]) * 
        (target.metrics[axis] - root->inst.metrics[axis]) < bestDist)
        nearest(other, target, depth + 1, best, bestDist);
}

int main() {
    // Read instances from file
    ifstream fin("instances.txt");
    if(!fin) {
        cerr << "Cannot open instances.txt" << endl;
        return 1;
    }

    vector<Instance> instances;
    while(fin) {
        double cpu, mem, net, disk, cost;
        string name;
        if (!(fin >> cpu >> mem >> net >> disk >> cost)) break;
        getline(fin, name);
        if(name.size() && name[0]==' ') name = name.substr(1); // remove leading space
        instances.push_back({{cpu, mem, net, disk, cost}, name});
    }
    fin.close();

    // Build KD-Tree
    Node* root = build(instances, 0);

    // Print KD-Tree structure
    cout << "\nKD-Tree Structure After Building:\n";
    printKDTree(root);

    // User input for target instance
    cout << "\nEnter target instance metrics separated by space: CPU Mem Net Disk Cost\n";
    vector<double> targetMetrics(K);
    for(int i = 0; i < K; i++)
        cin >> targetMetrics[i];

    Instance target = {targetMetrics, "User Target"};

    // Find nearest instance
    Instance bestMatch;
    double bestDist = 1e18;

    nearest(root, target, 0, bestMatch, bestDist);

    cout << "\nRecommended replacement: " << bestMatch.name << endl;

    return 0;
}