#ifndef BLONSTECH_DEBUG_CONSOLEFUNCTION_H_
#define BLONSTECH_DEBUG_CONSOLEFUNCTION_H_

// Includes
#include <functional>
#include <vector>

namespace blons
{
namespace console
{
// Forward declarations
struct ConsoleArg;

template<unsigned...Is>struct indices{ typedef indices type; };
template<unsigned Max, unsigned...Is>struct make_indices :make_indices<Max - 1, Max - 1, Is...>{};
template<unsigned...Is>struct make_indices<0, Is...> :indices<Is...>{};
template<unsigned Max>using make_indices_t = typename make_indices<Max>::type;

class Function
{
public:
    virtual bool Run(std::vector<ConsoleArg> params)=0;
};

template <typename... Args>
class TemplatedFunction : public Function
{
public:
    bool Run(std::vector<ConsoleArg> params);

private:
    std::function<void(Args...)> func_;
    TemplatedFunction(std::function<void(Args...)> func) : func_(func) {}

    template<typename T>
    T read(unsigned int i, std::vector<ConsoleArg> args);

    template<>
    int read(unsigned int i, std::vector<ConsoleArg> args) {
        if (args[i].type != ConsoleArg::Type::INT) {
            throw "Bad news bears";
        }
        return atoi(args[i].value.c_str());
    }

    template<>
    std::string read(unsigned int i, std::vector<ConsoleArg> args) {
        if (args[i].type != ConsoleArg::Type::STRING) {
            throw "Bad news bears";
        }
        return args[i].value;
    }

    template<>
    const char* read(unsigned int i, std::vector<ConsoleArg> args) {
        if (args[i].type != ConsoleArg::Type::STRING) {
            throw "Bad news bears";
        }
        return _strdup(args[i].value.c_str());
    }

    template<unsigned... i>
    bool BuildCall(indices<i...>, std::vector<ConsoleArg> runtime_args)
    {
        if (runtime_args.size() != sizeof...(Is))
        {
            return false;
        }
        auto tupes = std::make_tuple(read<Args>(i, runtime_args)...);
        func_(std::get<i>(tupes)...);
        return true;
    }

};
} // namespace console
} // namespace blons
#endif