// Credit: https://github.com/TechHara/cpp_channel

#pragma once

#include <iostream>
#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>

template <typename T>
class AsyncChannel;

// The interface to create channel
template <typename T>
std::pair<AsyncChannel<T>, AsyncChannel<T>> channel() {
    auto state = std::make_shared<typename AsyncChannel<T>::AsyncChannelState>();

    AsyncChannel<T> sender(state, true);
    AsyncChannel<T> receiver(std::move(state), false);

    std::cerr << "[AsyncChannel] DEBUG: AsyncChannel has been established" << "\n";

    return {
        std::move(sender),
        std::move(receiver)
    };
}

/*
    NOTE: AsyncChannel is not intended to be used as an instance.
    Be aware the constructor is private, It means the class is
    suppose to be created by the call of 'channel()'
 */
template <typename T>
class AsyncChannel {
public:
    AsyncChannel(AsyncChannel const&) = delete;
    AsyncChannel(AsyncChannel&&) = default;
    AsyncChannel& operator = (AsyncChannel const&) = delete;
    AsyncChannel& operator = (AsyncChannel&&) = default;

    bool send(T item) {
        bool success = true;

        if (!is_sender_ || state_ == nullptr)
        {
            success = false;
        }
        else
        {
            std::lock_guard lock(state_->mtx);

            if (state_->closed)
            {
                success = false;
            }
            else
            {
                state_->queue.push(std::move(item));
                state_->cv.notify_one();
            }
        }

        return success;
    }

    std::optional<T> recv() {
        bool success = true;
        std::optional<T> item = std::nullopt;

        if (is_sender_ || state_ == nullptr)
        {
            success = false;
        }
        else
        {
            std::unique_lock lock(state_->mtx);

            state_->cv.wait(lock, [this] {
                return !state_->queue.empty() || state_->closed;
            });

            if (state_->queue.empty())
            {
                success = false;
            }
            else
            {
                item = std::move(state_->queue.front());
                state_->queue.pop();
            }
        }

        return success ? item : std::nullopt;
    }

    bool closed() {
        if (!state_)
        {
            return true;
        }

        std::lock_guard lock(state_->mtx);
        return state_->closed;
    }

    void close() {
        if (state_ != nullptr)
        {
            std::lock_guard lock(state_->mtx);

            if (!state_->closed) {
                state_->closed = true;
                state_->cv.notify_all();

                std::cerr << "[AsyncChannel] DEBUG: AsyncChannel has been closed" << "\n";
            }
        }
    }

    ~AsyncChannel() { close(); }

private:
    using AsyncChannelState = struct {
        std::queue<T> queue;
        std::mutex mtx;
        std::condition_variable cv;
        bool closed = false;
    };

    std::shared_ptr<AsyncChannelState> state_;
    bool is_sender_;

    explicit AsyncChannel(std::shared_ptr<AsyncChannelState> state, bool is_sender)
        : state_{std::move(state)}
        , is_sender_{is_sender} {}

    friend std::pair<AsyncChannel<T>, AsyncChannel<T>> channel<T>();
};
