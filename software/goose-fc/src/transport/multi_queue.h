#pragma once

#include <variant>
#include "QueueCPP.h"
#include "MutexCPP.h"

template<typename TYPE, size_t LENGTH, typename... T>
class MultiQueue {
	using Data = std::variant<T...>;

	struct Element {
		TYPE type;
		Data data;
	};

	Queue<Element, LENGTH> queue;

	Mutex lock;
	Data value;

public:
	MultiQueue(const char *name) : queue(name), lock{"read lock"} {

	}

	bool add(const TYPE type, const Data data, const TickType_t timeout) {
		return queue.add(Element{type, data}, timeout);
	}

	bool pop(TYPE &type, const TickType_t timeout) {
		Element element;

		if(!queue.pop(element, timeout)) {
			return false;
		}

		lock.take(timeout);
		type = element.type;
		value = element.data;
		return true;
	}

	template<typename U>
	void getValue(U &dest) {
		const U *ptr = std::get_if<U>(&value);

		if(ptr) {
			dest = *ptr;
		}

		lock.give();
	}
};
