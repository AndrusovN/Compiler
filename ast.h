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

#if DEBUG_MODE
	#define debug(x) std::cout << (x) << std::endl;
#else
	#define debug(x) ;
#endif

class INode;

	
class Context {
	std::vector<std::map<std::string, value_type>> variables;
	std::vector<value_type> results;
	std::map<std::string, std::pair<INode*, INode*>> functions;

	public:

	value_type& get(std::string var) {
		for(int i = variables.size() - 1; i >= 0; i--) {
			if (variables[i].find(var) != variables[i].end()) {
				return variables[i][var];
			}
		}
		
		return variables.back()[var];
	}

	void addScope() {
		variables.push_back({});
		results.push_back(0);
	}
	void leaveScope() {
		variables.pop_back();
		results.pop_back();
	}

	void setResult(value_type v) {
		results[results.size() - 1] = v;
	}

	value_type getResult() {
		return results.back();
	}

	void addFunction(std::string name, INode* func, INode* args) {
		functions[name] = std::make_pair(func, args);
	}

	std::pair<INode*, INode*> getFunction(std::string name) {
		return functions[name];
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

	virtual value_type apply(Context& context) override {
		debug("Scope apply called");
		context.addScope();
		_expr->apply(context);
		value_type res = context.getResult();
		context.leaveScope();
		return res;
	}
};

class NameToken : public INode {
	std::string _name;
public:
	NameToken(std::string name) : _name(name) {}

	virtual value_type apply(Context& context) override {
		return 0; 
	}	
	
	std::string getName() {
		return _name;
	}
};

class Function : public INode {
	INode* _expr;
	INode* _name;
	INode* _arguments;
public:
	Function(INode* expr, INode* name, INode* args) : _expr(expr), _name(name), _arguments(args) {}

	virtual value_type apply(Context& context) override {
		context.addFunction(((NameToken*)_name)->getName(), _expr, _arguments);
		return 0;
	}
};


class Return : public INode {
	INode* _expr;
public:
	Return(INode* expr) : _expr(expr) {}

	virtual value_type apply(Context& context) {
		value_type res = _expr->apply(context);
		context.setResult(res);
		return res;
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

class Mult : public INode {
	INode* _left_child, *_right_child;
public:
	Mult(INode* lc, INode* rc) : _left_child(lc), _right_child(rc) {}

	value_type apply(Context& context) override {
		return _left_child->apply(context) * _right_child->apply(context);
	}
};

class Tuple : public INode {
	INode* _current; INode* _next;
	size_t size;
public:
	Tuple(INode* c, INode* next) : _current(c), _next(next) {
		size = 1 + ((Tuple*)_next)->size;
	}

	value_type apply(Context& context) {
		return _current->apply(context);
	}

	INode* getNext() {
		return _next;
	}

	INode* getCurrent() {
		return _current;
	}
};


class FunctionCall : public INode { 
	INode* _arguments;
	INode* _name;
public:
	FunctionCall(INode* argsTuple, INode* name) : _arguments(argsTuple), _name(name) {}

	virtual value_type apply(Context& context) override {
		std::pair<INode*, INode*> funcInfo = context.getFunction(((NameToken*)_name)->getName());
		INode* expr = funcInfo.first;
		INode* argNames = funcInfo.second;
		
		context.addScope();
		Tuple* current = (Tuple*)_arguments;
		Tuple* currentName = (Tuple*)argNames;
		while(current != nullptr && currentName != nullptr) {
			NameToken* name = (NameToken*)currentName->getCurrent();
			value_type val = current->apply(context);
			context.get(name->getName()) = val;

			current = (Tuple*)current->getNext();
			currentName = (Tuple*)currentName->getNext();
		}
		if (current != nullptr) throw 1;
		if (currentName != nullptr) throw 1;

		value_type res = expr->apply(context);
		context.leaveScope();
		return res;
	}
};

class Splitted : public INode {
	INode* _left_child, *_right_child;
public:
	Splitted(INode* lc, INode* rc) : _left_child(lc), _right_child(rc) {
		debug("Splitted initialized");
	}

	virtual value_type apply(Context& context) override {
		debug("Splitted apply called");
		value_type res = _left_child->apply(context);
		_right_child->apply(context);
		return res;
	}
};

class Variable : public INode {
private:
	INode* _name;
public:
	Variable(INode* name) : _name(name) {}
	value_type& get(Context& context) {
		return context.get(((NameToken*)_name)->getName());
	}
	value_type apply(Context& context) {
		return context.get(((NameToken*)_name)->getName());
	}
};
class Assign : public INode {
private:
	INode* _right_child;
	Variable* _left_child;
public:
	Assign(Variable* lc, INode* rc) : _left_child(lc), _right_child(rc) {
		debug("Assign initialized");
	}

	virtual value_type apply(Context& context) override {
		debug("Assign apply called");
		return (_left_child->get(context) = _right_child->apply(context));
	}
};

class Log : public INode {
private:
	INode* _expr;
public:
	Log(INode* lc) : _expr(lc) {
		debug("Log initialized");
	}

	virtual value_type apply(Context& context) override {
		debug("Log apply called");
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
		debug("Value apply called " << _value);
		return _value;
	}
};
#endif

