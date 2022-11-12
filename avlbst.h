#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft( AVLNode<Key,Value>* node);
    void rotateRight( AVLNode<Key,Value>* node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
  // TODO
  // Code re-uses some aspects from the bst insert function, except it is changed
  // to now work wiht AVLNodes
  if(this->internalFind(new_item.first) != nullptr) {
      this->internalFind(new_item.first)->setValue(new_item.second);
      return;
  }

  //case of insertion in an empty tree
  if(this->root_ == nullptr) {
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      return;
  }
  
  //advances down the tree by comparing key values
  //if an open spot exists for a possible child, a node is created
  //otherwise, it goes down the corresponding path and continues checking
  AVLNode<Key, Value>* temp = dynamic_cast<AVLNode<Key, Value>*>(this->root_);
  AVLNode<Key, Value>* current = nullptr;
  while(true) {
      if(temp->getKey() > new_item.first) {
          if(temp->getLeft() == nullptr) {
              temp->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
              current = temp->getLeft();
              break;
          }
          temp = temp->getLeft();
      }
      else if(temp->getKey() < new_item.first) {
          if(temp->getRight() == nullptr) {
              temp->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
              current = temp->getRight();
              break;
          }
          temp = temp->getRight();
      }
  }

  // This while loop fixes the tree after the insertion
  // First, the balance is updated for the parent node
  // Depending on the new updated balance, rotation may or may not occur
  // If the updated balance of the parent is zero, the height of the tree has not changed
  // and thus there is no need to further propogate changes
  // If the updated balance of the parent is 2 or -2, another check is made
  // for the current node's balance to see if a zig zag or a zag zig is required
  // If the updated balance of the parent is 1 or -1, the tree is still updated
  // but the height of the tree has changed, and thus that must be propogated upwarsd
  // to the parent's parent, i.e. current's grandparent
  AVLNode<Key, Value>* parent = current->getParent();
  while(parent != nullptr) {
      if(current == parent->getLeft()) {
          //current is a left child
          parent->updateBalance(-1);
      }
      else {
          //current is a right child
          parent->updateBalance(1);
      }

      if(parent->getBalance() == 0) break;
      else if(parent->getBalance() == 2) {
          if(current->getBalance() == -1) {
            rotateRight(current);
          }
          rotateLeft(parent);
          break;
      }
      else if(parent->getBalance() == -2) {
          if(current->getBalance() == 1) {
            rotateLeft(current);
          }
          rotateRight(parent);
          break;
      }

      current = parent;
      parent = current->getParent();

  }

}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // Also reuses code from the bst that is now modified for AVLNode
    AVLNode<Key, Value>* temp = (AVLNode<Key, Value>*)this->internalFind(key);
    if(temp == nullptr) return;

    //in the case of two children
    if(temp->getLeft() != nullptr && temp->getRight() != nullptr) {
        nodeSwap(temp, (AVLNode<Key, Value>*)this->predecessor(temp));
    }

    //At this point, we are guaranteed that temp has at most one child
    AVLNode<Key, Value>* child = nullptr;
    if(temp->getLeft() != nullptr) child = temp->getLeft();
    else if(temp->getRight() != nullptr) child = temp->getRight();

    AVLNode<Key, Value>* parent = temp->getParent();
    //if temp has a parent, promote the child to the parent and delete temp
    if(parent != nullptr) {
        if(temp == parent->getLeft()) {
            parent->setLeft(child);
            parent->updateBalance(1);
        }
        else {
            parent->setRight(child);
            parent->updateBalance(-1);
        }
    }
    //if temp has no parent, this means that temp is the root node
    else {
        this->root_ = child;
    }
    
    if(child != nullptr) {
        child->setParent(temp->getParent());
    }
    delete temp;

    // While loop to fix the tree after removal
    // if the parent's balance is 2 or -2, the appropriate rotations are done to fix
    // the tree
    // there is also a case where after rotation, the height of the parent actually has
    // not changed, and thus an if statement afterwards checks, and breaks early if needed
    // otherwise, change is propogated upwards onto gradparents to continue checking
    while(parent != nullptr) {
      if(parent->getBalance() == 2) {
          if(parent->getRight()->getBalance() == -1) {
            rotateRight(parent->getRight());
          }
          rotateLeft(parent);
          parent = parent->getParent();
      }
      else if(parent->getBalance() == -2) {
          if(parent->getLeft()->getBalance() == 1) {
            rotateLeft(parent->getLeft());
          }
          rotateRight(parent);
          parent = parent->getParent();
      }

      if(abs(parent->getBalance()) == 1) break;

      temp = parent;
      parent = parent->getParent();
      if(parent != nullptr) {
          if(temp == parent->getLeft()) {
              parent->updateBalance(1);
          }
          else {
              parent->updateBalance(-1);
          }
      }

    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// this function rotates the subtree to the left
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft( AVLNode<Key,Value>* node)
{
    AVLNode<Key, Value>* a = node;
    AVLNode<Key, Value>* b = a->getRight();
    AVLNode<Key, Value>* x = a->getLeft();
    AVLNode<Key, Value>* y = b->getLeft();
    AVLNode<Key, Value>* z = b->getRight();

    AVLNode<Key, Value>* parent = a->getParent();

    // the majority of this code is manual reassignment of the node pointers, done in constant time
    if(parent != nullptr) {
        if(a == parent->getLeft()) {
            //a is a left child
            parent->setLeft(b);
            b->setParent(parent);
        }
        else {
            //a is a right child
            parent->setRight(b);
            b->setParent(parent);
        }
    }
    else {
        this->root_ = b;
        b->setParent(NULL);
    }

    b->setLeft(a);
    a->setParent(b);
    a->setLeft(x);
    if(x != NULL) x->setParent(a);
    a->setRight(y);
    if(y != NULL) y->setParent(a);
    b->setRight(z);
    if(z != NULL) z->setParent(b);

    int oldBfA = a->getBalance();
    int oldBfB = b->getBalance();

    // these nwew balances are derived from a system of equations where
    // the new balances of a and b are given in terms of a single subtree's height.
    // in which after doing the correct operations, all the heights cancel out
    // and the new balances of a and b are expressed in by the old balances of a and b
    a->setBalance(-1 * std::max(0, (int)oldBfB) - 1 + oldBfA);
    b->setBalance(oldBfB + std::min(0, (int)a->getBalance()) -1);
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight( AVLNode<Key,Value>* node)
{
    AVLNode<Key, Value>* b = node;
    AVLNode<Key, Value>* a = b->getLeft();
    AVLNode<Key, Value>* x = a->getLeft();
    AVLNode<Key, Value>* y = a->getRight();
    AVLNode<Key, Value>* z = b->getRight();

    AVLNode<Key, Value>* parent = b->getParent();

    // the majority of this code is manual reassignment of the node pointers, done in constant time
    if(parent != nullptr) {
        if(b == parent->getLeft()) {
            //b is a left child
            parent->setLeft(a);
            a->setParent(parent);
        }
        else {
            //b is a right child
            parent->setRight(a);
            a->setParent(parent);
        }
    }
    else {
        this->root_ = a;
        a->setParent(NULL);
    }

    a->setLeft(x);
    if(x != NULL) x->setParent(a);
    a->setRight(b);
    b->setParent(a);
    b->setLeft(y);
    if(y != NULL) y->setParent(b);
    b->setRight(z);
    if(z != NULL) z->setParent(b);

    int oldBfA = a->getBalance();
    int oldBfB = b->getBalance();


    // these nwew balances are derived from a system of equations where
    // the new balances of a and b are given in terms of a single subtree's height.
    // in which after doing the correct operations, all the heights cancel out
    // and the new balances of a and b are expressed in by the old balances of a and b
    b->setBalance(oldBfB + 1 + std::max(0, (int)oldBfA) - oldBfA);
    a->setBalance(oldBfA + 1 + std::max(0, (int)b->getBalance()));
}

#endif
