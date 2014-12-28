////////////////////////////////////////////////////////////////////////////////
// blonstech
// Copyright(c) 2014 Dominic Bowden
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

namespace internal
{
// Builds a list of indices during compilation
/// @cond Doxygen_Suppress
// ^^^ Why am I even getting doxygen warnings?? These are excluded symbols, go away
template<unsigned... i> struct Indices { typedef Indices type; };
template<unsigned size, unsigned... i> struct MakeIndices : MakeIndices<size - 1, size - 1, i...> {};
template<unsigned... i> struct MakeIndices<0, i...> : Indices<i...> {};
template<unsigned size> using MakeIndicesType = typename MakeIndices<size>::type;
/// @endcond

// Base class used to store functions in the console
class Function
{
public:
    // Turns the list of console::Variable into real arguments and calls the function
    virtual bool Run(const std::vector<Variable>& args)=0;
    // Returns a ordered list of expected inputs
    virtual std::vector<Variable::Type> ArgList()=0;
};

class VoidFunction : public Function
{
public:
    VoidFunction(std::function<void()> func) : func_(func) {}

    bool Run(const std::vector<Variable>& args)
    {
        if (args.size() != 0)
        {
            return false;
        }
        func_();
        return true;
    }
    std::vector<Variable::Type> ArgList()
    {
        return {};
    }
private:
    std::function<void()> func_;
};

// Generates one class for each unique function prototype registered to the console
template <typename... Args>
class TemplatedFunction : public Function
{
public:
    TemplatedFunction(std::function<void(Args...)> func) : func_(func) {}

    bool Run(const std::vector<Variable>& runtime_args)
    {
        return PackArgsAndCall(MakeIndicesType<sizeof...(Args)>{}, runtime_args);
    }

    std::vector<Variable::Type> ArgList()
    {
        // TODO: Fix this when func type is void()
        return { ArgType<Args>()... };
    }

private:
    std::function<void(Args...)> func_;

    template<typename T>
    Variable::Type ArgType()
    {
        static_assert(false, "Unsupported argument type in console function");
    }

    template<>
    Variable::Type ArgType<int>() { return Variable::INT; }
    template<>
    Variable::Type ArgType<std::string>() { return Variable::STRING; }
    template<>
    Variable::Type ArgType<const char*>() { return Variable::STRING; }

    template<unsigned... i>
    bool PackArgsAndCall(Indices<i...>, const std::vector<Variable>& runtime_args)
    {
        // Check we were passed the right amount of arguments
        if (runtime_args.size() != sizeof...(i))
        {
            return false;
        }
        try
        {
            // This will throw if there's a type mismatch in supplied arguments
            auto arg_tuple = std::make_tuple(runtime_args[i].to<Args>()...);
            // Calls the stored function with real arguments
            func_(std::get<i>(arg_tuple)...);
        }
        catch (const char*)
        {
            return false;
        }
        return true;
    }
};

// Registers a function the console
void __registerfunction(const std::string& name, Function* func);
} // namespace internal