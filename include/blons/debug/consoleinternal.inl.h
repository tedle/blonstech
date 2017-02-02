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
// Generates a std::function type to match a given lambda type
// Usage LambdaToFunction<decltype(lambda)>::Type func(lambda);
template <typename T>
struct LambdaToFunction { using Type = typename LambdaToFunction<decltype(&T::operator())>::Type; };

template <typename T, typename U, typename... Args>
struct LambdaToFunction<T(U::*)(Args...) const>
{
    using Type = std::function<void(Args...)>;
};
// TODO: Implement this if we can think of a concise way to do it...?
// template <typename T>
// struct BindToFunction { using Type = typename BindToFunction<T>::Type; };
// template <typename T, typename Func, typename... Args>
// struct BindToFunction<std::_Binder<T, Func, Args...>> {};

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
        return PackArgsAndCall(std::index_sequence_for<Args...>{}, runtime_args);
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
    Variable::Type ArgType<float>() { return Variable::FLOAT; }
    template<>
    Variable::Type ArgType<std::string>() { return Variable::STRING; }
    template<>
    Variable::Type ArgType<const char*>() { return Variable::STRING; }

    template<std::size_t... i>
    bool PackArgsAndCall(std::index_sequence<i...>, const std::vector<Variable>& runtime_args)
    {
        // Check we were passed the right amount of arguments
        if (runtime_args.size() != sizeof...(i))
        {
            return false;
        }
        try
        {
            // Calls the stored function with real arguments
            // Will throw if there's a type mismatch in supplied arguments
            func_(runtime_args[i].to<Args>()...);
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

// Registers a variable the console
const Variable* __registervariable(const std::string& name, const Variable& var);

// Retrieves a variable from the console
const Variable* __var(const std::string& name);

// Sets the value of an existing variable in the console
void __set_var(const std::string& name, const Variable& var);
} // namespace internal