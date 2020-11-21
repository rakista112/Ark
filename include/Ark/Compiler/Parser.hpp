/**
 * @file Parser.hpp
 * @author Alexandre Plateau (lexplt.dev@gmail.com)
 * @brief Parses a token stream into an AST by using the Ark::internal::Node
 * @version 0.1
 * @date 2020-10-27
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef ark_parser
#define ark_parser

#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <utility>
#include <cinttypes>
#include <sstream>

#include <Ark/Exceptions.hpp>
#include <Ark/Compiler/Lexer.hpp>
#include <Ark/Compiler/Node.hpp>

namespace Ark
{
    /**
     * @brief The parser is responsible of constructing the Abstract Syntax Tree from a token list
     * 
     */
    class Parser
    {
    public:
        /**
         * @brief Construct a new Parser object
         * 
         * @param debug the debug level
         * @param lib_dir the path to the standard library
         * @param options the parsing options
         */
        Parser(unsigned debug, const std::string& lib_dir, uint16_t options) noexcept;

        /**
         * @brief Give the code to parse
         * 
         * @param code the ArkScript code
         * @param filename the name of the file
         */
        void feed(const std::string& code, const std::string& filename=ARK_NO_NAME_FILE);

        /**
         * @brief Return the generated AST
         * 
         * @return const internal::Node& 
         */
        const internal::Node& ast() const noexcept;

        /**
         * @brief Return the list of files imported by the code given to the parser
         * 
         * Each path of each imported file is relative to the filename given when feeding the parser.
         * 
         * @return const std::vector<std::string>& 
         */
        const std::vector<std::string>& getImports() const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const Parser& P) noexcept;

    private:
        unsigned m_debug;
        std::string m_libdir;
        uint16_t m_options;
        internal::Lexer m_lexer;
        internal::Node m_ast;
        internal::Token m_last_token;

        // path of the current file
        std::string m_file;
        // source code of the current file
        std::string m_code;
        // the files included by the "includer" to avoid multiple includes
        std::vector<std::string> m_parent_include;

        /**
         * @brief Applying syntactic sugar: {...} => (begin...), [...] => (list ...)
         * 
         * @param tokens a list of tokens
         */
        void sugar(std::vector<internal::Token>& tokens) noexcept;

        /**
         * @brief Parse a list of tokens recursively
         * 
         * @param tokens 
         * @param authorize_capture if we are authorized to consume TokenType::Capture tokens
         * @param authorize_field_read if we are authorized to consume TokenType::GetField tokens
         * @return internal::Node 
         */
        internal::Node parse(std::list<internal::Token>& tokens, bool authorize_capture=false, bool authorize_field_read=false);

        /**
         * @brief Get the next token if possible, from a list of tokens
         * 
         * The list of tokens is modified.
         * 
         * @param tokens list of tokens to get the next token from
         * @return internal::Token 
         */
        internal::Token nextToken(std::list<internal::Token>& tokens);

        /**
         * @brief Convert a token to a node
         * 
         * @param token the token to converts
         * @return internal::Node 
         */
        internal::Node atom(const internal::Token& token);

        /**
         * @brief Search for all the includes in a given node, in its sub-nodes and replace them by the code of the included file
         * 
         * @param n 
         * @return true returned on success
         * @return false returned on failure
         */
        bool checkForInclude(internal::Node& n);

        // error management functions
        inline void expect(bool pred, const std::string& message, internal::Token token);
        inline void throwParseError(const std::string& message, internal::Token token);
        inline void throwParseError_(const std::string& message);
    };

    #include "Parser.inl"
}

#endif  // ark_parser
