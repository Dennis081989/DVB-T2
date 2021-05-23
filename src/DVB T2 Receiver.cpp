// DVB T2 Receiver.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <complex>
#include <memory>


template<typename T>
class delay_line
{
public:
    delay_line(const int line_size) :line_size{ static_cast<uint64_t>(line_size) }
    {
        init();
    }

    void init()
    {
        line.resize(line_size);

        for (int i = 0; i < line.size(); i++)
            line[i] = 0;
        pos_in_line = 0;
    }

    T delay(const T &sample)
    {
        T t = line[pos_in_line % line_size];
        line[pos_in_line % line_size] = sample;
        pos_in_line++;
        return t;
    }

private:
    std::vector<T> line;
    uint64_t pos_in_line,line_size;
};

template<typename T>
class moving_average_filter
{
public:

    moving_average_filter(const int line_size) :line_size{ static_cast<uint64_t>(line_size) }
    {
        init();
    }

    void init()
    {
        line.resize(line_size);

        for (int i = 0; i < line.size(); i++)
            line[i] = 0;
        pos_in_line = 0;
        sum = 0;
    }

    T to_filter(const T& sample)
    {
        T t = line[pos_in_line % line_size];
        sum += sample;
        sum -= t;
        line[pos_in_line % line_size] = sample;
        pos_in_line++;
        return sum;
    }
private:
    T sum;
    std::vector<T> line;
    uint64_t pos_in_line, line_size;
};

template <typename T>
class P1_detection
{
public:

    P1_detection()
    {
        ptr_to_delayA = std::make_unique<delay_line<T>>(Ta);
        ptr_to_delayB = std::make_unique<delay_line<T>>(Tb);
        ptr_to_delayC = std::make_unique<delay_line<T>>(Tc);
        ptr_to_upper_filter = std::make_unique<moving_average_filter<T>>(Tr);
        ptr_to_lower_filter = std::make_unique<moving_average_filter<T>>(Tr);
        init();
    }

    void init()
    {
        phase = 1;
        theta = std::polar(1.f, 2* PI * f_SH / f_d);
        ptr_to_delayA->init();
        ptr_to_delayB->init();
        ptr_to_delayC->init();
        ptr_to_upper_filter->init();
        ptr_to_lower_filter->init();
    }

    T autocorr_PtByPt(const T& s)
    {
        T down_shifter_sample = phase * s;
        phase *= theta;
        phase /= abs(phase);

        T sample_to_upper_MA = mul_with_conj(s, ptr_to_delayC->delay(down_shifter_sample));
        T sample_to_lower_MA = mul_with_conj(down_shifter_sample, ptr_to_delayB->delay(s));
        
        return ptr_to_delayA->delay(ptr_to_upper_filter->to_filter(sample_to_upper_MA)) * 
           ( ptr_to_lower_filter->to_filter(sample_to_lower_MA));
        
    }
private:

    T mul_with_conj(const T a,const T b) const { return a * std::conj(b); }

    std::unique_ptr<delay_line<T>> ptr_to_delayA, ptr_to_delayB, ptr_to_delayC;
    std::unique_ptr<moving_average_filter<T>> ptr_to_upper_filter, ptr_to_lower_filter;
    int Ta, Tb, Tc,Tr;
    const float PI = 3.1415926;
    float f_SH,f_d;
    T theta, phase;
};

int main()
{
    delay_line<int> delay(1);

    for (int i = 0; i < 10; i++)
    {
        //std::cout << i << " " << delay.delay(i) << std::endl;
    }

    moving_average_filter<int> ma(2);
    for (int i = 0; i < 10; i++)
    {
        std::cout << i << " " << ma.to_filter(i) << std::endl;
    }

    P1_detection<std::complex<float>> p1_detection;
    std::complex<float> v;
    p1_detection.autocorr_PtByPt(v);

}


