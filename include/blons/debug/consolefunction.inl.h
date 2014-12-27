////////////////////////////////////////////////////////////////////////////////
/// blonstech
/// Copyright(c) 2014 Dominic Bowden
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files(the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

namespace internal
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains an immutable typed console variable
///
/// Can easily convert to native types through the templated Variable::to()
/// function. Will throw if a type mismatch occurs. See blons::console for valid
/// types.
////////////////////////////////////////////////////////////////////////////////
struct Variable
{
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Possible types of variables
    ////////////////////////////////////////////////////////////////////////////////
    enum Type
    {
        NONE,     ///< Unknown or null
        FUNCTION, ///< Callable C function
        STRING,   ///< String
        INT,      ///< Signed integer
        FLOAT     ///< Signed floating point
    };

    Variable() : type_(NONE), value_("") {}

    Variable(const Type t, std::string v) : type_(t), value_(v) {}
    Variable(const Type t, const char* v) : type_(t), value_(v) {}

    Variable(std::string v) : type_(STRING), value_(v) {}
    Variable(const char* v) : type_(STRING), value_(v) {}
    Variable(int v) : type_(INT)
    {
        value_ = std::to_string(v);
    }
    Variable(float v) : type_(FLOAT)
    {
        value_ = std::to_string(v);
    }

    Type type() const
    {
        return type_;
    }

    // Possible future option? Bugged with intellisense at the moment
    // template <typename T>
    // operator T() const { return to<T>(); }

    Variable& operator= (const Variable& var) { type_ = var.type_; value_ = var.value_; return *this; }
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Converts variable in native C++ types
    ////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    T to() const
    {
        static_assert(false, "Unsupported conversion of console variable to native type");
    }

    template<>
    int to() const
    {
        AssertInt();
        return atoi(value_.c_str());
    }

    template<>
    float to() const
    {
        AssertFloat();
        return static_cast<float>(atof(value_.c_str()));
    }

    template<>
    const char* to() const
    {
        AssertString();
        return value_.c_str();
    }

    template<>
    std::string to() const
    {
        AssertString();
        return value_;
    }

private:
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Type of the variable
    ////////////////////////////////////////////////////////////////////////////////
    Type type_;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Value of the variable
    ////////////////////////////////////////////////////////////////////////////////
    std::string value_;

    inline void AssertFloat() const
    {
        if (type_ != Variable::FLOAT)
        {
            throw "Expected float";
        }
    }

    inline void AssertInt() const
    {
        if (type_ != Variable::INT)
        {
            throw "Expected int";
        }
    }

    inline void AssertString() const
    {
        if (type_ != Variable::STRING && type_ != Variable::FUNCTION)
        {
            throw "Expected string";
        }
    }
};

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

// Registers a variable the console
void __registervariable(const std::string& name, const Variable& var);

// Retrieves a variable from the console
const Variable& __var(const std::string& name);

// Sets the value of an existing variable in the console
void __set_var(const std::string& name, const Variable& var);
} // namespace internal