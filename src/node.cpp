#include <iostream>
#include <list>

#include "player.hpp"
#include "moving_enemy.hpp"
#include "node.hpp"
using namespace std;

Node::Node() {
}

Node::~Node() {
} 

void Node::clean(){
	return;
}

bool Node::run() {
	return 0;
};

CompositeNode::~CompositeNode() {
	clean();
} 

void CompositeNode::clean(){
	for (Node* c: getChildren()){
		if (c != NULL){
			c->clean();
		}
		delete c;
	}
	childNotes.clear();
}

const list<Node*>& CompositeNode::getChildren() const {
	return childNotes;
}

void CompositeNode::addChild(Node* child) {
	childNotes.emplace_back(child);
}

bool SelectorNode::run() {
	for (Node* child : getChildren()) {
		if (child->run())
			return true;
	}
	return false;
}

bool SequenceNode::run() {
	for (Node* child : getChildren()) {
		if (!child->run())
			return false;
	}
	return true;
}

/* CHECK DEATH CLASS*/

CheckPlayerIsDeadTask::CheckPlayerIsDeadTask(struct PlayerStatus* playerStatus)
{
	status = playerStatus;
}

bool CheckPlayerIsDeadTask::run()  {
	return status->m_player_is_dead;
}

void CheckPlayerIsDeadTask::clean(){
	delete status;
}

/* PATROL TASK CLASS*/

PatrolTask::PatrolTask(struct PlayerStatus* playerStatus, Enemy* a_enemy) {
	enemy = a_enemy;
	status = playerStatus;
	is_player_found = false;
}

void PatrolTask::clean(){
	return;
}

bool PatrolTask::run() {
	return is_player_found;
}

vec2 PatrolTask::get_player_position() {
	return status->m_player_position;
}

void PatrolTask::set_player_position(vec2 player_location) {
	status->m_player_position = player_location;
}

void PatrolTask::set_enemy_position(vec2 enemy_position) {
	status->m_enemy_position = enemy_position;
}

void PatrolTask::set_time(float ms) {
	time = ms;
}

void PatrolTask::find_player(bool rv) {
	is_player_found = rv;
}

/* CHASE TASK CLASS*/

ChaseTask::ChaseTask(PlayerStatus* playerStatus, Enemy* v_enemy) {
	status = playerStatus;
	enemy = v_enemy;
	is_player_caught = false;
}

void ChaseTask::clean(){
	return;
}

bool ChaseTask::run() {
	return is_player_caught;
}

void ChaseTask::set_player_position(vec2 player_location) {
	status->m_player_position = player_location;
}

void ChaseTask::set_enemy_position(vec2 enemy_position) {
	status->m_enemy_position = enemy_position;
}

void ChaseTask::catch_player(bool rv) {
	is_player_caught = rv;
}