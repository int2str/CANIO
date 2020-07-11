// CANIO - CAN I/O board firmware
// Copyright (C) 2020 Andre Eisenbach
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE for a copy of the GNU General Public License or see
// it online at <http://www.gnu.org/licenses/>.

#ifndef QUEUE_H
#define QUEUE_H

namespace canio {
namespace utils {

template <typename T>
class Queue {
  struct Node {
    T data;
    Node *next;
  };

 public:
  class iterator {
    explicit iterator(Node *p) : node(p) {}

   public:
    bool operator!=(const iterator &rhs) { return this->node != rhs.node; }

    iterator &operator++() {
      node = node->next;
      return *this;
    }

    iterator operator++(const int) {
      iterator lhs = *this;
      node = node->next;
      return lhs;
    }

    T &operator*() { return node->data; }

   private:
    friend class Queue;
    Node *node;
  };

  Queue() : head(0) {}

  ~Queue() {
    while (!empty()) pop();
  }

  void push(const T &data) {
    if (!head) {
      head = new Node{data, 0};

    } else {
      auto n = head;
      while (n->next) n = n->next;
      n->next = new Node{data, 0};
    }
  }

  void pop() {
    if (empty()) return;

    auto n = head;
    head = head->next;
    delete n;
  }

  iterator erase(const iterator &it) {
    if (head && head == it.node) {
      head = head->next;
      delete it.node;
      return iterator(head);
    }

    auto n = head;
    while (n && n->next) {
      if (n->next == it.node) {
        n->next = it.node->next;
        delete it.node;
        return iterator(n->next);
      }
      n = n->next;
    }

    return iterator(0);
  }

  bool empty() const { return !head; }

  const T &front() { return head->data; }

  iterator begin() { return iterator(head); }

  iterator end() { return iterator(0); }

 private:
  Node *head;
};

}
}

#endif  // QUEUE_H