#include "header.h"
#include <future>

// Shared mutex for controlling synchronized output
std::mutex print_mutex;

void update_progress(thread_data *tdata)
{
    ProgressBar *bar = tdata->bar;

    try
    {
        for (size_t i = 0; i <= 100; i += tdata->id)
        {
            {
                std::lock_guard<std::mutex> lock(print_mutex);

                // Check if the progress should stop
                if (bar->stop())
                    return;

                // Simulate a runtime error if Thread 1 reaches 67%
                if (tdata->id == 1 && i >= 67)
                    throw std::runtime_error("Thread 1 encountered a runtime error at 67%!");

                bar->set_progress(i);
            }

            // Simulate work by sleeping for 50 milliseconds
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    catch (const std::exception &e)
    {
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cerr << "Exception caught in Thread " << tdata->id << ": " << e.what() << std::endl;
        }
        bar->stop_progress(); // Ensure the bar stops if an exception is caught
    }
}
int main()
{
    const int nums_thread = 2;
    ProgressBar bar1("Thread 1");
    ProgressBar bar2("Thread 2");

    thread_data tdata1 = {1, &bar1};
    thread_data tdata2 = {2, &bar2};

    // Packaged tasks for each thread
    std::packaged_task<void(thread_data *)> task1(update_progress);
    std::packaged_task<void(thread_data *)> task2(update_progress);

    // Futures to capture the result of the tasks
    auto result1 = task1.get_future();
    auto result2 = task2.get_future();

    // Start the threads
    std::thread thread1(std::move(task1), &tdata1);
    std::thread thread2(std::move(task2), &tdata2);

    bool all_stopped = false;

    // Continuously print progress bars until all are stopped
    while (!all_stopped)
    {
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            bar1.print_progress();
            bar2.print_progress();

            // Move the cursor up by 2 lines after printing both progress bars
            bar1.move_up(2);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Refresh rate

        // Check if Thread 1 threw an exception and stop if needed
        try
        {
            if (result1.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                result1.get(); // Handle exceptions from thread 1
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception caught from Thread 1: " << e.what() << std::endl;
            bar1.stop_progress(); // Ensure thread 1 stops if an exception is caught
        }

        // Check if Thread 2 threw an exception and stop if needed
        try
        {
            if (result2.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                result2.get(); // Handle exceptions from thread 2
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception caught from Thread 2: " << e.what() << std::endl;
            bar2.stop_progress(); // Ensure thread 2 stops if an exception is caught
        }

        // Check if all progress bars are either complete or manually stopped
        all_stopped = (bar1.stop() && bar2.stop());
    }

    // Final process printing
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        bar1.print_progress();
        bar2.print_progress();
    }

    std::cout << "All downloads complete!" << std::endl;

    // Join threads
    thread1.join();
    thread2.join();

    return 0;
}
