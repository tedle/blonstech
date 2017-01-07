float __sh_l0m0()
{
    // {l=0,m=0} = 1 / (2 * sqrt(pi))
    return 0.28209479177387814347403972578039f;
}

float __sh_l1m_1(const vec3 v)
{
    // {l=1,m=-1} = -(sqrt(3) * y) / (2 * sqrt(pi))
    return -0.48860251190291992158638462283835f * v.y;
}

float __sh_l1m0(const vec3 v)
{
    // {l=1,m=0} = (sqrt(3) * z) / (2 * sqrt(pi))
    return 0.48860251190291992158638462283835f * v.z;
}

float __sh_l1m1(const vec3 v)
{
    // {l=1,m=1} = -(sqrt(3) * x) / (2 * sqrt(pi))
    return -0.48860251190291992158638462283835f * v.x;
}

float __sh_l2m_2(const vec3 v)
{
    // {l=2,m=-2} = (sqrt(15) * y * x) / (2 * sqrt(pi))
    return 1.0925484305920790705433857058027f * v.y * v.x;
}

float __sh_l2m_1(const vec3 v)
{
    // {l=2,m=-1} = -(sqrt(15) * y * z) / (2 * sqrt(pi))
    return -1.0925484305920790705433857058027f * v.y * v.z;
}

float __sh_l2m0(const vec3 v)
{
    // {l=2,m=0} = (sqrt(5) * (3 * z^2 - 1)) / (4 * sqrt(pi))
    return 0.94617469575756001809268107088713f * v.z * v.z - 0.31539156525252000603089369029571f;
}

float __sh_l2m1(const vec3 v)
{
    // {l=2,m=1} = -(sqrt(15) * x * z) / (2 * sqrt(pi))
    return -1.0925484305920790705433857058027f * v.x * v.z;
}

float __sh_l2m2(const vec3 v)
{
    // {l=2,m=2} = (sqrt(15) * (x^2 - y^2)) / (4 * sqrt(pi))
    return 0.54627421529603953527169285290134f * (v.x * v.x - v.y * v.y);
}

void SHProjectDirection2(vec3 direction, inout vec4 coefficients)
{
    coefficients = vec4(__sh_l0m0(), __sh_l1m_1(direction), __sh_l1m0(direction), __sh_l1m1(direction));
}

void SHProjectDirection3(vec3 direction, inout float coefficients[9])
{
    coefficients[0] = __sh_l0m0();
    coefficients[1] = __sh_l1m_1(direction);
    coefficients[2] = __sh_l1m0(direction);
    coefficients[3] = __sh_l1m1(direction);
    coefficients[4] = __sh_l2m_2(direction);
    coefficients[5] = __sh_l2m_1(direction);
    coefficients[6] = __sh_l2m0(direction);
    coefficients[7] = __sh_l2m1(direction);
    coefficients[8] = __sh_l2m2(direction);
}

float SHDot2(vec4 coeffs_a, vec4 coeffs_b)
{
    return dot(coeffs_a, coeffs_b);
}

float SHDot3(float coeffs_a[9], float coeffs_b[9])
{
    float sh_dot = coeffs_a[0] * coeffs_b[0];
    sh_dot += coeffs_a[1] * coeffs_b[1];
    sh_dot += coeffs_a[2] * coeffs_b[2];
    sh_dot += coeffs_a[3] * coeffs_b[3];
    sh_dot += coeffs_a[4] * coeffs_b[4];
    sh_dot += coeffs_a[5] * coeffs_b[5];
    sh_dot += coeffs_a[6] * coeffs_b[6];
    sh_dot += coeffs_a[7] * coeffs_b[7];
    sh_dot += coeffs_a[8] * coeffs_b[8];
    return sh_dot;
}