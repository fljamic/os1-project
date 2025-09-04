#ifndef LIST_HPP
#define LIST_HPP

template<typename T>
class List {
private:
    struct Elem {
        T* data;
        Elem* next;

        Elem(T* dat, Elem* nxt) : data(dat), next(nxt) {}
    };

    Elem* head;
    Elem* tail;

    unsigned long size;
public:
    List() : head(0), tail(0), size(0) {}

    List(const List<T>&) = delete;

    List<T>& operator=(const List<T>&) = delete;

    unsigned long getSize() const { return size; }

    void addFirst(T* data) {
        Elem* elem = new Elem(data, head);
        head = elem;
        if (!tail)
            tail = head;

        ++size;
    }

    void addLast(T* data) {
        Elem* elem = new Elem(data, 0);
        if (tail) {
            tail->next = elem;
            tail = elem;
        }
        else
            head = tail = elem;

        ++size;
    }

    T* removeFirst() {
        if (!head) return 0;

        Elem* elem = head;
        head = head->next;
        if (!head)
            tail = 0;

        --size;

        T* ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekFirst() {
        if (!head) return 0;
        return head->data;
    }

    T* removeLast() {
        if (!head) return 0;

        Elem* prev = 0;
        for (Elem* curr = head; curr && curr != tail; curr = curr->next)
            prev = curr;

        Elem* elem = tail;
        if (prev)
            prev->next = 0;
        else
            head = 0;
        tail = prev;

        --size;

        T* ret = elem->data;
        delete elem;
        return ret;
    }

    T* peekLast() {
        if (!tail) return 0;
        return tail->data;
    }
};

#endif
