#include "database.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <queue>
#include <map>
#include <list>

#include "member.h"
#include "csv_reader.h"

namespace algdb {

Database::Database() {
}


Database::~Database() {
  for (auto event : events) {
    delete event; 
  }

  for (auto member : members) {
    delete member;
  }

  for (auto group : groups) {
    delete group;
  }
}

void Database::LoadEventData(const std::string &event_file, int limit) {
  CsvReader reader;
  reader.OpenFile(event_file.c_str());

  int count = 0;
  while(reader.NextEntry()) {
    count ++;
    if (count > limit) break;
    Event *event = new Event();
    for (int j = 0; j < 48; j++) {
      std::string cell = reader.NextCell();
      switch (j) {
      case 0:
        event->event_id = cell;
        break;
      case 3:
        event->duration = std::stoull(cell);
        break;
      case 6:
        event->fee = std::stoi(cell);
        break;
      case 14:
        event->group_id = std::stoull(cell);
        break;
      case 28:
        event->time = cell;
      case 47:
        event->yes_rsvp_count = std::stoi(cell);
        break;
      }
    }
    events.push_back(event);
  }
  std::cout << events.size() << " events loaded.\n";
}

void Database::LoadMemberData(const std::string &member_file, int limit) {
  CsvReader reader;
  reader.OpenFile(member_file.c_str());

  int count = 0;
  uint64_t last_id = 0;
  Member *member;

  while(reader.NextEntry()) {
    uint64_t id = std::stoull(reader.NextCell());
    if (id != last_id) {
      last_id = id;
      count++;
      if (count > limit) {
        break;
      }

      member = new Member();
      members.push_back(member);
      member->member_id = id;
      for (int j = 0; j < 13; j++) {
        std::string cell = reader.NextCell();
        switch (j) {
        case 5:
          member->lat = std::stod(cell);
          break;
        case 7:
          member->lon = std::stod(cell);
          break;
        case 8:
          member->member_name = cell;
          break;
        case 12:
          member->group_ids.push_back(std::stoull(cell));
        }
      }
      member_id_hash_index[member->member_id] = member;
    }
  }
  std::cout << members.size() << " merbers loaded.\n";
}

void Database::LoadGroupData(const std::string &group_file, int limit) {
  CsvReader reader;
  reader.OpenFile(group_file.c_str());

  int count = 0;
  while(reader.NextEntry()) {
    count++;
    if (count > limit) {
      break;
    }

    Group *group = new Group();
    for (int j = 0; j < 36; j++) {
      std::string cell = reader.NextCell();
      switch (j) {
      case 0:
        group->group_id = std::stoull(cell);
        break;
      case 20:
        group->group_name = cell;
        break;
      case 21:
        group->organizer_id = std::stoull(cell);
      case 29:
        group->rating = std::stof(cell);
        break;
      }
    }
    groups.push_back(group);
    group_id_hash_index[group->group_id] = group;
  }
  std::cout << groups.size() << " groups loaded.\n";
}

void Database::AssociateMembersWithGroups() {
  for (auto member : members) {
    for (auto group_id : member->group_ids) {

      // Check if the group_id is in the group_group_id_hash_index
      if (group_id_hash_index.find(group_id) != 
          group_id_hash_index.end()) {
        auto group = group_id_hash_index[group_id];
        member->groups.push_back(group);
        group->members.push_back(member);
      }
    }
  }
}

void Database::RandomizeGraph(int num_connections) {
  // Create some random connections to make the problem more interesting.
  srand(0);
  for (int i = 0; i < num_connections; i++) {
    int src_index = rand() % members.size();
    int dst_index = rand() % members.size();
    int group_index = rand() % groups.size();

    Member *src = members[src_index];
    Member *dst = members[dst_index];

    if (src == dst) continue;

    if (src->connecting_members.find(dst->member_id) == 
        src->connecting_members.end()) {
      MemberConnection conn;
      conn.group = groups[group_index];
      conn.dst = dst;
      src->connecting_members[dst->member_id] = conn;
      dst->connecting_members[src->member_id] = conn;
    }
  }
}


void Database::LoadData(const std::string &data_folder_path, 
                        int event_limit, 
                        int member_limit, 
                        int group_limit) {
  LoadEventData(data_folder_path + "/events.csv", event_limit);
  LoadMemberData(data_folder_path + "/members.csv", member_limit);
  LoadGroupData(data_folder_path + "/groups.csv", group_limit);
  AssociateMembersWithGroups();
  // RandomizeMembers();
}


void Database::BuildMemberGraph() {
  // Fill in your code here
  for(std::vector<Member *>::iterator it = members.begin(); it != members.end(); ++it){
    Member * currMem = *it;
    for(std::vector<Group *>::iterator it2 = currMem->groups.begin(); it2 != currMem->groups.end(); ++it2){
      Group * currGroup = *it2;
      for(std::vector<Member *>::iterator it3 = currGroup->members.begin(); it3 != currGroup->members.end(); ++it3){
        Member * otherMem = *it3;
        if(currMem == otherMem){
          continue;
        }
        if(currMem->connecting_members.find(otherMem->member_id) == currMem->connecting_members.end()){
          MemberConnection currToOther;
          currToOther.group = currGroup;
          currToOther.dst = otherMem;
          currMem->connecting_members[otherMem->member_id] = currToOther;
        }
        if(otherMem->connecting_members.find(currMem->member_id) == otherMem->connecting_members.end()){
          MemberConnection otherToCurr;
          otherToCurr.group = currGroup;
          otherToCurr.dst = currMem;
          otherMem->connecting_members[currMem->member_id] = otherToCurr;
        }
      }

    }
  }
  
}

double Database::BestGroupsToJoin(Member *root) {
  // Fill in your code here
  double totalWeight = 0.0;
  // Search all nodes, initializing them
  std::list<std::pair<uint64_t, MemberConnection>> memList;
  std::queue<Member *> memQueue;
  memQueue.push(root);
  int numNodes = 0;
  while(memQueue.size() >0){
    Member * currMem = memQueue.front();
    memQueue.pop();
    for(std::unordered_map<uint64_t, MemberConnection>::iterator it = currMem->connecting_members.begin(); it != currMem->connecting_members.end(); ++it){
      MemberConnection MC = it->second;
      Member* otherMem = MC.dst;
      if(otherMem->color == COLOR_WHITE){
        otherMem->key = -1;
        otherMem->color = COLOR_GRAY;
        memQueue.push(otherMem);
        numNodes++;
      }
    }
  }
  std::cout<<"Total members: "<<numNodes<<std::endl;
  // Initialize root
  root->key = 0;
  root->color = COLOR_BLACK;
  root->parent = NULL;
  Member * currMem = root;
  while(numNodes>1){
    for(std::unordered_map<uint64_t, MemberConnection>::iterator it = currMem->connecting_members.begin(); it != currMem->connecting_members.end(); ++it){
      MemberConnection MC = it->second;
      Member * otherMem = MC.dst;
      double weight = MC.GetWeight();
      if(otherMem->color == COLOR_GRAY){
        std::list<std::pair<uint64_t, MemberConnection>>::iterator it2 = memList.begin();
        while(it2 != memList.end()){
          if(it2->second.dst->color == COLOR_BLACK){
            it2 = memList.erase(it2);
            continue;
          }
          if(it2->second.GetWeight() > weight){
            break;
          }
          ++it2;
        }
        std::pair<uint64_t, MemberConnection> p = std::make_pair(currMem->member_id,it->second);
        memList.insert(it2,p);
      }
    }
    while(memList.front().second.dst->color != COLOR_GRAY){
      memList.pop_front();
    }
    if(memList.front().second.dst->color == COLOR_GRAY){
      std::pair<uint64_t, MemberConnection> bestPair = memList.front();
      memList.pop_front();
      Member * newMem = bestPair.second.dst;
      newMem->parent = member_id_hash_index[bestPair.first];
      numNodes--;
      MemberConnection MC = bestPair.second;
      newMem->color = COLOR_BLACK;
      newMem->key = MC.GetWeight();
      currMem = newMem;
      totalWeight += MC.GetWeight();
    }
  }

  return totalWeight;
}

}
