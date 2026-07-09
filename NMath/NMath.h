#pragma once

#include <random>
#include <cstdint>
#include <cmath>
#include "imath.h"


namespace NMath
{
    class CMath : public IMath
    {
    public:

        //--------------------------------------------------------------------------
        // Rounding
        //--------------------------------------------------------------------------

        // Rounds a value down toward negative infinity.
        inline double Floor(double x) const override
        {
            return std::floor(x);
        }

        // Rounds a value up toward positive infinity.
        inline double Ceiling(double x) const override
        {
            return std::ceil(x);
        }

        // Rounds a value to the nearest integer.
        inline double Round(double x) const override
        {
            return std::round(x);
        }

        //--------------------------------------------------------------------------
        // Trigonometry (angles in radians)
        //--------------------------------------------------------------------------

        // Returns the sine of an angle in radians.
        inline double Sin(double radians) const override
        {
            return std::sin(radians);
        }

        // Returns the cosine of an angle in radians.
        inline double Cos(double radians) const override
        {
            return std::cos(radians);
        }

        // Returns the tangent of an angle in radians.
        inline double Tan(double radians) const override
        {
            return std::tan(radians);
        }

        //--------------------------------------------------------------------------
        // Angle conversion
        //--------------------------------------------------------------------------

        // Converts degrees to radians.
        inline double DegreesToRadians(double degrees) const override
        {
            return degrees * (PI / 180.0);
        }

        // Converts radians to degrees.
        inline double RadiansToDegrees(double radians) const override
        {
            return radians * (180.0 / PI);
        }

        //--------------------------------------------------------------------------
        // Logarithms & exponentials
        //--------------------------------------------------------------------------

        // Returns the natural logarithm (base e).
        inline double Log(double x) const override
        {
            return std::log(x);
        }

        // Returns the common logarithm (base 10).
        inline double Log10(double x) const override
        {
            return std::log10(x);
        }

        // Returns Euler's number (e) raised to the specified power.
        inline double Exp(double x) const override
        {
            return std::exp(x);
        }

        //--------------------------------------------------------------------------
        // Mathematical constants
        //--------------------------------------------------------------------------

        // Returns the value of π (pi).
        double GetPi() const override
        {
            return PI;
        }

        // Returns the value of Euler's number (e).
        double GetEulersNumber() const override
        {
            return E;
        }

private:

        static constexpr double PI = 3.1415926535897932384;
        static constexpr double E = 2.7182818284590452353;
    };

    class CRandom : public IRandom
    {
    private:
        // Random number generator engine.
        static std::mt19937_64 g_RandomEngine;

    public:

        //--------------------------------------------------------------------------
        // Uniform distributions
        //--------------------------------------------------------------------------

        // Returns a uniformly distributed random integer in the specified range.
        int RandomInt(int lower, int upper) override
        {
            return std::uniform_int_distribution<int>(lower, upper)(g_RandomEngine);
        }

        // Returns a uniformly distributed random float in the specified range.
        float RandomFloat(float lower, float upper) override
        {
            return std::uniform_real_distribution<float>(lower, upper)(g_RandomEngine);
        }

        // Returns a uniformly distributed random double in the specified range.
        double RandomDouble(double lower, double upper) override
        {
            return std::uniform_real_distribution<double>(lower, upper)(g_RandomEngine);
        }

        // Returns a uniformly distributed random long double in the specified range.
        long double RandomLongDouble(long double lower, long double upper) override
        {
            return std::uniform_real_distribution<long double>(lower, upper)(g_RandomEngine);
        }

        //--------------------------------------------------------------------------
        // Normal (Gaussian) distributions
        //--------------------------------------------------------------------------

        // Returns a normally distributed random float.
        float RandomGaussianFloat(float mean, float stddev) override
        {
            return std::normal_distribution<float>(mean, stddev)(g_RandomEngine);
        }

        // Returns a normally distributed random double.
        double RandomGaussianDouble(double mean, double stddev) override
        {
            return std::normal_distribution<double>(mean, stddev)(g_RandomEngine);
        }

        // Returns a normally distributed random long double.
        long double RandomGaussianLongDouble(long double mean, long double stddev) override
        {
            return std::normal_distribution<long double>(mean, stddev)(g_RandomEngine);
        }

        //--------------------------------------------------------------------------
        // Boolean
        //--------------------------------------------------------------------------

        // Returns either true or false with equal probability.
        bool RandomBool() override
        {
            return std::bernoulli_distribution(0.5)(g_RandomEngine);
        }

        //--------------------------------------------------------------------------
        // Character types
        //--------------------------------------------------------------------------

        // Returns a random char in the specified range.
        char RandomChar(char lower, char upper) override
        {
            return static_cast<char>(
                std::uniform_int_distribution<int>(
                    static_cast<int>(lower),
                    static_cast<int>(upper)
                )(g_RandomEngine)
                );
        }

