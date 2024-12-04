#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include<deque>

struct Operator {
	uint8_t precendence = 0;	// Precendece of the operator (higher means greater precendece)
	uint8_t arguments = 0;		// Number of arguments the operator takes (2=binary, 1=unary, etc)
};

struct Symbol {
	std::string symbol = "";

	enum class Type : uint8_t {
		Unknown,
		LiteralNumeric,
		Operator,
		ParenOpen,
		ParenClose,
	} type = Type::Unknown;

	Operator op;
};

std::unordered_map<char, Operator> generateOperatorMap() {
	std::unordered_map<char, Operator> map;
	map['/'] = { 3, 2 };
	map['*'] = { 3, 2 };
	map['-'] = { 1, 2 };
	map['+'] = { 1, 2 };
	return map;
}

int solver(std::deque<Symbol>& stackOutput) {
	std::deque<double> stackSolve;

	for (const auto& inst : stackOutput) {
		switch (inst.type) {
		case Symbol::Type::LiteralNumeric: {
			stackSolve.push_front(std::stod(inst.symbol));
		}
		break;

		case Symbol::Type::Operator: {
			std::vector<double> mem(inst.op.arguments);

			for (uint8_t a = 0; a < inst.op.arguments; a++) {
				if (stackSolve.empty()) {
					std::cerr << "Operator requires argument" << std::endl;
				}
				else {
					mem[a] = stackSolve[0];
					stackSolve.pop_front();
				}
			}

			double result = 0.0;
			if (inst.op.arguments == 2) {
				if (inst.symbol[0] == '/') result = mem[1] / mem[0];
				if (inst.symbol[0] == '*') result = mem[1] * mem[0];
				if (inst.symbol[0] == '-') result = mem[1] - mem[0];
				if (inst.symbol[0] == '+') result = mem[1] + mem[0];
			}else if (inst.op.arguments == 1) {
				if (inst.symbol[0] == '*') result = +mem[0];
				if (inst.symbol[0] == '-') result = -mem[0];
			}

			stackSolve.push_front(result);
		}
		break;
		}
	}

	std::cout << std::to_string(stackSolve[0]) << std::endl;
	return 0;
}

int tokenize(std::string& source) {
	auto operatorMap = generateOperatorMap();
	std::deque<Symbol> stackHolding;
	std::deque<Symbol> stackOutput;

	Symbol lastSymbol = { "0", Symbol::Type::LiteralNumeric, 0, 0 };
	int pass = 0;

	for (const char c : source) {
		if (std::isdigit(c)) {
			stackOutput.push_back({ std::string(1, c), Symbol::Type::LiteralNumeric });
			lastSymbol = stackOutput.back();
		}
		else if (c == '(') {
			stackHolding.push_front({ std::string(1, c), Symbol::Type::ParenOpen });
			lastSymbol = stackOutput.back();
		}
		else if (c == ')') {
			// Make sure the stack isn't empty while draining
			while (!stackHolding.empty() && stackHolding.front().type != Symbol::Type::ParenOpen) {
				stackOutput.push_back(stackHolding.front());
				stackHolding.pop_front();
			}

			if (stackHolding.empty()) {
				std::cerr << "Mismatched parentheses" << std::endl;
				return -1;
			}

			// Remove the open parentheses from the holding stack after draining
			if (!stackHolding.empty() && stackHolding.front().type == Symbol::Type::ParenOpen) {
				stackHolding.pop_front();
			}

			lastSymbol = { ")", Symbol::Type::ParenClose };

		} else if (operatorMap.contains(c)) {
			// Get the op (should be of type Operator)
			Operator op = operatorMap[c];

			if (c == '-' || c == '+') {
				if ((lastSymbol.type != Symbol::Type::LiteralNumeric) && (lastSymbol.type != Symbol::Type::ParenClose) || pass == 0) {
					op.arguments = 1;
					op.precendence = 100;
				}
			}


			while (!stackHolding.empty() && stackHolding.front().type != Symbol::Type::ParenOpen) {
				// Find something on the stack that is an operator
				if (stackHolding.front().type == Symbol::Type::Operator) {
					const auto& holdingOp = stackHolding.front().op;

					// If the current op is equal to or less than the current one on the stack,
					// push the holding op onto the output and put our new op onto the stack
					if (holdingOp.precendence >= op.precendence) {
						stackOutput.push_back(stackHolding.front());
						stackHolding.pop_front();
					}
					else {
						break;
					}
				}
			}

			// Push the new operator onto the TOP of holding stack
			stackHolding.push_front({ std::string(1, c), Symbol::Type::Operator, op });
			lastSymbol = stackHolding.front();
		}
		else {
			std::cerr << "Bad Token: " << std::string(1, c) << std::endl;
			return -1;
		}
		pass++;
	}

	while (!stackHolding.empty()) {
		stackOutput.push_back(stackHolding.front());
		stackHolding.pop_front();
	}

	std::cout << "Output after algo\n" << std::endl;
	for (const auto& s : stackOutput) {
		std::cout << s.symbol;
	}
	std::cout << std::endl;

	return solver(stackOutput);
}

int main() {
	std::string source = "-1+2*-4-3";

	return tokenize(source);
}