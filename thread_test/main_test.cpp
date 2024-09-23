#include <iostream>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <curl/curl.h>

// Progress bar namespace and details
namespace indicators
{
    // cursor_movement
#ifdef _MSC_VER
    // Not available yet
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
                for (size_t i = 0; i < bar_width;)
                {
                    if (i < pos)
                        os << fill;
                    else if (i == pos)
                        os << lead;
                    else
                        os << remainder;
                    i++;
                }
                return os;
            }

        private:
            std::ostream &os;
            size_t bar_width;
            std::string fill;
            std::string lead;
            std::string remainder;
        };

        enum class ProgressBarOption
        {
            bar_width = 0,
            prefix_text,
            completed,
            show_percentage,
            saved_stated_time,
            min_progress,
            max_progress,
            stream
        };
    }

    class ProgressBar
    {
    private:
        template <details::ProgressBarOption id>
        auto get_value()
            -> decltype((details::get_value<id>(std::declval<Settings &>()).value))
        {
            return details::get_value<id>(settings_).value;
        }

        template <details::ProgressBarOption id>
        auto get_value() const -> decltype((details::get_value<id>(std::declval<const Settings &>()).value))
        {
            return details::get_value<id>(settings_).value;
        }
        std::pair<std::string, int> get_prefix_text()
        {
            std::stringstream os;
            os << get_value<details::ProgressBarOption::prefix_text>();
            const auto result = os.str();
            const auto result_size = unicode::display_width(result);
            return {result, result_size};
        }

    public:
        explicit ProgressBar(const std::string &prefix = "")
            : prefix_(prefix), progress_(0) {}

        // Delete copy constructor and assignment operator
        ProgressBar(const ProgressBar &) = delete;
        ProgressBar &operator=(const ProgressBar &) = delete;

        // Default move constructor and assignment operator
        ProgressBar(ProgressBar &&other) noexcept = default;
        ProgressBar &operator=(ProgressBar &&other) noexcept = default;

        void set_progress(size_t new_progress)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            progress_ = new_progress;
        }

        size_t get_progress() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return progress_;
        }

        bool is_completed() const
        {
            return get_value<details::ProgressBarOption::completed>();
        }

        void mark_as_completed()
        {
            get_value<details::ProgressBarOption::completed>() = true;
            print_progress();
        }

        void print_progress(bool from_multi_progress = false)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::cout << prefix_ << " ";
            details::ProgressScaleWriter writer{std::cout, 50, "#", ">", " "};
            writer.write(static_cast<float>(progress_));
            std::cout << " " << progress_ << "%\n";
        }

    private:
        std::atomic<size_t> progress_;
        std::string prefix_;
        mutable std::mutex mutex_;
    };

    class MultiProgress
    {
    public:
        MultiProgress(std::vector<std::reference_wrapper<ProgressBar>> bars)
            : bars_(bars), started_(false) {}

        void print_progress()
        {
            std::lock_guard<std::mutex> lock{mutex_};
            if (started_)
                move_up(bars_.size());
            for (auto &bar : bars_)
            {
                bar.get().print_progress(true);
            }
            if (!started_)
                started_ = true;
        }

    private:
        std::atomic<bool> started_;
        std::mutex mutex_;
        std::vector<std::reference_wrapper<ProgressBar>> bars_;
    };
}

// ------------------------- main begin here ---------------
#define NUM_THREADS 2

// for downloading files
struct thread_data
{
    int tid_short;
    unsigned long int tid_;
    std::string url_;
    std::string filename_;
    std::string outdir;
    indicators::ProgressBar *bar;
};

int download_progress_callback(void *arg, curl_off_t dltotal,
                               curl_off_t dlnow, curl_off_t ultotal,
                               curl_off_t ulnow)
{
    static bool default_callback = false;
    if (default_callback)
    {
        CURL_PROGRESSFUNC_CONTINUE;
    }
    else
    {
        indicators::ProgressBar *bar = static_cast<indicators::ProgressBar *>(arg);
        if (bar->is_completed())
        {
            ;
        }
        else if (dltotal == 0)
        {
            progress_bar->set_progress(0);
        }
    }
}

void *update_progress(void *arg)
{
    thread_data *tdata = static_cast<thread_data *>(arg);
    indicators::ProgressBar *bar = tdata->bar;

    for (size_t i = 0; i <= 100; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        i += tdata->tid_short;
        bar->set_progress(i);
    }
    return nullptr;
}

void *pull_one_url(void *arg)
{
    thread_data *tdata = static_cast<thread_data *>(arg);

    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(tdata->filename_.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, tdata->url_.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, download_progress_callback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, static_cast<void *>(tdata->bar));
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

        // perform a file transfer synchoronously.
        res = curl_easy_perform(curl); /* ignores error */
        curl_easy_cleanup(curl);

        // close openfile
        fclose(fp);
    }

    return NULL;
}

std::vector<thread_data> test_parsed_handle(std::vector<std::unique_ptr<indicators::ProgressBar>> &bars)
{
    std::vector<std::string> urls;
    std::vector<std::string> filenames;
    std::string outdir = "./";

    urls.push_back("http://localhost:8000/aa1.txt");
    urls.push_back("http://localhost:8000/aa2.txt");

    filenames.push_back("aa1.txt");
    filenames.push_back("aa2.txt");

    // Initialize thread_data for each file
    thread_data val1;
    val1.tid_short = 0;
    val1.url_ = urls[0];
    val1.filename_ = filenames[0];
    val1.outdir = outdir;
    val1.bar = bars[0].get();

    thread_data val2;
    val2.tid_short = 1;
    val2.url_ = urls[1];
    val2.filename_ = filenames[1];
    val2.outdir = outdir;
    val2.bar = bars[1].get();

    return {val1, val2};
}

int main(int argc, char *argv[])
{
    // Create two progress bars using unique_ptr
    std::vector<std::unique_ptr<indicators::ProgressBar>> bars;
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        bars.push_back(std::make_unique<indicators::ProgressBar>("File " + std::to_string(i + 1)));
    }

    // Create a MultiProgress manager
    indicators::MultiProgress mp{std::vector<std::reference_wrapper<indicators::ProgressBar>>{*bars[0], *bars[1]}};

    // Parse the thread data
    std::vector<thread_data> tdatas = test_parsed_handle(bars);

    // Create threads for each download
    std::vector<pthread_t> threads(NUM_THREADS);

    for (size_t i = 0; i < NUM_THREADS; ++i)
    {
        int error = pthread_create(&threads[i], nullptr, update_progress, static_cast<void *>(&tdatas[i]));
    }

    // Continuously print multi progress bars until all are complete
    bool all_done = false;
    while (!all_done)
    {
        mp.print_progress();
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Refresh rate

        // Check if all bars are complete
        all_done = true;
        for (const auto &bar : bars)
        {
            if (bar->get_progress() < 100)
            {
                all_done = false;
                break;
            }
        }
    }

    // Join all threads
    for (size_t i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], nullptr);
    }

    std::cout << "All downloads complete!" << std::endl;

    return 0;
}