#pragma once

#include "json.h"

namespace json {

    class DictItemContext;
	class ArrayItemContext;
	class ValueItemContext;
	class KeyItemContext;
	class ArrValueItemContext;

    class Builder {
    public:
        Builder();

        KeyItemContext Key(std::string str);
        Builder& Value(Node::Value value);

        DictItemContext StartDict();
        ArrayItemContext StartArray();

        Node Build();

        Builder& EndDict();
        Builder& EndArray();
    
    private:
        Node root_;
        bool key_flag_ = false;
        bool value_flag_ = false;
        std::vector<Node*> nodes_stack_;
        std::vector<Node*> curr_container_node_;
    };


    class DictItemContext : private Builder {
	public:
		DictItemContext(Builder& builder);
		KeyItemContext Key(std::string key);
		Builder& EndDict();
	private:
		Builder& builder_;
	};

	class ArrayItemContext : private Builder {
	public:
		ArrayItemContext(Builder& builder);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndArray();
		ArrValueItemContext Value(Node::Value value);

	private:
		Builder& builder_;
	};

	class KeyItemContext : private Builder {
	public:
		KeyItemContext(Builder& builder);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		ValueItemContext Value(Node::Value value);
	private:
		Builder& builder_;
	};

	class ValueItemContext : private Builder {
	public:
		ValueItemContext(Builder& builder);
		KeyItemContext Key(std::string key);
		Builder& EndDict();

		ArrayItemContext StartArray() = delete;
	private:
		Builder& builder_;
	};

	class ArrValueItemContext : private Builder {
	public:
		ArrValueItemContext(Builder& builder);
		ArrValueItemContext Value(Node::Value value);
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		Builder& EndArray();

	private:
		Builder& builder_;
	};
}