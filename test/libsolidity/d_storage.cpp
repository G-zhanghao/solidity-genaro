/*
	This file is part of solidity.

*/

#include <string>

#include <libsolidity/parsing/Scanner.h>
#include <libsolidity/parsing/Parser.h>
#include <libsolidity/analysis/NameAndTypeResolver.h>
#include <libevmasm/Assembly.h>
#include <libsolidity/codegen/CompilerContext.h>
#include <libsolidity/codegen/Compiler.h>
#include <libsolidity/codegen/ExpressionCompiler.h>
#include <libsolidity/ast/AST.h>
#include <libsolidity/analysis/TypeChecker.h>
#include <libsolidity/interface/ErrorReporter.h>
#include <test/Options.h>


using namespace std;
using namespace dev::eth;

namespace dev
{
namespace solidity
{
namespace test
{
/*
class FirstFunctionCallExtractor: private ASTVisitor
{
    public:
            FirstFunctionCallExtractor(ASTNode& _node): m_expression(nullptr) { _node.accept(*this);  }
            Expression* expression() const { return m_expression;  }
    private:
            virtual bool visit(FunctionCall& _expression) override { return checkExpression(_expression);  }
            bool checkExpression(Expression& _expression)
            {
                if (m_expression == nullptr)
                    m_expression = &_expression;
                return false;
            }
    private:
            Expression* m_expression;

};
*/

bytes compileTestContract(const string& _sourceCode)
{
    ASTPointer<SourceUnit> sourceUnit;
    try{
        ErrorList errors;
        ErrorReporter errorReporter(errors);
        sourceUnit  = Parser(errorReporter).parse(make_shared<Scanner>(CharStream(_sourceCode)));
        if (!sourceUnit)
            return bytes();
    }
    catch(boost::exception const& _e)
    {
        auto msg = std::string("Parsing source code failed with:")+boost::diagnostic_information(_e);

        BOOST_FAIL(msg);
    }
   
    //code generation
    ErrorList errors;
    ErrorReporter errorReporter(errors);
	vector<Declaration const*> declarations;
    map<ASTNode const*, shared_ptr<DeclarationContainer>> scopes;
    NameAndTypeResolver resolver(declarations, scopes, errorReporter);
    solAssert(Error::containsOnlyWarnings(errorReporter.errors()), "");
    resolver.registerDeclarations(*sourceUnit);
    
    for (ASTPointer<ASTNode> const& node: sourceUnit->nodes())
        if (ContractDefinition* contract = dynamic_cast<ContractDefinition*>(node.get()))
        {
            BOOST_REQUIRE_NO_THROW(resolver.resolveNamesAndTypes(*contract));
            if (!Error::containsOnlyWarnings(errorReporter.errors()))
                return bytes();
        }
    for (ASTPointer<ASTNode> const& node: sourceUnit->nodes())
        if (ContractDefinition* contract = dynamic_cast<ContractDefinition*>(node.get()))
        {
            TypeChecker checker(dev::test::Options::get().evmVersion(), errorReporter);
            BOOST_REQUIRE_NO_THROW(checker.checkTypeRequirements(*contract));
            if (!Error::containsOnlyWarnings(errorReporter.errors()))
                return bytes();
                                                                                
        }
    for (ASTPointer<ASTNode> const& node: sourceUnit->nodes())
        if (ContractDefinition* contract = dynamic_cast<ContractDefinition*>(node.get()))
        {
            Compiler compiler(dev::test::Options::get().evmVersion());
            compiler.compileContract(*contract, map<ContractDefinition const*, Assembly const*>{}, bytes());
            return compiler.runtimeObject().bytecode;
        }
    BOOST_FAIL("No contract found in source.");
    return bytes();
}
    
BOOST_AUTO_TEST_SUITE(d_storage)

BOOST_AUTO_TEST_CASE(call_check_theft)
{
    std::string code = R"(
        contract d_storage_test {
            function Test() public view{
                d_storage d = 0x123;
                d.check_sentinel();
            }
        }
	)";

    bytes res = compileTestContract(code);
    
	bytes expectation({0x16, 0x90, 0x50, 0x80, (int)Instruction::SENTINEL_HEFT, 0x50, 0x50, 0x56});
    bytes slice(expectation.size());
    copy(res.rbegin(), res.rbegin()+expectation.size(), slice.rbegin());
    //debug 
    //copy (slice.begin(), slice.end(), ostream_iterator<int> (cout, ":"));
	BOOST_CHECK_EQUAL_COLLECTIONS(slice.begin(), slice.end(), expectation.begin(), expectation.end());
}

BOOST_AUTO_TEST_CASE(call_update)
{
    std::string code = R"(
        contract d_storage_test {
            function Test() public view{
                d_storage d = 0x123;
                d.update(0x1);
            }
        }
	)";

    bytes res = compileTestContract(code);
    
