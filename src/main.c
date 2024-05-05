#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define NUMBER_OF_32BIT_PRIMES 203280221
#define ODD_PRIME_BITMASK_SIZE 4294967296 / 2 / 8      // in bytes

int64_t nano_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (int64_t)ts.tv_sec * 1000000000 + (int64_t)ts.tv_nsec;
}

uint32_t all_32bit_primes[NUMBER_OF_32BIT_PRIMES + 1];          // we do this so that we get no errors when trying to access the prime after the largest 32 bit prime. makes the code run just a little faster
uint32_t *all_odd_32bit_primes = &all_32bit_primes[1];

uint8_t odd_prime_bitmask[ODD_PRIME_BITMASK_SIZE] = {0};

bool check_if_odd_32bit_prime(uint64_t prime_candidate)
{
    uint64_t half_prime_candidate = prime_candidate / 2;
    uint8_t bitmask_byte = odd_prime_bitmask[half_prime_candidate / 8];
    return (bitmask_byte << (half_prime_candidate % 8)) & 0b10000000;
}

void write_odd_prime_bitmask(uint64_t prime)
{
    uint64_t half_prime = prime / 2;
    uint8_t original_byte = odd_prime_bitmask[half_prime / 8];
    odd_prime_bitmask[half_prime / 8] = original_byte | (0b10000000 >> (half_prime % 8));
}

uint64_t fill_odd_prime_bitmask_from_to(uint64_t start, uint64_t end)
{
    uint64_t number_of_primes_found = 0;
    if (start % 2 == 0) start++;
    for (uint64_t prime_candidate = start; prime_candidate < end; prime_candidate = prime_candidate + 2)
    {
        // printf("current_prime_index = %llu, prime_candidate = %llu\n", current_prime_index, prime_candidate);
        uint64_t test_with_prime_index = 0;
        uint64_t divisor = 0;
        while (true)
        {
            divisor = all_odd_32bit_primes[test_with_prime_index];
            // printf("divisor = %llu\n", divisor);
            if (divisor * divisor > prime_candidate)
            {
                number_of_primes_found++;
                // highest_known_prime_index++;
                // all_odd_32bit_primes[highest_known_prime_index] = prime_candidate;
                write_odd_prime_bitmask(prime_candidate);
                test_with_prime_index = 0;
                break;
            }
            if (prime_candidate % divisor == 0)
            {
                test_with_prime_index = 0;
                break;
            }
            test_with_prime_index++;
        }
        //not_a_prime:;
    }
    // return highest_known_prime_index;
    return number_of_primes_found;
}

uint64_t fill_prime_array_to_65536()
{
    uint64_t highest_known_prime_index = 0;
    for (uint64_t prime_candidate = 5; prime_candidate < 65536; prime_candidate = prime_candidate + 2)
    {
        // printf("current_prime_index = %llu, prime_candidate = %llu\n", current_prime_index, prime_candidate);
        uint64_t test_with_prime_index = 0;
        uint64_t divisor = 0;
        while (true)
        {
            divisor = all_odd_32bit_primes[test_with_prime_index];
            // printf("divisor = %llu\n", divisor);
            if (divisor * divisor > prime_candidate)
            {
                highest_known_prime_index++;
                all_odd_32bit_primes[highest_known_prime_index] = prime_candidate;
                write_odd_prime_bitmask(prime_candidate);
                test_with_prime_index = 0;
                break;
            }
            if (prime_candidate % divisor == 0)
            {
                test_with_prime_index = 0;
                break;
            }
            test_with_prime_index++;
        }
        //not_a_prime:;
    }
    // this needs to be here so that we get known_prime ^ 2 > prime_candidate
    // if we don't do this the search will never end since there is no prime just a little bit bigger than 65536
    all_odd_32bit_primes[highest_known_prime_index + 1] = 18446744073709551615;    // should be cropped to 4294967295
    return highest_known_prime_index;
}

uint64_t smallest_prime_factor(uint64_t prime_candidate)
{
    uint64_t test_with_prime_index = 0;
    uint64_t divisor = 0;
    while (true)
    {
        divisor = all_32bit_primes[test_with_prime_index];
        // printf("divisor = %llu\n", divisor);
        if (divisor * divisor > prime_candidate || test_with_prime_index >= NUMBER_OF_32BIT_PRIMES)
        {
            // number_of_primes_found++;
            // highest_known_prime_index++;
            // all_odd_32bit_primes[highest_known_prime_index] = prime_candidate;
            // write_odd_prime_bitmask(prime_candidate);
            // test_with_prime_index = 0;
            // break;
            return 0;
        }
        if (prime_candidate % divisor == 0)
        {
            return divisor;
        }
        test_with_prime_index++;
    }
    return 0;
}

