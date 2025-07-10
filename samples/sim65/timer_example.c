/*
 * Sim65 timer example.
 *
 * Description
 * -----------
 *
 * This example tests the clock cycle counter feature of sim65.
 *
 * The function 'timestamp' obtains the lower 32-bits of the clock cycle counter.
 *
 * The function 'calc_sum_terms' calculates the sum of a range of integers
 * starting at zero. It simply iterates over all terms, which means that its
 * runtime is a linear function of its input value.
 *
 * In the main function, we first derive an 'offset' value by getting two timestamp
 * values, with nothing happening in between. Ideally this should yield a 0 clock
 * cycle duration, but due to the overhead of calling the 'timestamp' function,
 * and the 'timestamp' function itself, the difference between these timestamp
 * will be non-zero. We store this value in the 'overhead' variable, and subtract
 * this value in later measurements.
 *
 * Next, we measure the duration of calling the function 'calc_sum_terms' with two
 * input values, 0, and 1. The duration includes storing the result in the 'result'
 * variable.
 *
 * Extrapolating from these two measurements, and assuming that the runtime of
 * calling 'calc_sum_terms' and storing its result scales linearly with its argument,
 * we can predict the duration of a call to 'calc_sum_terms' with a much larger
 * argument (max_terms = 10000).
 *
 * Finally, we actually measure the duration with max_terms = 10000. If the
 * duration measured is equal to the predicted value, we exit successfully. If not,
 * we exit with failure.
 *
 * Running the example
 * -------------------
 *
 * cl65 -t sim6502 -O timer_example.c -o timer_example.prg
 * sim65 timer_example.prg
 *
 */

#include <stdio.h>
#include <sim65.h>

static uint32_t timestamp(void)
{
    peripherals.counter.select = COUNTER_SELECT_CLOCKCYCLE_COUNTER;
    peripherals.counter.latch = 0;
    return peripherals.counter.value32[0];
}

static unsigned long calc_sum_terms(unsigned max_term)
/* A function with a runtime that scales linearly with its argument. */
{
    unsigned k;
    unsigned long sum = 0;
    for (k = 0; k <= max_term; ++k)
    {
        sum += k;
    }
    return sum;
}

int main(void)
{
    unsigned max_term;
    unsigned long result;
    uint32_t t1, t2, overhead;
    int32_t d0, d1, duration;
    int32_t predicted_duration;

    /* Calibration measurement of zero clock cycles, to determine the overhead. */

    overhead = 0;
    t1 = timestamp();
    t2 = timestamp() - overhead;
    overhead = (t2 - t1);

    /* Calculate call duration (including assignment of result) for argument value 0. */

    max_term = 0;
    t1 = timestamp();
    result = calc_sum_terms(max_term);
    t2 = timestamp();
    d0 = (t2 - t1) - overhead;
    printf("max_term = %u -> result = %lu; duration = %ld\n", max_term, result, d0);

    /* Calculate call duration (including assignment of result) for argument value 1. */

    max_term = 1;
    t1 = timestamp();
    result = calc_sum_terms(max_term);
    t2 = timestamp();
    d1 = (t2 - t1) - overhead;
    printf("max_term = %u -> result = %lu; duration = %ld\n", max_term, result, d1);

    /* Predict runtime for a much bigger argument value, 10000. */

    max_term = 10000;
    predicted_duration = d0 + max_term * (d1 - d0);

    printf("predicted duration for max_term = %u: %ld\n", max_term, predicted_duration);

    /* Do the actual measurement for max_term = 10000.
     *  Note: equality between the prediction and the measurement is only achieved if we compile with -O.
     */

    t1 = timestamp();
    result = calc_sum_terms(max_term);
    t2 = timestamp();
    duration = (t2 - t1) - overhead;
    printf("max_term = %u -> result = %lu; duration = %ld\n", max_term, result, duration);


    return 0;
}
