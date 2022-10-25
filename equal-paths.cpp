#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsRecursion(Node * root, int currdepth, int& otherdepth) {
    //base case: If the root node is NULL, return true since by default its
    //children paths are all the same length
    if(root == NULL) return true;
    
    //if the current node has no children, set otherdepth equal to 
    //the current depth so as we recurse later, we can check to see if 
    //the depth of other paths down the tree matched this depth that we got to
    if(root->left == NULL && root->right == NULL) {
        if(otherdepth == -1) otherdepth = currdepth;
        //if the current depth is different from our recorded other depths, 
        //the paths are unequal
        return currdepth == otherdepth;
    }

    return equalPathsRecursion(root->left, currdepth+1, otherdepth) 
            && equalPathsRecursion(root->right, currdepth+1, otherdepth);

}

bool equalPaths(Node * root)
{
    // Add your code below
    int otherdepth = -1;
    return equalPathsRecursion(root, 0, otherdepth);
}