bool check_if_prime(uint64_t prime_candidate)
{
    if (prime_candidate < 2) return false;
    if (prime_candidate == 2) return true;
    if (prime_candidate % 2 == 0) return false;
    if (prime_candidate < 4294967296) return check_if_odd_32bit_prime(prime_candidate);
    
    uint64_t factor = smallest_prime_factor(prime_candidate);
    if (factor == 0) return true;
    printf("Smallest prime divisor is %llu\n", factor);
    return false;
}

uint64_t odd_prime_bitmask_to_prime_array(uint64_t end)
{
    uint64_t odd_prime_index = 0;
    for (uint64_t prime_candidate = 3; prime_candidate <= end; prime_candidate = prime_candidate + 2)
    {
        if (check_if_odd_32bit_prime(prime_candidate))
        {
            all_odd_32bit_primes[odd_prime_index] = prime_candidate;
            odd_prime_index++;
        }
    }
    return odd_prime_index;
}

bool read_odd_prime_bitmask_file_if_it_exists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    uint64_t number_of_read_uint32_t = 0;
    if (file != NULL)
    {
        number_of_read_uint32_t = fread(odd_prime_bitmask, sizeof(uint8_t), ODD_PRIME_BITMASK_SIZE, file);
        fclose(file); // close the file
    }
    else
    {
        return false;
    }
    if (number_of_read_uint32_t < ODD_PRIME_BITMASK_SIZE)
    {
        printf("Couldn't read the whole file\n");
        exit(1);
    }
    odd_prime_bitmask_to_prime_array(4294967296);
    return true;
}

int main()
{
    all_32bit_primes[0] = 2;
    all_odd_32bit_primes[0] = 3;
    if (read_odd_prime_bitmask_file_if_it_exists("odd_prime_bitmask.32b")) goto skip;
    
    int64_t start_time_ns = nano_time();
    
    odd_prime_bitmask[0] = 0b01110110;
    odd_prime_bitmask[1] = 0b11010011;
    // all_32bit_primes[2] = 5;
    
    uint64_t number_of_16bit_primes = fill_prime_array_to_65536() + 2;
    printf("There are %llu 16 bit primes\n", number_of_16bit_primes);
    
    fill_odd_prime_bitmask_from_to(65536, 4294967296);
    
    uint64_t number_of_32bit_primes = odd_prime_bitmask_to_prime_array(4294967296) + 1;
    printf("There are %llu 32 bit primes\n", number_of_32bit_primes);
    
    for (uint64_t i = 0; i < 20; i++)
    {
        printf("%3llu ", all_32bit_primes[i]);
    }
    printf("\n");
    
    int64_t end_time_ns = nano_time();
    int64_t delta_time_ns = end_time_ns - start_time_ns;
    
    printf("Calculating them took %lld hours, %lld minutes and %lld seconds\n",
        delta_time_ns / 1000000000 / 60 / 60,
        delta_time_ns / 1000000000 / 60,
        delta_time_ns / 1000000000);
    
    int64_t written = 0;
    FILE *f;
    /*
    f = fopen("primes.32b", "wb");
    written = fwrite(all_odd_32bit_primes, sizeof(uint32_t), number_of_32bit_primes - 1, f);
    if (written == 0)
    {
        printf("Error during writing to file !");
    }
    fclose(f);
    */
    
    written = 0;
    f = fopen("odd_prime_bitmask.32b", "wb");
    written = fwrite(odd_prime_bitmask, sizeof(uint8_t), ODD_PRIME_BITMASK_SIZE, f);
    if (written == 0)
    {
        printf("Error during writing to file !");
    }
    fclose(f);
    
    skip:;
    
    while(true)
    {
        uint64_t prime_candidate = 0;
        printf("\nType a potential prime number:\n");
        scanf("%llu", &prime_candidate);
        if (check_if_prime(prime_candidate))
        {
            printf("\x1B[32mHooray %llu is a prime\x1B[0m\n", prime_candidate);
        }
        else
        {
            printf("\x1B[31mBummer %llu is not a prime\x1B[0m\n", prime_candidate);
        }
    }
    return 0;
}
