#include <atomic> // Include for atomic flag
#include "header.h"
#include <future>
#include <curl/curl.h>
#include <sstream> // For building exception message

// Shared mutex for controlling synchronized output
std::mutex print_mutex;

std::atomic<bool> stop_all(false); // Shared atomic flag to stop all threads

// Exception-handling callback for libcurl with thread ID in exception
int download_progress_callback_interrupted(void *tdata_ptr, curl_off_t total_download, curl_off_t downloaded, curl_off_t, curl_off_t)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    // Revert static cast from void* back to thread_data*
    thread_data *tdata = static_cast<thread_data *>(tdata_ptr);
    ProgressBar *bar = tdata->bar;

    try
    {
        if (total_download > 0) // Prevent division by zero
        {
            float percentage = static_cast<float>(downloaded) / total_download * 100.0f;
            bar->set_progress(percentage);

            // // Condition to simulate a runtime error based on the download size and percentage
            // if (tdata->id == 1 && percentage > 20.0f)
            // {
            //     // Create error message with thread ID
            //     std::stringstream ss;
            //     ss << "Thread " << tdata->id << " encountered an error after 20% completion!";
            //     throw std::runtime_error(ss.str());
            // }
        }
    }
    catch (const std::exception &e)
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cerr << "Exception in progress callback (Thread " << tdata->id << "): " << e.what() << std::endl;
        return 1; // Returning non-zero will interrupt curl_easy_perform
    }

    return 0;
}

void *pull_one_url(void *arg)
{
    thread_data *tdata = static_cast<thread_data *>(arg);

    CURL *curl;
    FILE *fp = nullptr;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        std::string fullpath = tdata->outdir + "/" + tdata->filename_;
        fp = fopen(fullpath.c_str(), "wb");
        if (!fp)
        {
            std::cerr << "Error opening file: " << fullpath << std::endl;
            stop_all = true; // Signal all threads to stop
            return nullptr;
        }

        curl_easy_setopt(curl, CURLOPT_URL, tdata->url_.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, download_progress_callback_interrupted);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, static_cast<void *>(tdata)); // Pass tdata here
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

        try
        {
            // Perform file transfer synchronously, but check stop flag
            while (!stop_all)
            {
                res = curl_easy_perform(curl);
                if (res != CURLE_OK)
                {
                    std::lock_guard<std::mutex> lock(print_mutex);
                    std::cerr << "Download error (Thread " << tdata->id << "): " << curl_easy_strerror(res) << std::endl;
                    stop_all = true; // Signal all threads to stop
                    throw std::runtime_error("Download failed in Thread " + std::to_string(tdata->id));
                }
                break; // Exit loop if download is successful
            }
        }
        catch (const std::exception &e)
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cerr << "Exception caught in download thread (Thread " << tdata->id << "): " << e.what() << std::endl;
            tdata->bar->stop_progress(); // Stop progress on exception
            stop_all = true;             // Signal all threads to stop

            // Ensure file is closed in case of an exception
            if (fp)
            {
                fclose(fp);
                fp = nullptr; // Nullify pointer after closing
            }

            curl_easy_cleanup(curl);
            return nullptr; // Exit the function on exception
        }

        // Cleanup curl and close the file if no exception occurred
        curl_easy_cleanup(curl);

        if (fp)
        {
            fclose(fp); // Close file normally if no exception
        }
    }

    return nullptr;
}

int main()
{
    const int nums_thread = 2;
    ProgressBar bar1("Thread 1");
    ProgressBar bar2("Thread 2");

    thread_data tdata1 = {1, &bar1, "http://localhost:8000/a0_1M.txt", "a0_1M.txt", "."};
    thread_data tdata2 = {2, &bar2, "http://localhost:8000/a1_50M.txt", "a1_1M.txt", "."};

    std::vector<std::thread> threads;

    threads.emplace_back(pull_one_url, &tdata1);
    threads.emplace_back(pull_one_url, &tdata2);

    bool all_stopped = false;

    // Continuously print progress bars until all are completed or stopped
    while (!all_stopped && !stop_all) // Add stop_all check
    {
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            bar1.print_progress();
            bar2.print_progress();

            // Move the cursor up by 2 lines after printing both progress bars
            bar1.move_up(2);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Refresh rate

        // Check if both progress bars are stopped or stop_all is triggered
        all_stopped = (bar1.stop() && bar2.stop());
    }

    // Final process printing
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        bar1.print_progress();
        bar2.print_progress();
    }

    if (stop_all)
    {
        std::cout << "A thread encountered an error. Stopping all threads..." << std::endl;
    }
    else
    {
        std::cout << "All complete!" << std::endl;
    }

    // Join threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    // After joining, print success message if not interrupted
    if (!stop_all && bar1.complete())
    {
        std::cout << "Thread 1: Download saved successfully at: " << tdata1.outdir << "/" << tdata1.filename_ << std::endl;
    }

    if (!stop_all && bar2.complete())
    {
        std::cout << "Thread 2: Download saved successfully at: " << tdata2.outdir << "/" << tdata2.filename_ << std::endl;
    }

    return 0;
}