	bytes expectation({0x16, 0x90, 0x50, 0x80, 0x60, 0x01, (int)Instruction::DATAVERSIONUPDATE, 0x50, 0x50, 0x56});
    bytes slice(expectation.size());
    copy(res.rbegin(), res.rbegin()+expectation.size(), slice.rbegin());
    //copy (slice.begin(), slice.end(), ostream_iterator<int> (cout, ":"));
	BOOST_CHECK_EQUAL_COLLECTIONS(slice.begin(), slice.end(), expectation.begin(), expectation.end());
}
BOOST_AUTO_TEST_CASE(call_sgas)
{
    std::string code = R"(
        contract d_storage_test {
            function Test() public view{
                d_storage d = 0x123;
                d.sgas(0x1);
            }
        }
	)";

    bytes res = compileTestContract(code);
    
	bytes expectation({0x16, 0x90, 0x50, 0x80, 0x60, 0x01, (int)Instruction::STORAGE_GAS, 0x50, 0x50, 0x56});
    bytes slice(expectation.size());
    copy(res.rbegin(), res.rbegin()+expectation.size(), slice.rbegin());
    //copy (slice.begin(), slice.end(), ostream_iterator<int> (cout, ":"));
	BOOST_CHECK_EQUAL_COLLECTIONS(slice.begin(), slice.end(), expectation.begin(), expectation.end());
}
BOOST_AUTO_TEST_CASE(call_slice)
{
    std::string code = R"(
        contract d_storage_test {
            function Test() public view{
                d_storage d = 0x123;
                d.slice(0x1);
            }
        }
	)";

    bytes res = compileTestContract(code);
    
	bytes expectation({0x16, 0x90, 0x50, 0x80, 0x60, 0x01, (int)Instruction::STORAGE_GAS_PRICE, 0x50, 0x50, 0x56});
    bytes slice(expectation.size());
    copy(res.rbegin(), res.rbegin()+expectation.size(), slice.rbegin());
    //copy (slice.begin(), slice.end(), ostream_iterator<int> (cout, ":"));
	BOOST_CHECK_EQUAL_COLLECTIONS(slice.begin(), slice.end(), expectation.begin(), expectation.end());
}

BOOST_AUTO_TEST_CASE(call_sused)
{
    std::string code = R"(
        contract d_storage_test {
            function Test() public view{
                d_storage d = 0x123;
                d.sused(0x1);
            }
        }
	)";

    bytes res = compileTestContract(code);
    
	bytes expectation({0x16, 0x90, 0x50, 0x80, 0x60, 0x01, (int)Instruction::STORAGE_GAS_USED, 0x50, 0x50, 0x56});
    bytes slice(expectation.size());
    copy(res.rbegin(), res.rbegin()+expectation.size(), slice.rbegin());
    //copy (slice.begin(), slice.end(), ostream_iterator<int> (cout, ":"));
	BOOST_CHECK_EQUAL_COLLECTIONS(slice.begin(), slice.end(), expectation.begin(), expectation.end());
}

BOOST_AUTO_TEST_CASE(call_ssize)
{
    std::string code = R"(
        contract d_storage_test {
            function Test() public view{
                d_storage d = 0x123;
                d.ssize(0x1);
            }
        }
	)";

    bytes res = compileTestContract(code);
    
	bytes expectation({0x16, 0x90, 0x50, 0x80, 0x60, 0x01, (int)Instruction::SSIZE, 0x50, 0x50, 0x56});
    bytes slice(expectation.size());
    copy(res.rbegin(), res.rbegin()+expectation.size(), slice.rbegin());
    //copy (slice.begin(), slice.end(), ostream_iterator<int> (cout, ":"));
	BOOST_CHECK_EQUAL_COLLECTIONS(slice.begin(), slice.end(), expectation.begin(), expectation.end());
}

BOOST_AUTO_TEST_CASE(call_read_version)
{
    std::string code = R"(
        contract d_storage_test {
            function Test() public view{
                uint[16] memory a;
                uint64 res;
                d_storage d = 0x123;
                res = a.read_version(d, 0x2, 0x3);
            }
        }
	)";

    bytes res = compileTestContract(code);
    
    auto it = find(res.begin(), res.end(), (byte)Instruction::DATAVERSIONREAD);
    BOOST_CHECK_EQUAL(*it, (byte)Instruction::DATAVERSIONREAD);
	bytes expectation({0x60, 0x02, 0x60, 0x03, (int)Instruction::DATAVERSIONREAD, 0x91, 0x50, 0x50, 0x50, 0x50, 0x56});
    bytes slice(expectation.size());
    copy(it-4, it+7, slice.begin());
    //cout<<hex; copy (slice.begin(), slice.end(), ostream_iterator<int> (cout, ":"));
    BOOST_CHECK_EQUAL_COLLECTIONS(slice.begin(), slice.end(), expectation.begin(), expectation.end());
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
