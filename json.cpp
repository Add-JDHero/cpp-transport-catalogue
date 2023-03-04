#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        std::string LoadLiteral(std::istream& input) {
            std::string result_literal;
            while (std::isalpha(input.peek())) {
                result_literal.push_back(static_cast<char>(input.get()));
            }
            return result_literal;
        }

        Node LoadBool(std::istream& input) {
            const std::string literal = LoadLiteral(input);
            if (literal == "true"sv) {
                return Node{true};
            } else if (literal == "false"sv) {
                return Node{false};
            } else {
                throw ParsingError("Failed to parse '"s + literal + "' as bool"s);
            }
        }

        Node LoadNull(std::istream& input) {
            if (std::string literal = LoadLiteral(input); literal == "null"sv) {
                return Node{nullptr};
            } else {
                throw ParsingError("Failed to parse '"s + literal + "' as null"s);
            }
        }

        Node LoadArray(istream& input) {
            Array result;
            char c;
            for (; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (c != ']') {
                throw ParsingError("Failed to parse array node");
            }

            return Node(move(result));
        }

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            } else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return Node(std::stoi(parsed_num));
                    } catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return Node(std::stod(parsed_num));
            } catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                } else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                        case 'n':
                            s.push_back('\n');
                            break;
                        case 't':
                            s.push_back('\t');
                            break;
                        case 'r':
                            s.push_back('\r');
                            break;
                        case '"':
                            s.push_back('"');
                            break;
                        case '\\':
                            s.push_back('\\');
                            break;
                        default:
                            // Встретили неизвестную escape-последовательность
                            throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                } else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                } else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return Node{s};
        }

        Node LoadDict(istream& input) {
            Dict result;

            char c;
            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({move(key), LoadNode(input)});
            }
            if (c != '}') {
                throw ParsingError("Failed to parse dict node");
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError("Unexpected EOF"s);
            }
            switch (c) {
                case '[':
                    return LoadArray(input);
                case '{':
                    return LoadDict(input);
                case '"':
                    return LoadString(input);
                case 't':
                    [[fallthrough]];
                case 'f':
                    input.putback(c);
                    return LoadBool(input);
                case 'n':
                    input.putback(c);
                    return LoadNull(input);
                default:
                    input.putback(c);
                    return LoadNumber(input);
            }
        }

        struct PrintContext {
            std::ostream &out;
            int indent_step = 4;
            int indent = 0;

            void PrintIndent() const {
                for (int i = 0; i < indent; ++i) {
                    out.put(' ');
                }
            }

            PrintContext Indented() const {
                return {out, indent_step, indent_step + indent};
            }
        };

        void PrintNode(const Node &value, const PrintContext &ctx);

        template<typename Value>
        void PrintValue(const Value &value, const PrintContext &ctx) {
            ctx.out << value;
        }

        void PrintString(const std::string &value, std::ostream &out) {
            out.put('"');
            for (const char character: value) {
                switch (character) {
                    case '\r':
                        out << "\\r"sv;
                        break;
                    case '\n':
                        out << "\\n"sv;
                        break;
                    case '"':
                        // Символы " и \ выводятся как \" или \\, соответственно
                        [[fallthrough]];
                    case '\\':
                        out.put('\\');
                        [[fallthrough]];
                    default:
                        out.put(character);
                        break;
                }
            }
            out.put('"');
        }

        template<>
        void PrintValue<std::string>(const std::string &value, const PrintContext &ctx) {
            PrintString(value, ctx.out);
        }

        template<>
        void PrintValue<std::nullptr_t>(const std::nullptr_t &, const PrintContext &ctx) {
            ctx.out << "null"sv;
        }

        // В специализаци шаблона PrintValue для типа bool параметр value передаётся
        // по константной ссылке, как и в основном шаблоне.
        // В качестве альтернативы можно использовать перегрузку:
        // void PrintValue(bool value, const PrintContext& ctx);
        template<>
        void PrintValue<bool>(const bool &value, const PrintContext &ctx) {
            ctx.out << (value ? "true"sv : "false"sv);
        }

        template<>
        void PrintValue<Array>(const Array& nodes, const PrintContext &ctx) {
            std::ostream &out = ctx.out;
            out << "[\n"sv;
            bool first = true;
            auto inner_ctx = ctx.Indented();
            for (const Node &node: nodes) {
                if (first) {
                    first = false;
                } else {
                    out << ",\n"sv;
                }
                inner_ctx.PrintIndent();
                PrintNode(node, inner_ctx);
            }
            out.put('\n');
            ctx.PrintIndent();
            out.put(']');
        }

        template<>
        void PrintValue<Dict>(const Dict& nodes, const PrintContext &ctx) {
            std::ostream &out = ctx.out;
            out << "{\n"sv;
            bool first = true;
            auto inner_ctx = ctx.Indented();
            for (const auto &[key, node]: nodes) {
                if (first) {
                    first = false;
                } else {
                    out << ",\n"sv;
                }
                inner_ctx.PrintIndent();
                PrintString(key, ctx.out);
                out << ": "sv;
                PrintNode(node, inner_ctx);
            }
            out.put('\n');
            ctx.PrintIndent();
            out.put('}');
        }

        void PrintNode(const Node& node, const PrintContext &ctx) {
            std::visit(
                    [&ctx](const auto &value) {
                        PrintValue(value, ctx);
                    },
                    node.GetValue());
        }

    } // namespace


    bool Node::IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const {
        return IsPureDouble() || IsInt();
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("not an int"s);
        } else {
            return std::get<int>(*this);
        }
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("not a bool"s);
        } else {
            return std::get<bool>(*this);
        }
    }

    double Node::AsDouble() const {
        if (!IsDouble()) {
            throw std::logic_error("not a double"s);
        } else {
            return IsPureDouble() ? std::get<double>(*this) : AsInt();
        }
    }

    const std::string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("not a string"s);
        } else {
            return std::get<std::string>(*this);
        }
    }

    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("not an array"s);
        } else {
            return std::get<Array>(*this);
        }
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("not a map"s);
        } else {
            return std::get<Dict>(*this);
        }
    }

    bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }

    bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    //  ---------- Document ----------

    Document::Document(Node root)
            : root_(std::move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), PrintContext{ output });
    }
    
    bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

}  // namespace json