// ---------------------------------------- independent download with catching exception
// #include "header.h"
// #include <future>
// #include <curl/curl.h>
// #include <sstream> // For building exception message

// // Shared mutex for controlling synchronized output
// std::mutex print_mutex;

// // Exception-handling callback for libcurl with thread ID in exception
// int download_progress_callback_interrupted(void *tdata_ptr, curl_off_t total_download, curl_off_t downloaded, curl_off_t, curl_off_t)
// {
//     std::this_thread::sleep_for(std::chrono::milliseconds(2));

//     // Revert static cast from void* back to thread_data*
//     thread_data *tdata = static_cast<thread_data *>(tdata_ptr);
//     ProgressBar *bar = tdata->bar;

//     try
//     {
//         if (total_download > 0) // Prevent division by zero
//         {
//             float percentage = static_cast<float>(downloaded) / total_download * 100.0f;
//             bar->set_progress(percentage);

//             // Condition to simulate a runtime error based on the download size and percentage
//             if (tdata->id == 1 && percentage > 20.0f)
//             {
//                 // Create error message with thread ID
//                 std::stringstream ss;
//                 ss << "Thread " << tdata->id << " encountered an error after 20% completion!";
//                 throw std::runtime_error(ss.str());
//             }
//         }
//     }
//     catch (const std::exception &e)
//     {
//         std::lock_guard<std::mutex> lock(print_mutex);
//         std::cerr << "Exception in progress callback (Thread " << tdata->id << "): " << e.what() << std::endl;
//         return 1; // Returning non-zero will interrupt curl_easy_perform
//     }

//     return 0;
// }

// void *pull_one_url(void *arg)
// {
//     thread_data *tdata = static_cast<thread_data *>(arg);

//     CURL *curl;
//     FILE *fp = nullptr;
//     CURLcode res;

//     curl = curl_easy_init();
//     if (curl)
//     {
//         std::string fullpath = tdata->outdir + "/" + tdata->filename_;
//         fp = fopen(fullpath.c_str(), "wb");
//         if (!fp)
//         {
//             std::cerr << "Error opening file: " << fullpath << std::endl;
//             return nullptr;
//         }

//         curl_easy_setopt(curl, CURLOPT_URL, tdata->url_.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

//         // Pass the entire thread_data to the XFERINFODATA instead of just the ProgressBar
//         curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, download_progress_callback_interrupted);
//         curl_easy_setopt(curl, CURLOPT_XFERINFODATA, static_cast<void *>(tdata)); // Pass tdata here
//         curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

//         try
//         {
//             // Perform file transfer synchronously
//             res = curl_easy_perform(curl);

//             if (res != CURLE_OK)
//             {
//                 std::lock_guard<std::mutex> lock(print_mutex);
//                 std::cerr << "Download error (Thread " << tdata->id << "): " << curl_easy_strerror(res) << std::endl;
//                 tdata->bar->stop_progress(); // Stop progress in case of error
//             }
//         }
//         catch (const std::exception &e)
//         {
//             std::lock_guard<std::mutex> lock(print_mutex);
//             std::cerr << "Exception caught in download thread (Thread " << tdata->id << "): " << e.what() << std::endl;
//             tdata->bar->stop_progress(); // Stop progress on exception

//             // Ensure file is closed in case of an exception
//             if (fp)
//             {
//                 fclose(fp);
//                 fp = nullptr; // Nullify pointer after closing
//             }

//             // Cleanup curl
//             curl_easy_cleanup(curl);
//             return nullptr; // Exit the function on exception
//         }

//         // Cleanup curl and close the file if no exception occurred
//         curl_easy_cleanup(curl);

//         if (fp)
//         {
//             fclose(fp); // Close file normally if no exception
//         }
//     }

//     return nullptr;
// }

// int main()
// {
//     const int nums_thread = 2;
//     ProgressBar bar1("Thread 1");
//     ProgressBar bar2("Thread 2");

//     thread_data tdata1 = {1, &bar1, "http://localhost:8000/a0_1M.txt", "a0_1M.txt", "."};
//     thread_data tdata2 = {2, &bar2, "http://localhost:8000/a1_50M.txt", "a1_1M.txt", "."};

//     std::vector<std::thread> threads;

//     threads.emplace_back(pull_one_url, &tdata1);
//     threads.emplace_back(pull_one_url, &tdata2);

//     bool all_stopped = false;

//     // Continuously print progress bars until all are completed or stopped
//     while (!all_stopped)
//     {
//         {
//             std::lock_guard<std::mutex> lock(print_mutex);
//             bar1.print_progress();
//             bar2.print_progress();

//             // Move the cursor up by 2 lines after printing both progress bars
//             bar1.move_up(2);
//         }

//         std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Refresh rate

//         // Check if both progress bars are stopped
//         all_stopped = (bar1.stop() && bar2.stop());
//     }

//     // Final process printing
//     {
//         std::lock_guard<std::mutex> lock(print_mutex);
//         bar1.print_progress();
//         bar2.print_progress();
//     }

//     std::cout << "All complete!" << std::endl;

//     // Join threads
//     for (auto &thread : threads)
//     {
//         thread.join();
//     }

