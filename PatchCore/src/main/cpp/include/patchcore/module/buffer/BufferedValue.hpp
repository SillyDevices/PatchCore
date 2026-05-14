#ifndef PATCHCORE_BUFFEREDVALUE_HPP
#define PATCHCORE_BUFFEREDVALUE_HPP

#include <vector>

class BufferedValue {
public:
    explicit BufferedValue(float initialValue = 0.0f): scalarValue(initialValue) {}
    BufferedValue(const BufferedValue& other) = default;
    BufferedValue(BufferedValue&& other) noexcept = default;
    BufferedValue& operator=(BufferedValue&& other) noexcept = default;

    BufferedValue& operator=(const BufferedValue& other) {
        if (this != &other) {
            set(other.get());
        }
        return *this;
    }

    BufferedValue& operator=(float newValue) {
        set(newValue);
        return *this;
    }

    BufferedValue& operator+=(float delta) {
        set(get() + delta);
        return *this;
    }

    BufferedValue& operator-=(float delta) {
        set(get() - delta);
        return *this;
    }

    BufferedValue& operator*=(float factor) {
        set(get() * factor);
        return *this;
    }

    BufferedValue& operator/=(float factor) {
        set(get() / factor);
        return *this;
    }

    operator float() const {
        return get();
    }

    void onStartBuffer(int size) {
        float initialValue = get();
        if (size <= 0) {
            buffer.clear();
            scalarValue = initialValue;
            currentIndex = 0;
            return;
        }
        buffer.assign(size, initialValue);
        currentIndex = 0;
    }

    void advanceSample() {
        if (buffer.empty()) {
            return;
        }
        if (currentIndex + 1 < static_cast<int>(buffer.size())) {
            buffer[currentIndex + 1] = buffer[currentIndex];
            currentIndex++;
        }
    }

    const std::vector<float>& getBuffer() const {
        return buffer;
    }

    float* data() {
        return buffer.data();
    }

    const float* data() const {
        return buffer.data();
    }

    int getCurrentIndex() const {
        return currentIndex;
    }

private:
    float get() const {
        if (buffer.empty()) {
            return scalarValue;
        }
        return buffer[currentIndex];
    }

    void set(float newValue) {
        if (buffer.empty()) {
            scalarValue = newValue;
        } else {
            buffer[currentIndex] = newValue;
        }
    }

private:
    float scalarValue = 0.0f;
    std::vector<float> buffer = {};
    int currentIndex = 0;
};

#endif //PATCHCORE_BUFFEREDVALUE_HPP
