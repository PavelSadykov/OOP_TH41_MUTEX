//
//  main.cpp
//  ООП ДЗ 41 MUTEX
//
//  Created by Павел on 05.03.2023.
//
/*

Потокобезопасная очередь реализуется с помощью стандартного объекта очереди, защищенного мьютексом и условной переменной. Мьютекс используется для защиты доступа к очереди, а условная переменная  используется для уведомления ожидающих потоков об изменении очереди.

Класс thread_safe_queue определяет несколько методов доступа к очереди:

push(): добавляет элемент в заднюю часть очереди.
empty(): проверяет, пуста ли очередь.
try_pop(): удаляет элемент из передней части очереди, если он не пуст.
wait_and_pop(): удаляет элемент из передней части очереди, блокируя до тех пор, пока очередь не станет пустой.
front(): возвращает передний элемент очереди, не удаляя его.
В основной функции создается потокобезопасная очередь для хранения целых чисел, и порождаются два потока:

Поток-Producer, добавляющий целые числа в очередь каждые 100 миллисекунд.
Поток Customer, который считывает целые числа из очереди с помощью try_pop() каждые 10 миллисекунд и выводит их на консоль.
Поток-Produser выполняется до тех пор, пока он не добавит в очередь 10 целых чисел, а затем завершит работу. Consumer  работает бесконечно, проверяя очередь на наличие новых элементов каждые 10 миллисекунд.

Программа ожидает завершения обоих потоков перед завершением.
 
 На экране :
 Got value 0
 Got value 1
 Got value 2
 Got value 3
 Got value 4
 Got value 5
 Got value 6
 Got value 7
 Got value 8
 Got value 9

*/


#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>


template<typename T>
class thread_safe_queue {
public:
    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(value);
        cv_.notify_one();  // уведомляем один поток, что элемент добавлен
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool try_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        value = queue_.front();
        queue_.pop();
        return true;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            cv_.wait(lock);  // ждем, пока не будет уведомления
        }
        value = queue_.front();
        queue_.pop();
    }

    T front() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.front();
    }
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

int main(int argc, const char * argv[]) {
   
    thread_safe_queue<int> q;

    // Заполняем очередь из нескольких потоков
    std::thread producer([&q]() {
        for (int i = 0; i < 10; ++i) {
            q.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    // Читаем элементы из очереди в другом потоке
    std::thread consumer([&q]() {
        int value;
        while (true) {
            if (q.try_pop(value)) {
                std::cout << "Got value " << value << std::endl;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    });

    producer.join();
    consumer.join();

    
    
    return 0;
}
