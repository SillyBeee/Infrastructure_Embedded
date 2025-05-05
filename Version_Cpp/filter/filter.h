#ifndef __FILTER_H_
#define __FILTER_H_

#ifdef __cplusplus
extern "C"
{
#endif





#ifdef __cplusplus
}
#endif
class Filter {
public:
    virtual ~Filter() = default;

    virtual float GetValue() const = 0;
};


class FirstOrderFilter final : public Filter
{
public:
    explicit FirstOrderFilter(float const coefficient) : First_Order_Filtering_Coefficient(coefficient) ,Value(0) {}

    float GetValue() const  override
    {
        return Value;
    }
    ~FirstOrderFilter() override = default;
protected:
    void Update(float  const input)
    {
        Value = First_Order_Filtering_Coefficient * input + (1 - First_Order_Filtering_Coefficient) * Value;
    }
    float First_Order_Filtering_Coefficient;
    float Value;
};


// class SecondOrderFilter final: public Filter
// {
// public:
//     SecondOrderFilter();
//
//
//     ~SecondOrderFilter() override = default;
//
// private:
// };






#endif
