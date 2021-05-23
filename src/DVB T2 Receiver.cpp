// DVB T2 Receiver.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <complex>


class P1_detection
{
    typedef std::complex<float> IQf;
public:

    void init()
    {
        phase = 1;
        theta = std::polar(1.f, 2* PI * f_SH / f_d);
        delay_line_Ta.resize(Ta);
        delay_line_Tb.resize(Tb);
        delay_line_Tc.resize(Tc);
    }

    void autocorr( std::vector<IQf> &piece_of_signal)
    {
        for (int i = 0; i < piece_of_signal.size(); i++)
        {
            phase *= theta;
            phase /= abs(phase);

            IQf down_shifter_sample = phase * piece_of_signal[i];


        }
    }
private:
    int Ta, Tb, Tc;
    const float PI = 3.1415926;
    float f_SH,f_d;
    IQf theta, phase;
    std::vector<IQf> delay_line_Tb, delay_line_Tc, delay_line_Ta;
};

int main()
{
    
}


