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
////////////////////////////////////////////////////////////////////////////////
/// \brief Contains an immutable typed console variable
///
/// Can easily convert to native types through the templated Variable::to()
/// function. Will throw if a type mismatch occurs. See blons::console for valid
/// types.
////////////////////////////////////////////////////////////////////////////////
class Variable
{
public:
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

// Registers a variable the console
void __registervariable(const std::string& name, const Variable& var);

// Retrieves a variable from the console
const Variable& __var(const std::string& name);

// Sets the value of an existing variable in the console
void __set_var(const std::string& name, const Variable& var);
} // namespace internal