        // Returns a random signed char in the specified range.
        signed char RandomSChar(signed char lower, signed char upper) override
        {
            return static_cast<signed char>(
                std::uniform_int_distribution<int>(
                    static_cast<int>(lower),
                    static_cast<int>(upper)
                )(g_RandomEngine)
                );
        }

        // Returns a random unsigned char in the specified range.
        unsigned char RandomUChar(unsigned char lower, unsigned char upper) override
        {
            return static_cast<unsigned char>(
                std::uniform_int_distribution<unsigned int>(
                    static_cast<unsigned int>(lower),
                    static_cast<unsigned int>(upper)
                )(g_RandomEngine)
                );
        }

        // Returns a random wide character in the specified range.
        wchar_t RandomWChar(wchar_t lower, wchar_t upper) override
        {
            return static_cast<wchar_t>(
                std::uniform_int_distribution<unsigned int>(
                    static_cast<unsigned int>(lower),
                    static_cast<unsigned int>(upper)
                )(g_RandomEngine)
                );
        }

        //--------------------------------------------------------------------------
        // Standard integer types
        //--------------------------------------------------------------------------

        // Returns a random short integer in the specified range.
        short RandomShort(short lower, short upper) override
        {
            return std::uniform_int_distribution<short>(lower, upper)(g_RandomEngine);
        }

        // Returns a random unsigned short integer in the specified range.
        unsigned short RandomUShort(unsigned short lower, unsigned short upper) override
        {
            return std::uniform_int_distribution<unsigned short>(lower, upper)(g_RandomEngine);
        }

        // Returns a random unsigned integer in the specified range.
        unsigned int RandomUInt(unsigned int lower, unsigned int upper) override
        {
            return std::uniform_int_distribution<unsigned int>(lower, upper)(g_RandomEngine);
        }

        // Returns a random long integer in the specified range.
        long RandomLong(long lower, long upper) override
        {
            return std::uniform_int_distribution<long>(lower, upper)(g_RandomEngine);
        }

        // Returns a random unsigned long integer in the specified range.
        unsigned long RandomULong(unsigned long lower, unsigned long upper) override
        {
            return std::uniform_int_distribution<unsigned long>(lower, upper)(g_RandomEngine);
        }

        // Returns a random long long integer in the specified range.
        long long RandomLongLong(long long lower, long long upper) override
        {
            return std::uniform_int_distribution<long long>(lower, upper)(g_RandomEngine);
        }

        // Returns a random unsigned long long integer in the specified range.
        unsigned long long RandomULongLong(unsigned long long lower, unsigned long long upper) override
        {
            return std::uniform_int_distribution<unsigned long long>(lower, upper)(g_RandomEngine);
        }

        //--------------------------------------------------------------------------
        // Fixed-width integer types
        //--------------------------------------------------------------------------

        // Returns a random 8-bit signed integer.
        int8_t RandomInt8(int8_t lower, int8_t upper) override
        {
            return static_cast<int8_t>(
                std::uniform_int_distribution<int>(
                    static_cast<int>(lower),
                    static_cast<int>(upper)
                )(g_RandomEngine)
                );
        }

        // Returns a random 8-bit unsigned integer.
        uint8_t RandomUInt8(uint8_t lower, uint8_t upper) override
        {
            return static_cast<uint8_t>(
                std::uniform_int_distribution<unsigned int>(
                    static_cast<unsigned int>(lower),
                    static_cast<unsigned int>(upper)
                )(g_RandomEngine)
                );
        }

        // Returns a random 16-bit signed integer.
        int16_t RandomInt16(int16_t lower, int16_t upper) override
        {
            return std::uniform_int_distribution<int16_t>(lower, upper)(g_RandomEngine);
        }

        // Returns a random 16-bit unsigned integer.
        uint16_t RandomUInt16(uint16_t lower, uint16_t upper) override
        {
            return std::uniform_int_distribution<uint16_t>(lower, upper)(g_RandomEngine);
        }

        // Returns a random 32-bit signed integer.
        int32_t RandomInt32(int32_t lower, int32_t upper) override
        {
            return std::uniform_int_distribution<int32_t>(lower, upper)(g_RandomEngine);
        }

        // Returns a random 32-bit unsigned integer.
        uint32_t RandomUInt32(uint32_t lower, uint32_t upper) override
        {
            return std::uniform_int_distribution<uint32_t>(lower, upper)(g_RandomEngine);
        }

        // Returns a random 64-bit signed integer.
        int64_t RandomInt64(int64_t lower, int64_t upper) override
        {
            return std::uniform_int_distribution<int64_t>(lower, upper)(g_RandomEngine);
        }

        // Returns a random 64-bit unsigned integer.
        uint64_t RandomUInt64(uint64_t lower, uint64_t upper) override
        {
            return std::uniform_int_distribution<uint64_t>(lower, upper)(g_RandomEngine);
        }
    };
}
