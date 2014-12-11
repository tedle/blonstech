namespace internal
{
struct ConsoleArg
{
    enum Type
    {
        NONE,
        FUNCTION,
        STRING,
        INT,
        FLOAT
    } type;
    std::string value;
};

template<unsigned... i>struct Indices{ typedef Indices type; };
template<unsigned size, unsigned... i>struct MakeIndices : MakeIndices<size - 1, size - 1, i...> {};
template<unsigned... i>struct MakeIndices<0, i...> : Indices<i...> {};
template<unsigned size>using MakeIndicesType = typename MakeIndices<size>::type;

class Function
{
public:
    virtual bool Run(const std::vector<ConsoleArg>& args)=0;
    virtual std::vector<ConsoleArg::Type> ArgList()=0;
};

template <typename... Args>
class TemplatedFunction : public Function
{
public:
    TemplatedFunction(std::function<void(Args...)> func) : func_(func) {}

    bool Run(const std::vector<ConsoleArg>& runtime_args)
    {
        return PackArgsAndCall(MakeIndicesType<sizeof...(Args)>{}, runtime_args);
    }

    std::vector<ConsoleArg::Type> ArgList()
    {
        return { ArgType<Args>()... };
    }

private:
    std::function<void(Args...)> func_;

    void AssertFloat(const ConsoleArg& arg)
    {
        if (arg.type != ConsoleArg::FLOAT)
        {
            throw "Expected float";
        }
    }

    void AssertInt(const ConsoleArg& arg)
    {
        if (arg.type != ConsoleArg::INT)
        {
            throw "Expected int";
        }
    }

    void AssertString(const ConsoleArg& arg)
    {
        if (arg.type != ConsoleArg::STRING)
        {
            throw "Expected string";
        }
    }

    template<typename T>
    ConsoleArg::Type ArgType()
    {
        static_assert(false, "Unsupported argument type in console function");
    }

    template<>
    ConsoleArg::Type ArgType<int>() { return ConsoleArg::INT; }
    template<>
    ConsoleArg::Type ArgType<std::string>() { return ConsoleArg::STRING; }
    template<>
    ConsoleArg::Type ArgType<const char*>() { return ConsoleArg::STRING; }

    template<typename T>
    T UnpackArg(unsigned int i, const std::vector<ConsoleArg>& args)
    {
        static_assert(false, "Unsupported argument type in console function");
    }

    template<>
    int UnpackArg(unsigned int i, const std::vector<ConsoleArg>& args)
    {
        AssertInt(args[i]);
        return atoi(args[i].value.c_str());
    }

    template<>
    std::string UnpackArg(unsigned int i, const std::vector<ConsoleArg>& args)
    {
        AssertString(args[i]);
        return args[i].value;
    }

    template<>
    const char* UnpackArg(unsigned int i, const std::vector<ConsoleArg>& args)
    {
        AssertString(args[i]);
        return _strdup(args[i].value.c_str());
    }

    template<unsigned... i>
    bool PackArgsAndCall(Indices<i...>, const std::vector<ConsoleArg>& runtime_args)
    {
        if (runtime_args.size() != sizeof...(i))
        {
            return false;
        }
        try
        {
            auto arg_tuple = std::make_tuple(UnpackArg<Args>(i, runtime_args)...);
            func_(std::get<i>(arg_tuple)...);
        }
        catch (const char*)
        {
            return false;
        }
        return true;
    }
};

void __register(const std::string& name, Function* func);
} // namespace internal