//     // Print success message after threads join
//     if (bar1.complete())
//     {
//         std::cout << "Thread 1: Download saved successfully at: " << tdata1.outdir << "/" << tdata1.filename_ << std::endl;
//     }
//     else
//     {
//         std::cout << "Thread 1: Download failed from url " << tdata1.url_ << std::endl;
//     }

//     if (bar2.complete())
//     {
//         std::cout << "Thread 2: Download saved successfully at: " << tdata2.outdir << "/" << tdata2.filename_ << std::endl;
//     }
//     else
//     {
//         std::cout << "Thread 1: Download failed from url " << tdata2.url_ << std::endl;
//     }

//     return 0;
// }

// -------------------------------------------------------
// #include "header.h"
// #include <future>
// #include <curl/curl.h>

// // Shared mutex for controlling synchronized output
// std::mutex print_mutex;

// void update_progress(thread_data *tdata)
// {
//     ProgressBar *bar = tdata->bar;

//     try
//     {
//         for (size_t i = 0; i <= 100; i += tdata->id)
//         {
//             {
//                 std::lock_guard<std::mutex> lock(print_mutex);

//                 // Check if the progress should stop
//                 if (bar->stop())
//                     return;

//                 // Simulate a runtime error if Thread 1 reaches 67%
//                 if (tdata->id == 1 && i >= 67)
//                     throw std::runtime_error("Thread 1 encountered a runtime error at 67%!");

//                 bar->set_progress(i);
//             }

//             // Simulate work by sleeping for 50 milliseconds
//             std::this_thread::sleep_for(std::chrono::milliseconds(50));
//         }
//     }
//     catch (const std::exception &e)
//     {
//         {
//             std::lock_guard<std::mutex> lock(print_mutex);
//             std::cerr << "Exception caught in Thread " << tdata->id << ": " << e.what() << std::endl;
//         }
//         bar->stop_progress(); // Ensure the bar stops if an exception is caught
//     }
// }

// // Progress callback function for libcurl
// int download_progress_callback(void *progress_bar, curl_off_t total_download, curl_off_t downloaded, curl_off_t, curl_off_t)
// {
//     std::this_thread::sleep_for(std::chrono::milliseconds(5));
//     ProgressBar *bar = static_cast<ProgressBar *>(progress_bar);

//     if (total_download > 0) // Prevent division by zero
//     {
//         float percentage = static_cast<float>(downloaded) / total_download * 100.0f;
//         bar->set_progress(percentage);
//     }
//     return 0;
// }

// void *pull_one_url(void *arg)
// {
//     thread_data *tdata = static_cast<thread_data *>(arg);

//     CURL *curl;
//     FILE *fp;
//     CURLcode res;

//     curl = curl_easy_init();
//     if (curl)
//     {
//         std::string fullpath = tdata->outdir + "/" + tdata->filename_;
//         fp = fopen(fullpath.c_str(), "wb");
//         if (!fp)
//         {
//             std::cerr << "Error opening file: " << fullpath << std::endl;
//             return nullptr;
//         }

//         curl_easy_setopt(curl, CURLOPT_URL, tdata->url_.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
//         curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, download_progress_callback);
//         curl_easy_setopt(curl, CURLOPT_XFERINFODATA, static_cast<void *>(tdata->bar));
//         curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

//         // Perform file transfer synchronously
//         res = curl_easy_perform(curl);

//         if (res != CURLE_OK)
//         {
//             std::cerr << "Download error: " << curl_easy_strerror(res) << std::endl;
//             tdata->bar->stop_progress(); // Stop progress in case of error
//         }

//         curl_easy_cleanup(curl);
//         fclose(fp);
//     }
//     return nullptr;
// }

// int main()
// {
//     const int nums_thread = 2;
//     ProgressBar bar1("Thread 1");
//     ProgressBar bar2("Thread 2");

//     thread_data tdata1 = {1, &bar1, "http://localhost:8000/a0_1M.txt", "a0_1M.txt", "."};
//     thread_data tdata2 = {2, &bar2, "http://localhost:8000/a1_50M.txt", "a1_1M.txt", "."};

//     std::vector<std::thread> threads;

//     threads.emplace_back(pull_one_url, &tdata1);
//     threads.emplace_back(pull_one_url, &tdata2);

//     bool all_stopped = false;

//     // Continuously print progress bars until all are completed or stopped
//     while (!all_stopped)
//     {
//         {
//             std::lock_guard<std::mutex> lock(print_mutex);
//             bar1.print_progress();
//             bar2.print_progress();

//             // Move the cursor up by 2 lines after printing both progress bars
//             bar1.move_up(2);
//         }

//         std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Refresh rate

//         // Check if both progress bars are stopped
//         all_stopped = (bar1.stop() && bar2.stop());
//     }

//     // Final process printing
//     {
//         std::lock_guard<std::mutex> lock(print_mutex);
//         bar1.print_progress();
//         bar2.print_progress();
//     }

//     std::cout << "All complete!" << std::endl;

//     // Join threads
//     for (auto &thread : threads)
//     {
//         thread.join();
//     }

//     return 0;
// }