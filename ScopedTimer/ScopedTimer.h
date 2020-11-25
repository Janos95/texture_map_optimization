//
// Created by Janos Meny on 9/4/19.
//

#pragma once

/**
 * Timer class. The timing is started when a variable
 * the variable is defined and is stopped when the variable
 * goes out of scope. The timing is written into a global
 * hash map using the provided name as a key. If multiple
 * measurements are performed for the same key, an average
 * is stored and a rolling standard deviation is computed.
 * The content of the hash map can be printed using the
 * printStatistics member function.
 */
class ScopedTimer
{
public:

    /**
     * @param name of the timer
     * @param verbose if yes, then the timing is printed on destruction.
     * Turned off by default.
     */
    explicit ScopedTimer(char const* name, bool verbose = false);

    ~ScopedTimer();

    /**
     * This goes through the set of stored timings
     * and prints them together with its standard
     * deviation.
     */
    static void printStatistics();

private:
    struct Impl;
    Impl* m_impl;
};
