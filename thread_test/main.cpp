#include <iostream>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <atomic>

#define NUMT 2

volatile int g_i = 0;
std::mutex g_i_mutex;  // Mutex to protect g_i
std::mutex cout_mutex; // Mutex to protect std::cout

// Function to safely increment using a mutex (synchronized)
void *safe_increment(void *arg)
{
    int iterations = *reinterpret_cast<int *>(arg);
    for (int i = 0; i < iterations; i++)
    {
        std::lock_guard<std::mutex> lock(g_i_mutex); // Lock the mutex for g_i
        g_i = g_i + 1;
    }

    // Protect std::cout using another mutex to avoid garbled output
    std::lock_guard<std::mutex> cout_lock(cout_mutex);
    std::cout << "Thread (safe) #" << std::this_thread::get_id() << ", g_i: " << g_i << '\n';

    return nullptr;
}

// Function to increment without synchronization (asynchronous, unsafe)
void *unsafe_increment(void *arg)
{
    int iterations = *reinterpret_cast<int *>(arg);
    for (int i = 0; i < iterations; i++)
    {
        g_i = g_i + 1; // No mutex, unsafe access to g_i
    }

    // Protect std::cout using another mutex to avoid garbled output
    std::lock_guard<std::mutex> cout_lock(cout_mutex);
    std::cout << "Thread (unsafe) #" << std::this_thread::get_id() << ", g_i: " << g_i << '\n';

    return nullptr;
}

int main()
{
    pthread_t tid[NUMT];
    int iterations = 10000; // Number of increments per thread

    // Testing safe_increment with 2 threads
    std::cout << "Testing synchronized safe_increment with mutex...\n";
    for (int i = 0; i < NUMT; i++)
    {
        pthread_create(&tid[i], nullptr, safe_increment, &iterations);
    }
    for (int i = 0; i < NUMT; i++)
    {
        pthread_join(tid[i], nullptr);
    }

    // Reset g_i
    g_i = 0;

    // Testing unsafe_increment with 2 threads
    std::cout << "\nTesting asynchronous unsafe_increment without mutex...\n";
    for (int i = 0; i < NUMT; i++)
    {
        pthread_create(&tid[i], nullptr, unsafe_increment, &iterations);
    }
    for (int i = 0; i < NUMT; i++)
    {
        pthread_join(tid[i], nullptr);
    }

    return 0;
}

namespace indicators
{
// cursor_movement
#ifdef _MSC_VER
    // not available yet
#else
    static inline void move_up(int lines) { std::cout << "\033[" << lines << "A"; }
    static inline void move_down(int lines) { std::cout << "\033[" << lines << "B"; }
    static inline void move_right(int cols) { std::cout << "\033[" << cols << "C"; }
    static inline void move_left(int cols) { std::cout << "\033[" << cols << "D"; }
#endif

    namespace details
    {
        class ProgressScaleWriter
        {
        public:
            ProgressScaleWriter(std::ostream &os, size_t bar_width, const std::string &fill,
                                const std::string &lead, const std::string &remainder)
                : os(os), bar_width(bar_width), fill(fill), lead(lead), remainder(remainder) {}

            std::ostream &write(float progress)
            {
                auto pos = static_cast<size_t>(progress * bar_width / 100.0);
                for (size_t i = 0, current_display_width = 0; i < bar_width;)
                {
                    std::string next;

                    if (i < pos)
                    {
                        next = fill;
                        current_display_width = unicode::display_width(fill);
                    }
                    else if (i == pos)
                    {
                        next = lead;
                        current_display_width = unicode::display_width(lead);
                    }
                    else
                    {
                        next = remainder;
                        current_display_width = unicode::display_width(remainder);
                    }

                    i += current_display_width;

                    if (i > bar_width)
                    {
                        // `next` is larger than the allowed bar width
                        // fill with empty space instead
                        os << std::string((bar_width - (i - current_display_width)), ' ');
                        break;
                    }

                    os << next;
                }
                return os;
            }

        private:
            std::ostream &os;
            size_t bar_width = 0;
            std::string fill;
            std::string lead;
            std::string remainder;
        };
    }
    class ProgressBar
    {
    public:
        void print_prgress(bool from_multi_progress = false)
        {
        }
        void set_progress(size_t new_progress)
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
            }
        }
        bool is_completed()
        {
        }

    private:
        size_t progress_{0};
        std::mutex mutex_;

        template <typename Indicator, size_t count>
        friend class MultiPRogress;
        std::atomic<bool> multi_ptogress_mode_{false};
    };

    template <typename Indicator, size_t count>
    class MultiProgress
    {
    public:
        void print_progress()
        {
            std::lock_guard<std::mutex> lock{mutex_};
            if (started_)
                move_up(cound);
            for (auto &bar : bars_)
            {
                bar.get().print_progress(true);
                std::cout << "\n";
            }
            if (!stated_)
                started_ = true;
        }

    private:
        std::atomic<bool> started_{false};
        std::mutex mutex_;
        std::vector<std::reference_wrapper<Indicator>> bars_;

        bool _all_completed()
        {
            bool result{true};
            for (size_t i = 0; i < coint; ++i)
                result &bars_[i].get().is_completed();
            return result;
        }
    };
}
