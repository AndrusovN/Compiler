#ifndef AST
#define AST
#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <string>

typedef int _type_id;
typedef int value_type;
typedef int err_type;
	
class Context {
	std::map<std::string, value_type> variables;
	std::vector<std::set<std::string>> scopes;
	public:

	value_type& get(std::string var) {
		if (variables.find(var) == variables.end()) {
			scopes.back().insert(var);
		}
		if (scopes.back().find(var) == scopes.back().end()) {
			throw 1;
		}
		return variables[var];
	}

	void addScope() {
		scopes.push_back({});
	}
	void leaveScope() {
		for(auto var: scopes.back()) {
			variables.erase(var);
		}
		scopes.pop_back();
	}
};

class INode {
public:	
	virtual value_type apply(Context& context) = 0;
};

class Scope : public INode {
private:
	INode* _expr;
public:
	Scope(INode* expr) : _expr(expr) {}

	virtual value_type apply(Context& context) {
		//std::cout << "Scope apply called" << std::endl;
		context.addScope();
		value_type res = _expr->apply(context);
		context.leaveScope();
	}
};


class Plus : public INode {
	INode* _left_child, *_right_child;
public:
	Plus(INode* lc, INode* rc) : _left_child(lc), _right_child(rc) {}

	virtual value_type apply(Context& context) override {
		return _left_child->apply(context) + _right_child->apply(context);
	}
};

class Splitted : public INode {
	INode* _left_child, *_right_child;
public:
	Splitted(INode* lc, INode* rc) : _left_child(lc), _right_child(rc) {}

	virtual value_type apply(Context& context) override {
		value_type res = _left_child->apply(context);
		_right_child->apply(context);
		return res;
	}
};

class Variable : public INode {
private:
	std::string _name;
public:
	Variable(std::string name) : _name(name) {}
	value_type& get(Context& context) {
		return context.get(_name);
	}
	value_type apply(Context& context) {
		return context.get(_name);
	}
};
class Assign : public INode {
private:
	INode* _right_child;
	Variable* _left_child;
public:
	Assign(Variable* lc, INode* rc) : _left_child(lc), _right_child(rc) {}

	virtual value_type apply(Context& context) override {
		return (_left_child->get(context) = _right_child->apply(context));
	}
};

class Log : public INode {
private:
	INode* _expr;
public:
	Log(INode* lc) : _expr(lc) {
		//std::cout << "Log initialized" << std::endl;
	}

	virtual value_type apply(Context& context) override {
		//std::cout << "Log apply called" << std::endl;
		value_type res = _expr->apply(context);			
		std::cout << res << std::endl;
		return res;
	}
};

class Value : public INode {
private:
	value_type _value;
public:
	Value(value_type val) : _value(val) {}

	virtual value_type apply(Context& context) override {
		//std::cout << "Value apply called" << std::endl;
		return _value;
	}
};
#endif

