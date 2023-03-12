#include "json_builder.h"
#include <variant>
#include <stack>

using namespace std::literals;

namespace json {
    Builder::Builder() {}

    KeyItemContext Builder::Key(std::string str) {
        bool empty = curr_container_node_.empty();
        if (empty) { throw std::logic_error("Method Key can't be called"); }
        if (curr_container_node_.back()->IsMap() && !key_flag_) {
            nodes_stack_.push_back(new Node(str));
            key_flag_ = true;
        } else {
            throw std::logic_error("Method Key can't be called");
        }

        return  { *this };
    }

    Builder& Builder::Value(Node::Value value) {
        if (nodes_stack_.empty() || value_flag_) {
            if (value_flag_) {
                throw std::logic_error("Method Value can't be called");
            }
            value_flag_ = true;
        } else if (curr_container_node_.back()->IsMap() && key_flag_) {
            key_flag_ = false;
        } else if (curr_container_node_.back()->IsArray() ) {

        } else {
            throw std::logic_error("Method Value can't be called");
        }

        nodes_stack_.push_back(new Node(std::move(value)));
        
        return *this;

    }

    ArrayItemContext Builder::StartArray() {
        if (key_flag_) {
            key_flag_ = false;
        }
        nodes_stack_.push_back(new Node(json::Array()));
        curr_container_node_.push_back(nodes_stack_.back());
        
        return *this;
    }

    DictItemContext Builder::StartDict() {
        if (key_flag_) {
            key_flag_ = false;
        }
        nodes_stack_.push_back(new Node(json::Dict()));
        curr_container_node_.push_back(nodes_stack_.back());

        return *this;
    }

    Node Builder::Build() {
		if (nodes_stack_.size() > 1 || (nodes_stack_.size() == 0)) {
			throw std::logic_error("JSON document was not completed"s);
		} else {
            root_ = std::move(*nodes_stack_.back());
			nodes_stack_.pop_back();
		}
		return root_;
	}

    Builder& Builder::EndArray() {
        json::Array result;
        std::stack<Node*> buff;
        bool empty = curr_container_node_.empty();
        if (empty) { throw std::logic_error("Method Key can't be called"); }

        if (curr_container_node_.back()->IsArray()) {
            while (nodes_stack_.back() != curr_container_node_.back()) {
                json::Node* node = nodes_stack_.back();
                nodes_stack_.pop_back();

                buff.push(node);
            }

            while (!buff.empty()) {
                result.push_back(std::move(*buff.top()));
                buff.pop();
            }

            *nodes_stack_.back() = std::move(result);
            curr_container_node_.pop_back();
        } else {
            throw std::logic_error("No arr in the order");
        }
        
        return *this;
    }

    Builder& Builder::EndDict() {
        json::Dict result;

        bool empty = curr_container_node_.empty();
        if (empty) { throw std::logic_error("Method Key can't be called"); }

        if (curr_container_node_.back()->IsMap()) {
            while (nodes_stack_.back() != curr_container_node_.back()) {
                json::Node* node = nodes_stack_.back();
                nodes_stack_.pop_back();

                std::string str = (*nodes_stack_.back()).AsString();
                result[str] = std::move(*node);
                nodes_stack_.pop_back();
            }

            *nodes_stack_.back() = std::move(result);
            curr_container_node_.pop_back();
        } else {
            throw std::logic_error("No dict in the order");
        }    
        return *this;
    }


    // ----------


    //---------------- DictItemContext ----------------
	DictItemContext::DictItemContext(Builder& builder) : builder_(builder) {}

	Builder& DictItemContext::EndDict() {
		return builder_.EndDict();
	}

	KeyItemContext DictItemContext::Key(std::string key) {
		return builder_.Key(move(key));
	}
	//---------------- ArrayItemContext ----------------
	ArrayItemContext::ArrayItemContext(Builder& builder) : builder_(builder) {}

	DictItemContext ArrayItemContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext ArrayItemContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& ArrayItemContext::EndArray() {
		return builder_.EndArray();
	}

	ArrValueItemContext ArrayItemContext::Value(Node::Value value) {
		return builder_.Value(move(value));
	}
	//---------------- KeyItemContext ----------------
	KeyItemContext::KeyItemContext(Builder& builder) : builder_(builder) {}

	DictItemContext KeyItemContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext KeyItemContext::StartArray() {
		return builder_.StartArray();
	}

	ValueItemContext KeyItemContext::Value(Node::Value value) {
		return builder_.Value(move(value));
	}
	//---------------- ValueItemContext ----------------
	ValueItemContext::ValueItemContext(Builder& builder) : builder_(builder) {}

	KeyItemContext ValueItemContext::Key(std::string key) {
		return builder_.Key(move(key));
	}

	Builder& ValueItemContext::EndDict() {
		return builder_.EndDict();
	}
	//---------------- ArrValueItemContext ----------------
	ArrValueItemContext::ArrValueItemContext(Builder& builder) : builder_(builder) {}

	ArrValueItemContext ArrValueItemContext::Value(Node::Value value) {
		return builder_.Value(move(value));
	}

	DictItemContext ArrValueItemContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext ArrValueItemContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& ArrValueItemContext::EndArray() {
		return builder_.EndArray();
	}
}