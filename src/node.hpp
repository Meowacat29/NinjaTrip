#pragma once

#include <iostream>
#include <list>

class Enemy;

struct PlayerStatus {
	bool m_player_is_dead;
	vec2 m_player_position;
	vec2 m_enemy_position;
	int m_enemy_direction;
};

class Node {
public:
	Node();
	~Node();
	virtual bool run();
	virtual void clean();
};

class CompositeNode : public Node {
private:
	std::list<Node*> childNotes;
public:
	const std::list<Node*>& getChildren() const;
	void addChild(Node* child);
	void clean();
	~CompositeNode();
};

class SelectorNode : public CompositeNode {
public:
	virtual bool run() override;
};

class SequenceNode : public CompositeNode {
public:
	virtual bool run() override;
};

class CheckPlayerIsDeadTask : public Node {
private:
	struct PlayerStatus* status;
public:
	CheckPlayerIsDeadTask(struct PlayerStatus*);
	virtual bool run() override;
	virtual void clean() override;
	~CheckPlayerIsDeadTask();
};

class PatrolTask : public Node {
	private:
		struct PlayerStatus* status;
		class Enemy* enemy;
		float time;
		bool is_player_found;
	public:
		PatrolTask(struct PlayerStatus*, Enemy*);
		void set_player_position(vec2);
		void set_enemy_position(vec2);
		void set_time(float);
		vec2 get_player_position();
		virtual bool run() override;
		virtual void clean() override;
		void find_player(bool);
		~PatrolTask();
};

class ChaseTask : public Node {
	private:
		struct PlayerStatus* status;
		class Enemy* enemy;
		float time;
		bool is_player_caught;
	public:
		void set_player_position(vec2);
		void set_enemy_position(vec2);
		ChaseTask(struct PlayerStatus*, Enemy*);
		virtual bool run() override;
		virtual void clean() override;
		void catch_player(bool);
		~ChaseTask();

};
