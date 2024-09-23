#ifndef _HEADERH
#define _HEADERH

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <iomanip>

class ProgressBar
{
private:
    std::string prefix;
    std::string postfix = "";
    float percentage = 0.0;
    int bar_width = 50;
    std::mutex mutex_;
    bool is_stop = false;     // Flag to indicate if the progress bar should stop
    bool is_complete = false; // Flag to indicate if the progress reached 100%

    void write_()
    {
        int pos = static_cast<int>(percentage * bar_width);
        std::cout << "[";
        for (int i = 0; i < bar_width; ++i)
        {
            if (i < pos)
                std::cout << "#";
            else
                std::cout << "-";
        }
        std::cout << "]";
    }

public:
    ProgressBar(const std::string &_prefix) : prefix(_prefix) {}

    void set_progress(float value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!is_stop)
        {
            percentage = value / 100.0f;
            if (percentage >= 1.0f)
            {
                is_complete = true; // Mark as complete when it reaches 100%
                is_stop = true;     // Automatically stop once completed
            }
        }
    }

    float get_progress()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return percentage * 100.0f;
    }

    bool stop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return is_stop;
    }

    bool complete()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return is_complete;
    }

    void stop_progress() // Manual stop method
    {
        std::lock_guard<std::mutex> lock(mutex_);
        is_stop = true;
    }

    void print_progress()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << prefix << " ";
        write_();
        std::cout << " " << std::fixed << std::setprecision(2) << (percentage * 100.0f) << "%" << postfix << std::endl;
    }

    void move_up(int lines)
    {
        std::cout << "\033[" << lines << "A"; // ANSI escape sequence for moving the cursor up
    }
};

// thread_data struct definition
struct thread_data
{
    unsigned short id;
    ProgressBar *bar;

    std::string url_;
    std::string filename_;
    std::string outdir;
};

#endif
