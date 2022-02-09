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
	#define debug(x) std::cout << x << std::endl;
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
		debug("Function " << name << " is reset");
		functions[name] = std::make_pair(func, args);
	}

	std::pair<INode*, INode*> getFunction(std::string name) {
		if (functions.find(name) == functions.end()) {
			std::cout << "Error function not found!" << std::endl;
			throw 1;
		}
		if (functions[name].second == nullptr) {
			debug("second is null");
		}
		if (functions[name].first == nullptr) {
			debug("first is null");
		}
		debug("function received");
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

class Minus : public INode {
	INode* _left_child, *_right_child;
public:
	Minus(INode* lc, INode* rc) : _left_child(lc), _right_child(rc) {
		debug("minus initialized");
	}

	value_type apply(Context& context) override {
		return _left_child->apply(context) - _right_child->apply(context);
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
		int nextSize = 0;
		if (_next != nullptr) {
			nextSize = ((Tuple*)_next)->size;
		}
		size = 1 + nextSize;
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
		debug("Function name: "  << ((NameToken*)_name)->getName());
		std::pair<INode*, INode*> funcInfo = context.getFunction(((NameToken*)_name)->getName());
		INode* expr = funcInfo.first;
		INode* argNames = funcInfo.second;
		if (argNames == nullptr) {
			debug("argNames is null");
		}	
		context.addScope();
		Tuple* current = (Tuple*)_arguments;
		Tuple* currentName = (Tuple*)argNames;
		int steps = 0;
		while(current != nullptr && currentName != nullptr) {
			debug("argument_parsing in process");
			NameToken* name = (NameToken*)currentName->getCurrent();
			debug(name->getName());
			value_type val = current->apply(context);
			context.get(name->getName()) = val;

			current = (Tuple*)current->getNext();
			currentName = (Tuple*)currentName->getNext();
			steps++;
		}

		if (current != nullptr) {
			std::cout << "Args successfully parsed: " << steps << std::endl;
			std::cout << "currentName is null!" << std::endl;
			std::cout << current->apply(context) << std::endl;
			throw 1;
		}
		if (currentName != nullptr) {
			std::cout << "current is null!" << std::endl;
			throw 1;
		}

		value_type res = expr->apply(context);
		context.leaveScope();
		return res;
	}
};

class ConditionalBehaviour : public INode  {
	INode* _condition;
	INode* _expression;
	public:
	ConditionalBehaviour(INode* condition, INode* expression) : _condition(condition), _expression(expression) {
		debug("IF initialized");
	}

	value_type apply(Context& context) override {
		debug("IF called");
		value_type condition_result = _condition->apply(context);
		if ((bool)condition_result) {
			value_type res = _expression->apply(context);
			return res;
		}
		return 0;
	}


};

class Equals : public INode {
	INode* _left_child;
	INode* _right_child;

	public:
	Equals(INode* left_child, INode* right_child) : _left_child(left_child), _right_child(right_child) {
		debug("equals initialized");
	}

	value_type apply(Context& context) override {
		debug("equals called");
		value_type left = _left_child->apply(context);
		value_type right = _right_child->apply(context);
		return (left == right ? 1 : 0);
	}
};

enum BooleanOpType {
	_and, _or, _xor
};

class BooleanOp : public INode {
	INode* _left_child, *_right_child;
	BooleanOpType _t;

	public:
	BooleanOp(INode* left_child, INode* right_child, BooleanOpType t) : _left_child(left_child), _right_child(right_child), _t(t) {
		debug("boolean op initialized");
	}

	value_type apply(Context& context) override {
		value_type left = _left_child->apply(context);
		value_type right = _right_child->apply(context);
		bool res;
		switch(_t) {
			case _and: {
				res = (left != 0) && (right != 0);
				break;
				  };
			case _or: {
				res = (left != 0) || (right != 0);
				break;
				 };
			case _xor: {
				res = (left != 0) ^ (right != 0);
				break;
				  };
		}

		return (res ? 1 : 0);
	}
};

class InversedBoolean : public INode {
	INode* _child;

	public:
	InversedBoolean(INode* child) : _child(child) {
		debug("NOT initialized");
	}

	value_type apply(Context& context) override {
		value_type child_value = _child->apply(context);
		return (child_value == 0) ? 1 : 0;
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
	Variable* _left_child;
	INode* _right_child;

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
		return _value;
	}
};
#endif
