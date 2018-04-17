#include "member.h"

#include <queue>
#include <stack>
#include "database.h"

namespace algdb {

void Member::DumpConnections() {
  std::cout << "(" << member_id << ")";
  for (auto &m : connecting_members) {
    auto conn = m.second;
    std::cout << "->" << conn.dst->member_id << 
      "(" << conn.group->group_id << ")";
  }
  std::cout << "\n";
}

void Member::PathToMemberBFS(uint64_t dst_member_id) {
  // Fill in your code here.
  std::queue<Member *> memQueue;
  memQueue.push(this);
  this->color = COLOR_GRAY;
  this->key = 0;
  double depth = 0;
  while(memQueue.size() > 0){
    depth++;
    Member * currMem = memQueue.front();
    memQueue.pop();
    for(std::unordered_map<uint64_t, MemberConnection>::iterator it = currMem->connecting_members.begin(); it != currMem->connecting_members.end(); ++it){
      //uint64_t mem_id = it->first;
      MemberConnection MC = it->second;
      Member * otherMem = MC.dst;
      if(otherMem->color == COLOR_WHITE){
        memQueue.push(otherMem);
        otherMem->color = COLOR_GRAY;
        otherMem->key = depth;
        otherMem->parent = currMem;
        if(otherMem->member_id == dst_member_id){
          break;
        }
      }
    }
    currMem->color = COLOR_BLACK;
  }

}

void Member::PathToMemberIDDFS(uint64_t dst_member_id) {
  // Fill in your code here
  std::stack<Member *> memStack;
  this->key = 0;
  this->color = COLOR_GRAY;
  double depth = 0;
  memStack.push(this);
  double maxDepth = 0;
  int found = 0;
  while(found == 0){
    maxDepth++;
    while(memStack.size()>0 && depth <= maxDepth){
      depth++;
      Member * currMem = memStack.top();
      memStack.pop();
      for(std::unordered_map<uint64_t, MemberConnection>::iterator it = currMem->connecting_members.begin(); it != currMem->connecting_members.end(); ++it){
        MemberConnection MC = it->second;
        Member * otherMem = MC.dst;
        if(otherMem->color == COLOR_WHITE){
          otherMem->color = COLOR_GRAY;
          otherMem->parent = currMem; 
          otherMem->key = depth;
          if(otherMem->member_id == dst_member_id){
            found = 1;
            break;
          }
          memStack.push(otherMem);
        }
      }
    }
  }

}

void Member::PrintPath(Member* dst) {
  Member *curr = dst;
  while(curr && curr != this) {
    std::cout << curr->member_name << " <- ";
    curr = curr->parent;
  }
  std::cout << this->member_name << "\n";
}

}
