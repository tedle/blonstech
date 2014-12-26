namespace internal
{
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains a typed console variable
///
/// Can easily convert to native types through the templated Variable::to()
/// function. Will throw if a type mismatch occurs.
///
/// Valid C++ types include:
/// * int
/// * const char*
/// * std::string
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

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Type of the variable
    ////////////////////////////////////////////////////////////////////////////////
    Type type;
    ////////////////////////////////////////////////////////////////////////////////
    /// \brief Value of the variable
    ////////////////////////////////////////////////////////////////////////////////
    std::string value;

    // Possible future option? Bugged with intellisense at the moment
    // template <typename T>
    // operator T() const { return to<T>(); }

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
        return atoi(value.c_str());
    }

    template<>
    const char* to() const
    {
        AssertString();
        return value.c_str();
    }

    template<>
    std::string to() const
    {
        AssertString();
        return value;
    }

private:
    inline void AssertFloat() const
    {
        if (type != Variable::FLOAT)
        {
            throw "Expected float";
        }
    }

    inline void AssertInt() const
    {
        if (type != Variable::INT)
        {
            throw "Expected int";
        }
    }

    inline void AssertString() const
    {
        if (type != Variable::STRING)
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
void __register(const std::string& name, Function* func);

// Retrieves a variable from the console
const Variable& __var(const std::string& name);
} // namespace internal